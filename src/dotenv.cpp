#include "dotenv.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstring>

// Static member initialization
std::mutex Dotenv::s_mutex;
std::vector<std::string> Dotenv::s_loadedKeys;
bool Dotenv::s_isLoaded = false;
std::string Dotenv::s_lastError;

// Thread-safe buffer for C API string returns
static thread_local std::string s_cApiBuffer;

std::string Dotenv::trimWhitespace(const std::string& str) {
  size_t start = str.find_first_not_of(" \t\r\n");
  if (start == std::string::npos) return "";
  size_t end = str.find_last_not_of(" \t\r\n");
  return str.substr(start, end - start + 1);
}

std::string Dotenv::processValue(const std::string& value, const DotenvOptions& options) {
  std::string result = value;
  
  if (options.trimWhitespace) {
    result = trimWhitespace(result);
  }
  
  if (options.stripQuotes && result.length() >= 2) {
    // Handle double quotes
    if (result.front() == '"' && result.back() == '"') {
      result = result.substr(1, result.length() - 2);
    }
    // Handle single quotes
    else if (result.front() == '\'' && result.back() == '\'') {
      result = result.substr(1, result.length() - 2);
    }
  }
  
  // Process escape sequences (only for double-quoted strings originally)
  std::string processed;
  processed.reserve(result.size());
  bool escape = false;
  
  for (size_t i = 0; i < result.size(); ++i) {
    if (escape) {
      switch (result[i]) {
        case 'n': processed += '\n'; break;
        case 't': processed += '\t'; break;
        case 'r': processed += '\r'; break;
        case '\\': processed += '\\'; break;
        case '"': processed += '"'; break;
        case '\'': processed += '\''; break;
        default: 
          processed += '\\';
          processed += result[i];
          break;
      }
      escape = false;
    } else if (result[i] == '\\') {
      escape = true;
    } else {
      processed += result[i];
    }
  }
  
  if (escape) {
    processed += '\\';
  }
  
  return processed;
}

std::pair<std::string, std::string> Dotenv::parseLine(const std::string& line, const DotenvOptions& options) {
  std::string trimmedLine = trimWhitespace(line);
  
  // Skip empty lines and comments
  if (trimmedLine.empty() || trimmedLine[0] == '#') {
    return {"", ""};
  }
  
  // Find the first '=' delimiter
  size_t delimiterPos = trimmedLine.find('=');
  if (delimiterPos == std::string::npos) {
    return {"", ""};
  }
  
  std::string key = trimmedLine.substr(0, delimiterPos);
  if (options.trimWhitespace) {
    key = trimWhitespace(key);
  }
  
  // Validate key (must not be empty, must start with letter or underscore)
  if (key.empty()) {
    return {"", ""};
  }
  if (!std::isalpha(static_cast<unsigned char>(key[0])) && key[0] != '_') {
    return {"", ""};
  }
  
  std::string value = trimmedLine.substr(delimiterPos + 1);
  
  // Handle inline comments (not within quotes)
  bool inQuotes = false;
  char quoteChar = '\0';
  size_t commentPos = std::string::npos;
  
  for (size_t i = 0; i < value.length(); ++i) {
    char c = value[i];
    if (!inQuotes) {
      if (c == '"' || c == '\'') {
        inQuotes = true;
        quoteChar = c;
      } else if (c == '#') {
        // Check for space before # for inline comment
        if (i > 0 && (value[i-1] == ' ' || value[i-1] == '\t')) {
          commentPos = i - 1;
          break;
        }
      }
    } else {
      if (c == quoteChar && (i == 0 || value[i-1] != '\\')) {
        inQuotes = false;
      }
    }
  }
  
  if (commentPos != std::string::npos) {
    value = value.substr(0, commentPos);
  }
  
  value = processValue(value, options);
  
  return {key, value};
}

DotenvError Dotenv::load(const std::string& filename) {
  DotenvOptions defaultOptions;
  return load(filename, defaultOptions);
}

