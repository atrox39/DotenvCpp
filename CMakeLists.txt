cmake_minimum_required(VERSION 3.10)
project(DotenvCpp VERSION 2.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_SHARED_LIBS "Build shared library" ON)
option(BUILD_TESTS "Build test executable" ON)
option(INSTALL_HEADERS "Install header files" ON)

# Platform detection
if(WIN32)
    set(DOTENV_PLATFORM "windows")
elseif(APPLE)
    set(DOTENV_PLATFORM "macos")
elseif(UNIX)
    set(DOTENV_PLATFORM "linux")
else()
    set(DOTENV_PLATFORM "unknown")
endif()

message(STATUS "Building DotenvCpp for ${DOTENV_PLATFORM}")

# Source files
set(DOTENV_SOURCES
    src/dotenv.cpp
)

set(DOTENV_HEADERS
    src/dotenv.hpp
)

# Create library
if(BUILD_SHARED_LIBS)
    add_library(dotenv SHARED ${DOTENV_SOURCES})
    target_compile_definitions(dotenv PRIVATE BUILDING_DOTENV_DLL)
    
    # Set visibility for shared library
    if(NOT WIN32)
        set_target_properties(dotenv PROPERTIES
            CXX_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN YES
        )
    endif()
else()
    add_library(dotenv STATIC ${DOTENV_SOURCES})
endif()

# Set library properties
set_target_properties(dotenv PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${DOTENV_HEADERS}"
)

# Include directories
target_include_directories(dotenv
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(dotenv PRIVATE _CRT_SECURE_NO_WARNINGS)
    if(MSVC)
        target_compile_options(dotenv PRIVATE /W4)
    else()
        target_compile_options(dotenv PRIVATE -Wall -Wextra)
        target_link_options(dotenv PRIVATE -static-libgcc -static-libstdc++)
    endif()
else()
    target_compile_options(dotenv PRIVATE -Wall -Wextra -fPIC)
    find_package(Threads REQUIRED)
    target_link_libraries(dotenv PRIVATE Threads::Threads)
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    
    add_executable(dotenv_tests tests/test_dotenv.cpp)
    target_link_libraries(dotenv_tests PRIVATE dotenv)
    target_include_directories(dotenv_tests PRIVATE src)
    
    if(NOT WIN32)
        find_package(Threads REQUIRED)
        target_link_libraries(dotenv_tests PRIVATE Threads::Threads)
    endif()
    
    add_test(NAME DotenvTests COMMAND dotenv_tests)
    
    # Set RPATH for test executable
    if(UNIX AND NOT APPLE)
        set_target_properties(dotenv_tests PROPERTIES
            INSTALL_RPATH "$ORIGIN"
            BUILD_WITH_INSTALL_RPATH TRUE
        )
    elseif(APPLE)
        set_target_properties(dotenv_tests PROPERTIES
            INSTALL_RPATH "@loader_path"
            BUILD_WITH_INSTALL_RPATH TRUE
        )
    endif()
endif()

# Installation
include(GNUInstallDirs)

install(TARGETS dotenv
    EXPORT DotenvCppTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export targets for find_package support
install(EXPORT DotenvCppTargets
    FILE DotenvCppTargets.cmake
    NAMESPACE DotenvCpp::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DotenvCpp
)

# Create config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/DotenvCppConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_file(cmake/DotenvCppConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/DotenvCppConfig.cmake"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/DotenvCppConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/DotenvCppConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DotenvCpp
)

# Package configuration
set(CPACK_PACKAGE_NAME "DotenvCpp")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ library for loading environment variables from .env files")
set(CPACK_PACKAGE_VENDOR "atrox39")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
include(CPack)

# Print summary
message(STATUS "")
message(STATUS "DotenvCpp Configuration Summary:")
message(STATUS "  Version:        ${PROJECT_VERSION}")
message(STATUS "  Platform:       ${DOTENV_PLATFORM}")
message(STATUS "  Build type:     ${CMAKE_BUILD_TYPE}")
message(STATUS "  Shared library: ${BUILD_SHARED_LIBS}")
message(STATUS "  Build tests:    ${BUILD_TESTS}")
message(STATUS "")
