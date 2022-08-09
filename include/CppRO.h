#pragma once

#ifdef _WIN32
  #define CppRO_EXPORT __declspec(dllexport)
#else
  #define CppRO_EXPORT
#endif

CppRO_EXPORT void CppRO();
