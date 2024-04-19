#ifndef ISMRMRD_CPP98_H
#define ISMRMRD_CPP98_H

#ifdef __cplusplus

namespace ISMRMRD {
    // begin and end are used in comparisons operators but are not
    // consistently defined in all supported C++ versiona and platforms
    template <typename T, size_t N>
    T *begin(T (&array)[N]) {
        return array;
    }

    template <typename T, size_t N>
    T *end(T (&array)[N]) {
        return array + N;
    }
}

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

namespace ISMRMRD {
using ::int16_t;
using ::int32_t;
using ::int64_t;
using ::int8_t;

using ::uint16_t;
using ::uint32_t;
using ::uint64_t;
using ::uint8_t;
}
#else
#include <cstdint>
namespace ISMRMRD {
using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::int8_t;

using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::uint8_t;
}

#endif // __cplusplus <= 98
#endif // __cplusplus
#endif // ISMRMRD_CPP98_H
