#include <string.h>
#include <stdlib.h>

#include "ismrmrd/ismrmrd.h"

namespace ISMRMRD {

//
// AcquisitionHeader class implementation
//
// Constructors
AcquisitionHeader::AcquisitionHeader() {
    ismrmrd_init_acquisition_header(this);
}

// Flag methods
bool AcquisitionHeader::isFlagSet(const ISMRMRD_AcquisitionFlags val) {
    return ismrmrd_is_flag_set(flags, val);
}

void AcquisitionHeader::setFlag(const ISMRMRD_AcquisitionFlags val) {
    ismrmrd_set_flag(&flags, val);
}

void AcquisitionHeader::clearFlag(const ISMRMRD_AcquisitionFlags val) {
    ismrmrd_clear_flag(&flags, val);
}

void AcquisitionHeader::clearAllFlags() {
    ismrmrd_clear_all_flags(&flags);
}

// TODO: Channel mask methods go here

//
// Acquisition class Implementation
//
// Constructors, assignment operator, destructor
Acquisition::Acquisition() {
    ismrmrd_init_acquisition(this);
}

Acquisition::Acquisition(const Acquisition &other) {
    // This is a deep copy
    ismrmrd_init_acquisition(this);
    ismrmrd_copy_acquisition(this, &other);
}

Acquisition & Acquisition::operator= (const Acquisition &other) {
    // Assignment makes a copy
    if (this != &other )
    {
        ismrmrd_init_acquisition(this);
        ismrmrd_copy_acquisition(this, &other);
    }
    return *this;
}

Acquisition::~Acquisition() {
    ismrmrd_cleanup_acquisition(this);
}

// Accessors and mutators
const uint16_t &Acquisition::version() {
    return head.version;
}

const uint64_t &Acquisition::flags() {
    return head.flags;
}

uint32_t &Acquisition::measurement_uid() {
    return head.measurement_uid;
}

uint32_t &Acquisition::scan_counter() {
    return head.scan_counter;
}

uint32_t &Acquisition::acquisition_time_stamp() {
    return head.acquisition_time_stamp;
}

uint32_t (&Acquisition::physiology_time_stamp()) [ISMRMRD_PHYS_STAMPS] {
    return head.physiology_time_stamp;
}

const uint16_t &Acquisition::number_of_samples() {
    return head.number_of_samples;
}

void Acquisition::number_of_samples(uint16_t num_samples) {
    head.number_of_samples = num_samples;
    ismrmrd_make_consistent_acquisition(this);
}

uint16_t &Acquisition::available_channels() {
    return head.available_channels;
}

const uint16_t &Acquisition::active_channels() {
    return head.active_channels;
}

void Acquisition::active_channels(uint16_t num_channels) {
    head.active_channels = num_channels;
    ismrmrd_make_consistent_acquisition(this);
}

const uint64_t (&Acquisition::channel_mask()) [ISMRMRD_CHANNEL_MASKS] {
    return head.channel_mask;
}

uint16_t &Acquisition::discard_pre() {
    return head.discard_pre;
}

uint16_t &Acquisition::discard_post() {
    return head.discard_post;
}

uint16_t &Acquisition::center_sample() {
    return head.center_sample;
}

uint16_t &Acquisition::encoding_space_ref() {
    return head.encoding_space_ref;
}

const uint16_t &Acquisition::trajectory_dimensions() {
    return head.trajectory_dimensions;
}

void Acquisition::trajectory_dimensions(uint16_t traj_dim) {
    head.trajectory_dimensions =  traj_dim;
    ismrmrd_make_consistent_acquisition(this);
}


float &Acquisition::sample_time_us() {
    return head.sample_time_us;
}

float (&Acquisition::position())[3] {
    return head.position;
}

float (&Acquisition::read_dir())[3] {
    return head.read_dir;
}

float (&Acquisition::phase_dir())[3] {
    return head.phase_dir;
}

float (&Acquisition::slice_dir())[3] {
    return head.slice_dir;
}

float (&Acquisition::patient_table_position())[3] {
    return head.patient_table_position;
}

ISMRMRD_EncodingCounters &Acquisition::idx() {
    return head.idx;
}

int32_t (&Acquisition::user_int()) [ISMRMRD_USER_INTS] {
    return head.user_int;
}

float (&Acquisition::user_float()) [ISMRMRD_USER_FLOATS] {
    return head.user_float;
}

// Data and Trajectory accessors
AcquisitionHeader & Acquisition::getHead() {
    // This returns a reference
    return *static_cast<AcquisitionHeader *>(&head);
}

void Acquisition::setHead(const AcquisitionHeader other) {
    memcpy(&head, &other, sizeof(AcquisitionHeader));
    ismrmrd_make_consistent_acquisition(this);
}

complex_float_t * Acquisition::getData() {
    return data;
}

float * Acquisition::getTraj() {
    return traj;
}

// Flag methods
bool Acquisition::isFlagSet(const uint64_t val) {
    return ismrmrd_is_flag_set(head.flags, val);
};
void Acquisition::setFlag(const uint64_t val) {
    ismrmrd_set_flag(&head.flags, val);
};
void Acquisition::clearFlag(const uint64_t val) {
    ismrmrd_clear_flag(&head.flags, val);
};
void Acquisition::clearAllFlags() {
    ismrmrd_clear_all_flags(&head.flags);
};

// TODO: Channel mask methods go here

//
// ImageHeader class Implementation
//

// Constructor
ImageHeader::ImageHeader() {
    ismrmrd_init_image_header(this);
};

// Flag methods
bool ImageHeader::isFlagSet(const uint64_t val) {
    return ismrmrd_is_flag_set(flags, val);
};

void ImageHeader::setFlag(const uint64_t val) {
    ismrmrd_set_flag(&flags, val);
};

void ImageHeader::clearFlag(const uint64_t val) {
    ismrmrd_clear_flag(&flags, val);
};

void ImageHeader::clearAllFlags() {
    ismrmrd_clear_all_flags(&flags);
};

//
// Image class Implementation
//

// Constructors
Image::Image() {
    ismrmrd_init_image(this);
};

Image::Image(const Image &other) {
    // This is a deep copy
    ismrmrd_init_image(this);
    ismrmrd_copy_image(this, &other);
}

Image & Image::operator= (const Image &other) {
    // Assignment makes a copy
    if (this != &other )
    {
        ismrmrd_init_image(this);
        ismrmrd_copy_image(this, &other);
    }
    return *this;
}

Image::~Image() {
    ismrmrd_cleanup_image(this);
}

// Accessors and mutators
const uint16_t &Image::version() {
    return head.version;
};

const uint16_t &Image::data_type() {
    return head.data_type;
};

void Image::data_type(uint16_t dtype) {
    // TODO function to check if type is valid
    head.data_type = dtype;
    ismrmrd_make_consistent_image(this);
};

const uint64_t &Image::flags() {
    return head.flags;
};

uint32_t &Image::measurement_uid() {
    return head.measurement_uid;
};

const uint16_t (&Image::matrix_size())[3] {
    return head.matrix_size;
};

void Image::matrix_size(const uint16_t msize[3]) {
    head.matrix_size[0] = msize[0];
    if (msize[1] > 1) {
        head.matrix_size[1] = msize[1];
    } else {
        head.matrix_size[1] = 1;
    }
    if (msize[2] > 0) {
        head.matrix_size[2] = msize[2];
    } else {
        head.matrix_size[2] = 1;
    }
    ismrmrd_make_consistent_image(this);
};

float (&Image::field_of_view())[3] {
    return head.field_of_view;
};

const uint16_t &Image::channels() {
    return head.channels;
};

void Image::channels(const uint16_t num_channels) {
    if (num_channels > 1) {
        head.channels = num_channels;
    } else {
        head.channels = 1;
    }
    ismrmrd_make_consistent_image(this);
};

float (&Image::position())[3] {
    return head.position;
};

float (&Image::read_dir())[3] {
    return head.read_dir;
};

float (&Image::phase_dir())[3] {
    return head.phase_dir;
};

float (&Image::slice_dir())[3] {
    return head.slice_dir;
};

float (&Image::patient_table_position())[3] {
    return head.patient_table_position;
};

uint16_t &Image::average() {
    return head.average;
};

uint16_t &Image::slice() {
    return head.slice;
};

uint16_t &Image::contrast() {
    return head.contrast;
};

uint16_t &Image::phase() {
    return head.repetition;
};

uint16_t &Image::repetition() {
    return head.repetition;
};

uint16_t &Image::set() {
    return head.set;
};

uint32_t &Image::acquisition_time_stamp() {
    return head.acquisition_time_stamp;
};

uint32_t (&Image::physiology_time_stamp()) [ISMRMRD_PHYS_STAMPS] { 
    return head.physiology_time_stamp;
};

uint16_t &Image::image_type() {
    return head.image_type;
};

uint16_t &Image::image_index() {
    return head.image_index;
};

uint16_t &Image::image_series_index() {
    return head.image_series_index;
};

int32_t (&Image::user_int()) [ISMRMRD_USER_INTS] { 
    return head.user_int;
};

float (&Image::user_float()) [ISMRMRD_USER_FLOATS] {
    return head.user_float;
};

const uint32_t &Image::attribute_string_len() {
    return head.attribute_string_len;
};

// Header and data accessors
ImageHeader &Image::getHead() {
    // This returns a reference
    return *static_cast<ImageHeader *>(&head);
}

void Image::setHead(const ImageHeader other) {
    memcpy(&head, &other, sizeof(ImageHeader));
    ismrmrd_make_consistent_image(this);
}

void Image::setAttributeString(std::string attr) {
    head.attribute_string_len = attr.length();
    attribute_string = (char *)realloc(attribute_string, attr.length()+1);
    strcpy(attribute_string, attr.c_str());
}

void Image::getAttributeString(std::string &attr) {
    attr = std::string(attribute_string);
}

void *Image::getData() {
    return data;
}

size_t Image::getDataSize() {
    return ismrmrd_size_of_image_data(this);
}

// Flag methods
bool Image::isFlagSet(const uint64_t val) {
    return ismrmrd_is_flag_set(head.flags, val);
};

void Image::setFlag(const uint64_t val) {
    ismrmrd_set_flag(&(head.flags), val);
};

void Image::clearFlag(const uint64_t val) {
    ismrmrd_clear_flag(&(head.flags), val);
};

void Image::clearAllFlags() {
    ismrmrd_clear_all_flags(&(head.flags));
};

//
// Array class Implementation
//
NDArray::NDArray()
{
    ismrmrd_init_ndarray(this);
}

NDArray::NDArray(const ISMRMRD_DataTypes dtype, const std::vector<uint16_t> dimvec)
{
    ismrmrd_init_ndarray(this);
    setProperties(dtype, dimvec);    
}

NDArray::NDArray(const NDArray &other)
{
    ismrmrd_init_ndarray(this);
    ismrmrd_copy_ndarray(this, &other);
}

NDArray::~NDArray()
{
    ismrmrd_cleanup_ndarray(this);
}

NDArray & NDArray::operator= (const NDArray &other)
{
    // Assignment makes a copy
    if (this != &other )
    {
        ismrmrd_init_ndarray(this);
        ismrmrd_copy_ndarray(this, &other);
    }
    return *this;
}

const uint16_t NDArray::getVersion() {
    return version;
};

const ISMRMRD_DataTypes NDArray::getDataType() {
    return static_cast<ISMRMRD_DataTypes>( data_type );
}

const uint16_t NDArray::getNDim() {
    return  ndim;
};
    
const uint16_t (&NDArray::getDims())[ISMRMRD_NDARRAY_MAXDIM] {
    return dims;
};

int NDArray::setProperties(const ISMRMRD_DataTypes dtype, const std::vector<uint16_t> dimvec) {
    if (dimvec.size() > ISMRMRD_NDARRAY_MAXDIM) {
        // TODO throw exception
        return ISMRMRD_MEMORYERROR;
    }
    data_type = dtype;
    ndim = dimvec.size();
    for (int n=0; n<ndim; n++) {
        dims[n] = dimvec[n];
    }
    int status=ismrmrd_make_consistent_ndarray(this);
    return status;
}

void * NDArray::getData() {
    return data;
}

} // namespace ISMRMRD
