# Changelog

All notable changes to this project will be documented in this file.

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
