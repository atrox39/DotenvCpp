#include "dotenv.hpp"
#include<sstream>

void Dotenv::load(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Could not open the .env file: " << filename << std::endl;
    return;
  }
  std::string line;
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }
    size_t delimiterPos = line.find('=');
    size_t quotePos = line.find('"', delimiterPos + 1);
    if (quotePos != std::string::npos) {
      size_t endQuotePos = line.find('"', quotePos + 1);
      if (endQuotePos != std::string::npos) {
        line = line.substr(0, delimiterPos + 1) + line.substr(quotePos + 1, endQuotePos - quotePos - 1);
      }
    }
    if (delimiterPos != std::string::npos) {
      std::string key = line.substr(0, delimiterPos);
      std::string value = line.substr(delimiterPos + 1);
      #ifdef _WIN32
        _putenv_s(key.c_str(), value.c_str());
      #else
        setenv(key.c_str(), value.c_str(), 1);
      #endif
    }
  }
  file.close();
}

void CallDotenvLoad(const char* filename_c_str) {
  std::string filename_cpp_str = (filename_c_str != nullptr) ? filename_c_str : ".env";
  Dotenv::load(filename_cpp_str);
}
