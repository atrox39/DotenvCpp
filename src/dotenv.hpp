#pragma once
#ifndef DOTENV_HPP
#define DOTENV_HPP

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <mutex>

// Cross-platform DLL export/import macros
#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
  #ifdef BUILDING_DOTENV_DLL
    #ifdef __GNUC__
      #define DOTENV_API __attribute__((dllexport))
    #else
      #define DOTENV_API __declspec(dllexport)
    #endif
  #else
    #ifdef __GNUC__
      #define DOTENV_API __attribute__((dllimport))
    #else
      #define DOTENV_API __declspec(dllimport)
    #endif
  #endif
  #define DOTENV_LOCAL
#else
  #if __GNUC__ >= 4
    #define DOTENV_API __attribute__((visibility("default")))
    #define DOTENV_LOCAL __attribute__((visibility("hidden")))
  #else
    #define DOTENV_API
    #define DOTENV_LOCAL
  #endif
#endif

// Error codes
enum class DotenvError {
  Success = 0,
  FileNotFound = 1,
  ParseError = 2,
  InvalidKey = 3
};

// Options for loading
struct DotenvOptions {
  bool overwrite = true;       // Overwrite existing env vars
  bool interpolate = false;    // Interpolate variables ${VAR}
  bool stripQuotes = true;     // Strip quotes from values
  bool trimWhitespace = true;  // Trim whitespace from keys and values
};

class DOTENV_API Dotenv {
public:
  // Load environment variables from file
  static DotenvError load(const std::string& filename = ".env");
  static DotenvError load(const std::string& filename, const DotenvOptions& options);
  
  // Get an environment variable value (returns empty string if not found)
  static std::string get(const std::string& key, const std::string& defaultValue = "");
  
  // Check if an environment variable exists
  static bool has(const std::string& key);
  
  // Get all loaded environment variable keys
  static std::vector<std::string> getLoadedKeys();
  
  // Clear all loaded environment variables (only clears tracked ones)
  static void clear();
  
  // Check if the library is initialized (has loaded at least once)
  static bool isLoaded();
  
  // Get the error message for the last error
  static std::string getLastError();

private:
  static std::mutex s_mutex;
  static std::vector<std::string> s_loadedKeys;
  static bool s_isLoaded;
  static std::string s_lastError;
  
  static std::string trimWhitespace(const std::string& str);
  static std::string processValue(const std::string& value, const DotenvOptions& options);
  static std::pair<std::string, std::string> parseLine(const std::string& line, const DotenvOptions& options);
};

// C API for runtime DLL loading
// NOTE: String pointers returned by DotenvGet and DotenvGetLastError
// are valid until the next call to the same function. Copy the string
// immediately if you need to preserve the value.
extern "C" {
  DOTENV_API int DotenvLoad(const char* filename);
  DOTENV_API const char* DotenvGet(const char* key, const char* defaultValue);
  DOTENV_API int DotenvHas(const char* key);
  DOTENV_API void DotenvClear(void);
  DOTENV_API int DotenvIsLoaded(void);
  DOTENV_API const char* DotenvGetLastError(void);
  
  // Legacy function for backwards compatibility
  DOTENV_API void CallDotenvLoad(const char* filename_c_str);
}

#endif // DOTENV_HPP
