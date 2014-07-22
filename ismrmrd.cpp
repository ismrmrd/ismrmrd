#include "ismrmrd.hpp"

namespace ISMRMRD {

AcquisitionHeader::AcquisitionHeader() {
    ismrmrd_init_acquisition_header(this);
};

bool AcquisitionHeader::isFlagSet(const uint64_t val) {
    return ismrmrd_is_flag_set(this->flags, val);
};

void AcquisitionHeader::setFlag(const uint64_t val) {
    ismrmrd_set_flag(&(this->flags), val);
};

void AcquisitionHeader::clearFlag(const uint64_t val) {
    ismrmrd_clear_flag(&(this->flags), val);
};

void AcquisitionHeader::clearAllFlags() {
    ismrmrd_clear_all_flags(&(this->flags));
};

Acquisition::Acquisition() {
    ismrmrd_init_acquisition(this);
};

const uint16_t &Acquisition::version() {
    return (this->head.version);
};
uint64_t &Acquisition::flags() {
    return (this->head.flags);
};

bool Acquisition::isFlagSet(const uint64_t val) {
    return ismrmrd_is_flag_set((this->head.flags), val);
};
void Acquisition::setFlag(const uint64_t val) {
    ismrmrd_set_flag(&(this->head.flags), val);
};
void Acquisition::clearFlag(const uint64_t val) {
    ismrmrd_clear_flag(&(this->head.flags), val);
};
void Acquisition::clearAllFlags() {
    ismrmrd_clear_all_flags(&(this->head.flags));
};

ImageHeader::ImageHeader() {
    ismrmrd_init_image_header(this);
};

bool ImageHeader::isFlagSet(const uint64_t val) {
    return ismrmrd_is_flag_set(this->flags, val);
};

void ImageHeader::setFlag(const uint64_t val) {
    ismrmrd_set_flag(&(this->flags), val);
};

void ImageHeader::clearFlag(const uint64_t val) {
    ismrmrd_clear_flag(&(this->flags), val);
};

void ImageHeader::clearAllFlags() {
    ismrmrd_clear_all_flags(&(this->flags));
};

} // namespace ISMRMRD
