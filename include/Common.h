/////////////////////////////////////////
// Common.hpp
// Contains basic #defines and types common to all components
// Should be the first file included in any base module.
/////////////////////////////////////////

#ifndef COMMON_INCLUDED
#define COMMON_INCLUDED

#if ((!defined WINXX) && (!defined LINUX))
#error Must define one of WINXX or LINUX
#endif

#if ((!defined _DEBUG) && (!defined NDEBUG))
#error Must define one of _DEBUG or NDEBUG
#endif

#if (defined WINXX && defined LINUX)
#error Cannot define both WINXX and LINUX
#endif

#if ((defined _DEBUG) && (defined NDEBUG))
#error Cannot define both _DEBUG and NDEBUG
#endif

#if ! defined(lint)
#define M_UNREFERENCED_PARAMETER(P)          (P)
#else // lint
// Note: lint -e530 says don't complain about uninitialized variables for
// this varible.  Error 527 has to do with unreachable code.
// -restore restores checking to the -save state
#define UNREFERENCED_PARAMETER(P)          \
    /*lint -save -e527 -e530 */ \
    { \
        (P) = (P); \
    } \
    /*lint -restore */
#endif


// =========================================================================
// C/C++ Headers
// =========================================================================
#ifndef _HAS_AUTO_PTR_ETC
#define _HAS_AUTO_PTR_ETC 1 // Have to explicitly enable this for Exiv2
#endif

#include <new>
#include <exception>
#include <typeinfo>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <list>
#include <map>
#include <filesystem>
#include <algorithm>
#include <iterator>
#include <functional>
#include <memory>
#include <string>
#include <chrono>

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

// Internal Character type
typedef wchar_t          HChar;    /* c  */

// Internal String
typedef std::basic_string<wchar_t> HString;

typedef wchar_t          HChar;

// Signed type
typedef long             HInt;     /* i  */

// Unsigned type
typedef unsigned long    HUint;    /* u */

typedef size_t           HSize;

// Byte
typedef unsigned char    HByte;    /* b */

// Floating Point type
typedef float            HFloat;   /* flt */

// Handles
typedef void *           HHandle;  /* h */


// Quote text in a platform independent fashion
#define H__TEXT(c)     L##c
#define H_TEXT(c)      H__TEXT(c)

#define CRTCALL __cdecl

// Divide by zero tolerance
#define H_DIVIDEBYZEROTOLERANCE (HFloat(1.0e-07f))

// Macro for ANSI assert() style debugging.
// ============================================================

// use this macro instead of ANSI assert(). It returns TRUE if the expression is OK, else FALSE
// and it will behave OK in debug and non debug versions
//
// if (! H_ASSERT(expr))
//    return error

#define H_ASSERT(expr) \
if (!(expr)) { HostExceptionLogger::logAssertionFailure (H_TEXT(#expr), H_TEXT(__FILE__), __LINE__); throw exception();}

template <class AType> inline
AType upperOf(const AType& x1, const AType& x2)
{
   return (x1 > x2 ? x1 : x2);
}

template <class AType> inline
AType lowerOf(const AType& x1, const AType& x2)
{
   return (x1 < x2 ? x1 : x2);
}

template <class AType> inline
AType clamp (const AType& lo, const AType& x, const AType& hi)
{
   return ((x < lo) ? lo : ((x > hi) ? hi : x));
}

enum HErrorSource {kHost, kCore, kUILib, k3d, kNone};

#ifndef NDEBUG

#define COMMON_NEW                       new(_NORMAL_BLOCK, __FILE__ , __LINE__) 
#define COMMON_DELETE                    delete

#else

#define COMMON_NEW                       new
#define COMMON_DELETE                    delete

#endif

#define COMMON_STRING_BUFFER_SIZE 512

#if (defined WINXX)
   #pragma warning (disable : 4251) // 'Class 2' needs to have dll-interface to be used by clients of class 'Class 1'
#endif


#endif // COMMON_INCLUDED
