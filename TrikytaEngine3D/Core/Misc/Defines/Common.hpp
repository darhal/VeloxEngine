#pragma once

//Include known-size integer files, based on compiler. Some compilers do not have these
//files, so they must be created manually.
#if defined(__GNUC__) || defined(__clang__) || (defined(_MSC_VER) && _MSC_VER >= 1600)
	// Compilers supporting C99 or C++0x have stdint.h defining these integer types	
	#include <stdint.h>
#elif defined(_MSC_VER)
	typedef signed   __int8  int8_t;
	typedef unsigned __int8  uint8_t;
	typedef signed   __int16 int16_t;
	typedef unsigned __int16 uint16_t;
	typedef signed   __int32 int32_t;
	typedef unsigned __int32 uint32_t;
	typedef signed   __int64 int64_t;
	typedef unsigned __int64 uint64_t;
	typedef uint64_t uintptr_t;
	typedef int64_t intptr_t;
	typedef int16_t wchar_t;
	#ifndef _INTPTR_T_DEFINED
		#define _INTPTR_T_DEFINED
		#ifdef  __x86_64__
			typedef int64_t intptr_t;
		#else
			typedef int32_t intptr_t;
		#endif
	#endif
#else
	// sizeof(char) == 1
	// sizeof(char) <= sizeof(short)
	// sizeof(short) <= sizeof(int)
	// sizeof(int) <= sizeof(long)
	// sizeof(long) <= sizeof(long long)
	// sizeof(char) * CHAR_BIT >= 8
	// sizeof(short) * CHAR_BIT >= 16
	// sizeof(int) * CHAR_BIT >= 16
	// sizeof(long) * CHAR_BIT >= 32
	// sizeof(long long) * CHAR_BIT >= 64

	typedef signed   char      int8_t;
	typedef unsigned char      uint8_t;
	typedef signed   short int int16_t;
	typedef unsigned short int uint16_t;
	typedef signed   int       int32_t;
	typedef unsigned int       uint32_t;
	typedef long long          int64_t;
	typedef unsigned long long uint64_t;
	typedef uint64_t uintptr_t;
	typedef int64_t intptr_t;
	typedef int16_t wchar_t;
	#ifdef  __x86_64__
		typedef int64_t intptr_t;
	#else
		typedef int32_t intptr_t;
	#endif
#endif

/*
 * Types that differ between LLP64 and LP64 architectures - in LLP64,
 * pointers are 64 bits, but 'long' is still 32 bits. Win64 appears
 * to be the only LLP64 architecture in current use.
*/
#ifdef _WIN64
		typedef signed   long long int ssize_t;
		typedef unsigned long long int usize_t;
#else
		typedef signed   long  int     ssize_t;
		typedef unsigned long  int     usize_t;
#endif

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint32_t uint;
typedef uint64_t uint64;
typedef intptr_t intptr;
typedef uintptr_t uintptr;


#ifdef COMPILER_MSVC
	#define FORCEINLINE __forceinline
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG)
	#define FORCEINLINE inline __attribute__ ((always_inline))
#else
	#define FORCEINLINE inline
#endif

#if __cplusplus < 199711L
	#define nullptr NULL
	#define CONSTEXPR
#else
	#define CONSTEXPR constexpr
#endif

#define NULL_COPY_AND_ASSIGN(T) \
	T(const T& other) {(void)other;} \
	void operator=(const T& other) { (void)other; }

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#define TRE_NS_START namespace TRE {
#define TRE_NS_END	 };