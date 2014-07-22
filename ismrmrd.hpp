//
//  ISMRMRD C++ Interface
//

#include "ismrmrd.h"

#pragma once
#ifndef ISMRMRD_HPP
#define ISMRMRD_HPP

namespace ISMRMRD {

class AcquisitionHeader : protected ISMRMRD_AcquisitionHeader {
public:
    explicit AcquisitionHeader();

    // Accessors and mutators
    const uint16_t &version();
    uint64_t &flags();

    // Flag methods
    bool isFlagSet(const uint64_t val);
    void setFlag(const uint64_t val);
    void clearFlag(const uint64_t val);
    void clearAllFlags();
};

class Acquisition : protected ISMRMRD_Acquisition {
public:
    explicit Acquisition();

    // Accessors and mutators
    const uint16_t &version();
    uint64_t &flags();

    // Flag methods
    bool isFlagSet(const uint64_t val);
    void setFlag(const uint64_t val);
    void clearFlag(const uint64_t val);
    void clearAllFlags();
};

class ImageHeader : protected ISMRMRD_ImageHeader {
    explicit ImageHeader();
    bool isFlagSet(const uint64_t val);
    void setFlag(const uint64_t val);
    void clearFlag(const uint64_t val);
    void clearAllFlags();
};

class Image : protected ISMRMRD_Image {
};

class NDArray : protected ISMRMRD_NDArray {
};

} // namespace ISMRMRD

#endif // ISMRMRD_HPP
