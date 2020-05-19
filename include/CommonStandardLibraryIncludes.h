/////////////////////////////////////////
// CommonStandardLibraryIncludes.h
// Pull in external include files
/////////////////////////////////////////

#ifndef COMMON_STANDARD_LIBRARY_INCLUDES_INCLUDED
#define COMMON_STANDARD_LIBRARY_INCLUDES_INCLUDED

#if (defined WINXX)

#pragma warning (disable : 4251) // 'Class 2' needs to have dll-interface to be used by clients of class 'Class 1'

#endif

// =========================================================================
// C/C++ Headers
// =========================================================================
#include <new>
#include <exception>
#include <typeinfo>
#include <string>
#include <iostream>
#include <sstream>

#pragma warning (push)
#pragma warning (disable: 4127)
#include <fstream>
#pragma warning (pop)

#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <limits.h>
#include <time.h>
#include <stdarg.h>
#include <process.h>
#include <vector>
#include <list>
#include <map>
#include <filesystem>

#if (defined WINXX || defined LINUX)
   #include <wchar.h>
   #include <tchar.h>
#endif

#ifndef NDEBUG
   #if (defined WINXX)
      #define _CRTDBG_MAP_ALLOC
      #include <crtdbg.h>
   #endif
#endif

// =========================================================================
// STL Headers
// =========================================================================

#include <algorithm>
#include <iterator>
#include <functional>
#include <list>
#include <map>
#include <vector>


#endif // COMMON_STANDARD_LIBRARY_INCLUDES_INCLUDED
