#pragma once
#include<fstream>
#include<string>
#include<map>
#include<iostream>
#include<cstdlib>

#ifdef BUILDING_DOTENV_DLL
  #define DOTENV_API __declspec(dllexport)
#else
  #define DOTENV_API __declspec(dllimport)
#endif

class DOTENV_API Dotenv {
public:
  static void load(const std::string& filename = ".env");
};

extern "C" {
  DOTENV_API void CallDotenvLoad(const char* filename_c_str);
}
