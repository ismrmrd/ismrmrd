#pragma once
#ifndef VSTYPES_H
#define VSTYPES_H


#ifdef __cplusplus

#if __cplusplus > 199711L
#include <cstdint>
#elif defined(_MSC_VER) && (_MSC_VER < 1600)
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned __int64 uint64_t;

typedef signed char int8_t;
typedef short int16_t;
typedef long int32_t;
typedef __int64 int64_t;
#else
#include <stdint.h>
#endif // __cplusplus > 199711L

#else

#if defined(_MSC_VER) && (_MSC_VER < 1600)
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned __int64 uint64_t;

typedef signed char int8_t;
typedef short int16_t;
typedef long int32_t;
typedef __int64 int64_t;

#ifndef UINT64_C
# if __WORDSIZE == 64
#  define UINT64_C(c)        c ## UL
# else
#  define UINT64_C(c)        c ## ULL
# endif
#endif

#else
#include <stdint.h>
#endif  //(_MSC_VER) && (_MSC_VER < 1600)

#endif // __cplusplus 


#if defined (_MSC_VER) && (_MSC_VER < 1800) /* old MS C compiler */
#ifndef false
#define false 0
#endif // false
#ifndef true
#define true 1
#endif // true
#endif

#endif // VSTYPES_H