DotenvError Dotenv::load(const std::string& filename, const DotenvOptions& options) {
  std::lock_guard<std::mutex> lock(s_mutex);
  
  std::ifstream file(filename);
  if (!file.is_open()) {
    s_lastError = "Could not open the .env file: " + filename;
    std::cerr << s_lastError << std::endl;
    return DotenvError::FileNotFound;
  }
  
  s_lastError.clear();
  std::string line;
  int lineNumber = 0;
  
  while (std::getline(file, line)) {
    ++lineNumber;
    
    auto [key, value] = parseLine(line, options);
    
    if (key.empty()) {
      continue;
    }
    
    // Check if we should overwrite existing variables
    if (!options.overwrite) {
      #ifdef _WIN32
        size_t requiredSize;
        getenv_s(&requiredSize, nullptr, 0, key.c_str());
        if (requiredSize > 0) {
          continue;
        }
      #else
        if (std::getenv(key.c_str()) != nullptr) {
          continue;
        }
      #endif
    }
    
    // Set environment variable
    #if defined(_WIN32) || defined(_WIN64)
      _putenv_s(key.c_str(), value.c_str());
    #else
      setenv(key.c_str(), value.c_str(), 1);
    #endif
    
    // Track loaded keys
    if (std::find(s_loadedKeys.begin(), s_loadedKeys.end(), key) == s_loadedKeys.end()) {
      s_loadedKeys.push_back(key);
    }
  }
  
  file.close();
  s_isLoaded = true;
  
  return DotenvError::Success;
}

std::string Dotenv::get(const std::string& key, const std::string& defaultValue) {
  #if defined(_WIN32) || defined(_WIN64)
    char* value = nullptr;
    size_t len = 0;
    _dupenv_s(&value, &len, key.c_str());
    if (value != nullptr) {
      std::string result(value);
      free(value);
      return result;
    }
  #else
    const char* value = std::getenv(key.c_str());
    if (value != nullptr) {
      return std::string(value);
    }
  #endif
  return defaultValue;
}

bool Dotenv::has(const std::string& key) {
  #if defined(_WIN32) || defined(_WIN64)
    char* value = nullptr;
    size_t len = 0;
    _dupenv_s(&value, &len, key.c_str());
    if (value != nullptr) {
      free(value);
      return true;
    }
    return false;
  #else
    return std::getenv(key.c_str()) != nullptr;
  #endif
}

std::vector<std::string> Dotenv::getLoadedKeys() {
  std::lock_guard<std::mutex> lock(s_mutex);
  return s_loadedKeys;
}

void Dotenv::clear() {
  std::lock_guard<std::mutex> lock(s_mutex);
  
  for (const auto& key : s_loadedKeys) {
    #if defined(_WIN32) || defined(_WIN64)
      _putenv_s(key.c_str(), "");
    #else
      unsetenv(key.c_str());
    #endif
  }
  
  s_loadedKeys.clear();
  s_isLoaded = false;
}

bool Dotenv::isLoaded() {
  std::lock_guard<std::mutex> lock(s_mutex);
  return s_isLoaded;
}

std::string Dotenv::getLastError() {
  std::lock_guard<std::mutex> lock(s_mutex);
  return s_lastError;
}

// C API implementations
extern "C" {

int DotenvLoad(const char* filename) {
  std::string fname = (filename != nullptr) ? filename : ".env";
  return static_cast<int>(Dotenv::load(fname));
}

const char* DotenvGet(const char* key, const char* defaultValue) {
  if (key == nullptr) {
    s_cApiBuffer = "";
    return s_cApiBuffer.c_str();
  }
  std::string defVal = (defaultValue != nullptr) ? defaultValue : "";
  s_cApiBuffer = Dotenv::get(key, defVal);
  return s_cApiBuffer.c_str();
}

int DotenvHas(const char* key) {
  if (key == nullptr) return 0;
  return Dotenv::has(key) ? 1 : 0;
}

void DotenvClear(void) {
  Dotenv::clear();
}

int DotenvIsLoaded(void) {
  return Dotenv::isLoaded() ? 1 : 0;
}

const char* DotenvGetLastError(void) {
  s_cApiBuffer = Dotenv::getLastError();
  return s_cApiBuffer.c_str();
}

// Legacy function for backwards compatibility
void CallDotenvLoad(const char* filename_c_str) {
  std::string filename_cpp_str = (filename_c_str != nullptr) ? filename_c_str : ".env";
  Dotenv::load(filename_cpp_str);
}

} // extern "C"
