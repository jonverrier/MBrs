/////////////////////////////////////////
// Core.h
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

// Import / export for DLL linkage 
#if (defined WINXX)
   #ifdef CORE_EXPORTS
   #define CORE_API __declspec(dllexport)
   #else
   #define CORE_API __declspec(dllimport)
   #endif
#endif

// Package name for resource strings
#if (defined WINXX)
#ifdef NDEBUG
#define CORE_PACKAGE_NAME L"Corerw64.dll"
#else
#define CORE_PACKAGE_NAME L"Coredw64.dll"
#endif
#endif

#if (defined LINUX)
#ifdef NDEBUG
#error "Linux release package not yet defined."
#else
#error "Linux debug package not yet defined."
#endif
#endif
