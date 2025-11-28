# Dotenv C++ Library

A cross-platform C++ library for loading environment variables from `.env` files. Designed for runtime DLL/shared library usage.

📖 **[View Full Documentation](https://atrox39.github.io/DotenvCpp/)**

## Features

- ✅ Load environment variables from `.env` files
- ✅ Cross-platform compatible (Windows/Linux/macOS)
- ✅ Available as a shared library (DLL/SO/DYLIB)
- ✅ C and C++ API support
- ✅ Thread-safe operations
- ✅ Support for quoted values (single and double quotes)
- ✅ Inline comments support
- ✅ Escape sequences (`\n`, `\t`, `\\`)
- ✅ Whitespace trimming
- ✅ Error handling with return codes
- ✅ CMake and Makefile build systems

## Building

### Requirements
- C++17 compatible compiler (g++, clang++, MSVC)
- CMake 3.10+ or GNU Make

### Build with CMake (Recommended)

```bash
mkdir build && cd build
cmake ..
cmake --build .

# Run tests
ctest --output-on-failure
```

### Build with Make

```bash
# Build the library
make

# Run tests
make test

# Clean build artifacts
make clean

# Install (Unix systems)
sudo make install
```

### Build Commands (Windows)

```bash
# Using MinGW
make

# Sign the DLL (optional)
powershell -ExecutionPolicy Bypass -File create-cert.ps1
make sign
```

## Usage

### C++ API

```cpp
#include "dotenv.hpp"
#include <iostream>

int main() {
    // Load environment variables from .env
    DotenvError result = Dotenv::load(".env");
    if (result != DotenvError::Success) {
        std::cerr << "Error: " << Dotenv::getLastError() << std::endl;
        return 1;
    }
    
    // Get environment variables with default values
    std::string dbUrl = Dotenv::get("DATABASE_URL", "localhost");
    std::string apiKey = Dotenv::get("API_KEY");
    
    // Check if a variable exists
    if (Dotenv::has("DEBUG")) {
        std::cout << "Debug mode enabled" << std::endl;
    }
    
    // Get all loaded keys
    auto keys = Dotenv::getLoadedKeys();
    
    // Clear all loaded variables
    Dotenv::clear();
    
    return 0;
}
```

### C API

```c
#include "dotenv.hpp"

int main() {
    // Load environment variables
    int result = DotenvLoad(".env");
    if (result != 0) {
        printf("Error: %s\n", DotenvGetLastError());
        return 1;
    }
    
    // Get variables
    const char* port = DotenvGet("PORT", "8080");
    
    // Check if variable exists
    if (DotenvHas("DEBUG")) {
        printf("Debug mode enabled\n");
    }
    
    // Clear loaded variables
    DotenvClear();
    
    return 0;
}
```

### Dynamic Runtime Loading

#### Windows
```cpp
#include <windows.h>
#include <iostream>

typedef int (*DotenvLoadFunc)(const char*);
typedef const char* (*DotenvGetFunc)(const char*, const char*);

int main() {
    HMODULE hDll = LoadLibrary("dotenv.dll");
    if (!hDll) {
        std::cerr << "Failed to load dotenv.dll" << std::endl;
        return 1;
    }

    auto loadFunc = (DotenvLoadFunc)GetProcAddress(hDll, "DotenvLoad");
    auto getFunc = (DotenvGetFunc)GetProcAddress(hDll, "DotenvGet");
    
    if (loadFunc && getFunc) {
        loadFunc(".env");
        std::cout << "API Key: " << getFunc("API_KEY", "") << std::endl;
    }
    
    FreeLibrary(hDll);
    return 0;
}
```

#### Linux/macOS
```cpp
#include <dlfcn.h>
#include <iostream>

typedef int (*DotenvLoadFunc)(const char*);
typedef const char* (*DotenvGetFunc)(const char*, const char*);

int main() {
    void* handle = dlopen("./libdotenv.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Failed to load library: " << dlerror() << std::endl;
        return 1;
    }

    auto loadFunc = (DotenvLoadFunc)dlsym(handle, "DotenvLoad");
    auto getFunc = (DotenvGetFunc)dlsym(handle, "DotenvGet");
    
    if (loadFunc && getFunc) {
        loadFunc(".env");
        std::cout << "API Key: " << getFunc("API_KEY", "") << std::endl;
    }
    
    dlclose(handle);
    return 0;
}
```

### .env File Format

```bash
# Comments start with #
DATABASE_URL=postgresql://localhost/mydb
API_KEY="your-api-key-here"
SECRET='single-quoted-value'
PORT=3000
DEBUG=true

# Inline comments
HOST=localhost # This is the server host

# Escape sequences (in double quotes)
MESSAGE="Line1\nLine2\tTabbed"

# Whitespace is trimmed
  KEY_WITH_SPACES  =  value with spaces trimmed  
```

## API Reference

### C++ API (Dotenv class)

| Method | Description |
|--------|-------------|
| `load(filename)` | Load variables from file, returns `DotenvError` |
| `load(filename, options)` | Load with custom options |
| `get(key, default)` | Get variable value with optional default |
| `has(key)` | Check if variable exists |
| `getLoadedKeys()` | Get list of all loaded keys |
| `clear()` | Clear all loaded variables |
| `isLoaded()` | Check if library has loaded a file |
| `getLastError()` | Get last error message |

### C API Functions

| Function | Description |
|----------|-------------|
| `DotenvLoad(filename)` | Load variables, returns 0 on success |
| `DotenvGet(key, default)` | Get variable value |
| `DotenvHas(key)` | Returns 1 if exists, 0 otherwise |
| `DotenvClear()` | Clear all loaded variables |
| `DotenvIsLoaded()` | Returns 1 if loaded, 0 otherwise |
| `DotenvGetLastError()` | Get last error message |
| `CallDotenvLoad(filename)` | Legacy function (backward compatible) |

### Error Codes (DotenvError)

| Code | Value | Description |
|------|-------|-------------|
| `Success` | 0 | Operation successful |
| `FileNotFound` | 1 | Could not open file |
| `ParseError` | 2 | Error parsing file |
| `InvalidKey` | 3 | Invalid key format |

## Output Files

| Platform | Library | Import Library |
|----------|---------|----------------|
| Windows | `dotenv.dll` | `libdotenv.a` |
| Linux | `libdotenv.so` | - |
| macOS | `libdotenv.dylib` | - |

## C# Example

```csharp
using System;
using System.Runtime.InteropServices;

class DotenvExample
{
    // Modern C API - Recommended
    [DllImport("dotenv.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int DotenvLoad(string filename);

    [DllImport("dotenv.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr DotenvGet(string key, string defaultValue);

    [DllImport("dotenv.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int DotenvHas(string key);

    [DllImport("dotenv.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void DotenvClear();

    [DllImport("dotenv.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int DotenvIsLoaded();

    [DllImport("dotenv.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr DotenvGetLastError();

    // Legacy function (for backwards compatibility)
    [DllImport("dotenv.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void CallDotenvLoad(string filename);

    public static void Main()
    {
        // Load environment variables from .env file
        int result = DotenvLoad(".env");
        if (result != 0)
        {
            IntPtr errorPtr = DotenvGetLastError();
            string error = Marshal.PtrToStringAnsi(errorPtr);
            Console.WriteLine("Error loading .env: {0}", error);
            return;
        }

        // Check if loaded successfully
        if (DotenvIsLoaded() == 1)
        {
            Console.WriteLine("Environment variables loaded successfully!");
        }

        // Get environment variable with default value
        IntPtr valuePtr = DotenvGet("CONNECTION_STRING", "default_connection");
        string connectionString = Marshal.PtrToStringAnsi(valuePtr);
        Console.WriteLine("Connection string: {0}", connectionString);

        // Check if a variable exists
        if (DotenvHas("API_KEY") == 1)
        {
            IntPtr apiKeyPtr = DotenvGet("API_KEY", "");
            string apiKey = Marshal.PtrToStringAnsi(apiKeyPtr);
            Console.WriteLine("API Key: {0}", apiKey);
        }

        // You can also use standard .NET methods after loading
        string dbUrl = Environment.GetEnvironmentVariable("DATABASE_URL");
        Console.WriteLine("Database URL from Environment: {0}", dbUrl);

        // Clear loaded variables when done
        DotenvClear();
    }
}
```

> **Note:** On Linux, use `libdotenv.so` instead of `dotenv.dll`. On macOS, use `libdotenv.dylib`.

See also: [C# Example Gist](https://gist.github.com/atrox39/6b214f17e9f28668fad6a2b556b690bc)

## License

MIT License - see [LICENSE](LICENSE) file for details.
