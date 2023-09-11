#ifndef ISMRMRD_CPP03SHIM_H
#define ISMRMRD_CPP03SHIM_H
#pragma MESSAGE("cpp03shim.h is deprecated and will be removed in a future release. Please use a C++11 compiler.")
#if __cplusplus <= 199711L

#include <stdint.h>

#ifdef _MSC_VER
#ifndef __func__
#define __func__ __FUNCTION__
#endif
#endif

#define __unaligned
#define noexcept

#ifndef nullptr
#define nullptr NULL
#endif
namespace std {
using ::int16_t;
using ::int32_t;
using ::int64_t;
using ::int8_t;

using ::uint16_t;
using ::uint32_t;
using ::uint64_t;
using ::uint8_t;

#if !defined(_MSC_VER) || _MSC_VER <= 1600
template <typename T, size_t N>
T *begin(T (&array)[N]) {
    return array;
}

template <typename T, size_t N>
T *end(T (&array)[N]) {
    return array + N;
}
#endif
} // namespace std

#endif
#endif
