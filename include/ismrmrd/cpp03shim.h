#ifndef ISMRMRD_CPP03SHIM_H
#define ISMRMRD_CPP03SHIM_H

#if __cplusplus <= 199711L

#include <stdint.h>

#ifdef _MSC_VER
#ifndef __func__
    #define __func__ __FUNCTION__
#endif
#endif

#define __unaligned
#define noexcept
#define nullptr NULL

namespace std
{
    using ::int8_t;
    using ::int16_t;
    using ::int32_t;
    using ::int64_t;


    using ::uint8_t;
    using ::uint16_t;
    using ::uint32_t;
    using ::uint64_t;

    template<typename T, size_t N>
    T* begin(T (&array)[N]) {
        return array;
    }

    template<typename T, size_t N>
    T* end(T (&array)[N]) {
        return array + N;
    }
}

#endif
#endif
