# Dotenv C++ Library

A simple C++ library for loading environment variables from `.env` files.

📖 **[View Full Documentation](https://atrox39.github.io/DotenvCpp/)**

## Features

- Load environment variables from `.env` files
- Support for quoted values
- Cross-platform compatible (Windows/Linux)
- Available as a shared library (DLL)
- C and C++ API support

## Building

### Requirements
- g++ compiler with C++11 support
- Make

### Build Commands

Build the library:
```bash
make bin/dotenv.dll
```

Sign the DLL (Windows only):
```bash
# First, create a test certificate
powershell -ExecutionPolicy Bypass -File create-cert.ps1

# Then sign the DLL
make sign
```

Clean build artifacts:
```bash
make clean
```

## Usage

### C++ API

```cpp
#include "dotenv.hpp"

int main() {
    Dotenv::load(".env");  // Load from .env file
    // Your environment variables are now loaded
    return 0;
}
```

### C API

```cpp
#include "dotenv.hpp"

int main() {
    CallDotenvLoad(".env");  // Load from .env file
    // Your environment variables are now loaded
    return 0;
}
```

### Dynamic Runtime Loading

```cpp
#include <windows.h>
#include <iostream>

typedef void (*CallDotenvLoadFunc)(const char*);

int main() {
    // Load the DLL at runtime
    HMODULE hDll = LoadLibrary("dotenv.dll");
    if (!hDll) {
        std::cerr << "Failed to load dotenv.dll" << std::endl;
        return 1;
    }

    // Get the function pointer
    CallDotenvLoadFunc loadFunc = (CallDotenvLoadFunc)GetProcAddress(hDll, "CallDotenvLoad");
    if (!loadFunc) {
        std::cerr << "Failed to find CallDotenvLoad function" << std::endl;
        FreeLibrary(hDll);
        return 1;
    }

    // Call the function
    loadFunc(".env");
    
    // Your environment variables are now loaded
    
    // Clean up
    FreeLibrary(hDll);
    return 0;
}
```

### .env File Format

```
# Comments start with #
DATABASE_URL=postgresql://localhost/mydb
API_KEY="your-api-key-here"
PORT=3000
```

## Output Files

- `bin/dotenv.dll` - The shared library
- `bin/libdotenv.a` - Import library for linking
- `bin/dotenv.o` - Object file

# C# Example usage from my gist
[Example](https://gist.github.com/atrox39/6b214f17e9f28668fad6a2b556b690bc)

## License

MIT License - see [LICENSE](LICENSE) file for details.
