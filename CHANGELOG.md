# Changelog

All notable changes to this project will be documented in this file.

## [2.0.0] - 2025-11-27

### Added
- Cross-platform support for Linux and macOS (in addition to Windows)
- CMake build system support
- Thread-safe operations with mutex protection
- New C++ API methods:
  - `Dotenv::get()` - Get variable with default value
  - `Dotenv::has()` - Check if variable exists
  - `Dotenv::getLoadedKeys()` - Get all loaded keys
  - `Dotenv::clear()` - Clear loaded variables
  - `Dotenv::isLoaded()` - Check if loaded
  - `Dotenv::getLastError()` - Get error message
- New C API functions:
  - `DotenvLoad()` - Load with return code
  - `DotenvGet()` - Get variable with default
  - `DotenvHas()` - Check if exists
  - `DotenvClear()` - Clear variables
  - `DotenvIsLoaded()` - Check loaded state
  - `DotenvGetLastError()` - Get error message
- `DotenvError` enum for error handling
- `DotenvOptions` struct for load configuration
- Escape sequence support (`\n`, `\t`, `\\`, `\"`, `\'`)
- Single quote support for values
- Inline comments support (after space)
- Whitespace trimming for keys and values
- Unit test infrastructure
- Install/uninstall targets for Unix systems

### Changed
- Upgraded from C++11 to C++17
- Improved Makefile with automatic platform detection
- Better cross-platform API export macros
- `Dotenv::load()` now returns `DotenvError` instead of void

### Fixed
- Proper visibility attributes for Linux/macOS shared libraries
- Cross-platform environment variable handling

## [1.0.0] - 2025-11-27

### Added
- Initial release of Dotenv C++ Library
- Load environment variables from `.env` files
- Support for quoted values in environment variables
- Cross-platform compatibility (Windows/Linux)
- Shared library (DLL) distribution
- C++ API with `Dotenv::load()` function
- C API with `CallDotenvLoad()` function
- Makefile build system with dynamic pattern rules
- Code signing support for Windows DLL
- Self-signed certificate generation scripts (`create-cert.ps1` and `create-cert.sh`)
- Comprehensive README with usage examples
- Dynamic runtime loading example for Windows
- MIT License
- `.gitignore` for build artifacts and certificates
- Clean build target

### Build System
- Makefile with `bin/dotenv.dll` build target
- Automatic object file compilation from source
- Import library generation (`libdotenv.a`)
- Windows and Linux conditional compilation
- `make sign` target for DLL code signing
- `make clean` target for removing build artifacts

### Documentation
- README with building instructions
- Usage examples for C++ API, C API, and dynamic loading
- `.env` file format documentation
- Code signing instructions
- License file (MIT)

### Files
- `src/dotenv.cpp` - Main implementation
- `src/dotenv.hpp` - Header with API definitions
- `Makefile` - Build configuration
- `create-cert.ps1` - PowerShell script to generate test certificates
- `create-cert.sh` - Bash script to generate test certificates
- `README.md` - Project documentation
- `LICENSE` - MIT License
- `.gitignore` - Git ignore rules
