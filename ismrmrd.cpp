#include "ismrmrd.h"

namespace ISMRMRD {

//
// AcquisitionHeader class implementation
//
// Constructors
AcquisitionHeader::AcquisitionHeader() {
    ismrmrd_init_acquisition_header(&head_);
};

AcquisitionHeader::AcquisitionHeader(const ISMRMRD_AcquisitionHeader *head) {
    memcpy(&head_, head, sizeof(ISMRMRD_AcquisitionHeader));
};

// Accessors and mutators
const uint16_t &AcquisitionHeader::version() {
    return head_.version;
};

const uint64_t &AcquisitionHeader::flags() {
    return head_.flags;
};

uint32_t &AcquisitionHeader::measurement_uid() {
    return head_.measurement_uid;
};

uint32_t &AcquisitionHeader::scan_counter() {
    return head_.scan_counter;
};

uint32_t &AcquisitionHeader::acquisition_time_stamp() {
    return head_.acquisition_time_stamp;
};

uint32_t (&AcquisitionHeader::physiology_time_stamp()) [ISMRMRD_PHYS_STAMPS] {
    return head_.physiology_time_stamp;
};

uint16_t &AcquisitionHeader::number_of_samples() {
    return head_.number_of_samples;
};

uint16_t &AcquisitionHeader::available_channels() {
    return head_.available_channels;
};

uint16_t &AcquisitionHeader::active_channels() {
    return head_.active_channels;
};

const uint64_t (&AcquisitionHeader::channel_mask()) [ISMRMRD_CHANNEL_MASKS] { 
    return head_.channel_mask;
};

uint16_t &AcquisitionHeader::discard_pre() {
    return head_.discard_pre;
};

uint16_t &AcquisitionHeader::discard_post() {
    return head_.discard_post;
};

uint16_t &AcquisitionHeader::center_sample() {
    return head_.center_sample;
};

uint16_t &AcquisitionHeader::encoding_space_ref() {
    return head_.encoding_space_ref;
};

uint16_t &AcquisitionHeader::trajectory_dimensions() {
    return head_.trajectory_dimensions;
};

float &AcquisitionHeader::sample_time_us() {
    return head_.sample_time_us;
};

float (&AcquisitionHeader::position())[3] {
    return head_.position;
};

float (&AcquisitionHeader::read_dir())[3] {
    return head_.read_dir;
};

float (&AcquisitionHeader::phase_dir())[3] {
    return head_.phase_dir;
};

float (&AcquisitionHeader::slice_dir())[3] {
    return head_.slice_dir;
};

float (&AcquisitionHeader::patient_table_position())[3] {
    return head_.patient_table_position;
};

ISMRMRD_EncodingCounters &AcquisitionHeader::idx() {
    return head_.idx;
};

int32_t (&AcquisitionHeader::user_int()) [ISMRMRD_USER_INTS] { 
    return head_.user_int;
};

float (&AcquisitionHeader::user_float()) [ISMRMRD_USER_FLOATS] {
 return head_.user_float;
};

// Flag methods
bool AcquisitionHeader::isFlagSet(const ISMRMRD_AcquisitionFlags val) {
    return ismrmrd_is_flag_set(head_.flags, val);
};

void AcquisitionHeader::setFlag(const ISMRMRD_AcquisitionFlags val) {
    ismrmrd_set_flag(&(head_.flags), val);
};

void AcquisitionHeader::clearFlag(const ISMRMRD_AcquisitionFlags val) {
    ismrmrd_clear_flag(&(head_.flags), val);
};

void AcquisitionHeader::clearAllFlags() {
    ismrmrd_clear_all_flags(&(head_.flags));
};

// TODO: Channel mask methods go here

//
// Acquisition class Implementation
//
// Constructors, assignment operator, destructor
Acquisition::Acquisition() {
    ismrmrd_init_acquisition(&acq_);
}

Acquisition::Acquisition(const Acquisition &other) {
    // This is a deep copy
    ismrmrd_init_acquisition(&acq_);
    ismrmrd_copy_acquisition(&acq_, &other.acq_);
}

Acquisition::Acquisition(const ISMRMRD_Acquisition *acq) {
    // This is a deep copy
    ismrmrd_init_acquisition(&acq_);
    ismrmrd_copy_acquisition(&acq_, acq);
}

Acquisition & Acquisition::operator= (const Acquisition &other) {
    // Assignment makes a copy
    if (this != &other )
    {
        ismrmrd_init_acquisition(&acq_);
        ismrmrd_copy_acquisition(&acq_, &other.acq_);
    }
    return *this;
}

Acquisition::~Acquisition() {
    ismrmrd_cleanup_acquisition(&acq_);
}

// Accessors and mutators
const uint16_t &Acquisition::version() {
    return acq_.head.version;
};

const uint64_t &Acquisition::flags() {
    return acq_.head.flags;
};

uint32_t &Acquisition::measurement_uid() {
    return acq_.head.measurement_uid;
};

uint32_t &Acquisition::scan_counter() {
    return acq_.head.scan_counter;
};

uint32_t &Acquisition::acquisition_time_stamp() {
    return acq_.head.acquisition_time_stamp;
};

uint32_t (&Acquisition::physiology_time_stamp()) [ISMRMRD_PHYS_STAMPS] {
    return acq_.head.physiology_time_stamp;
};

const uint16_t &Acquisition::number_of_samples() {
    return acq_.head.number_of_samples;
};

void Acquisition::number_of_samples(uint16_t num_samples) {
    acq_.head.number_of_samples = num_samples;
    ismrmrd_make_consistent_acquisition(&acq_);
};

uint16_t &Acquisition::available_channels() {
    return acq_.head.available_channels;
};

const uint16_t &Acquisition::active_channels() {
    return acq_.head.active_channels;
};

void Acquisition::active_channels(uint16_t num_channels) {
    acq_.head.active_channels = num_channels;
    ismrmrd_make_consistent_acquisition(&acq_);
};

const uint64_t (&Acquisition::channel_mask()) [ISMRMRD_CHANNEL_MASKS] {
    return acq_.head.channel_mask;
};

uint16_t &Acquisition::discard_pre() {
    return acq_.head.discard_pre;
};

uint16_t &Acquisition::discard_post() {
    return acq_.head.discard_post;
};

uint16_t &Acquisition::center_sample() {
    return acq_.head.center_sample;
};

uint16_t &Acquisition::encoding_space_ref() {
    return acq_.head.encoding_space_ref;
};

uint16_t &Acquisition::trajectory_dimensions() {
    return acq_.head.trajectory_dimensions;
};

float &Acquisition::sample_time_us() {
    return acq_.head.sample_time_us;
};

float (&Acquisition::position())[3] {
    return acq_.head.position;
};

float (&Acquisition::read_dir())[3] {
    return acq_.head.read_dir;
};

float (&Acquisition::phase_dir())[3] {
    return acq_.head.phase_dir;
};

float (&Acquisition::slice_dir())[3] {
    return acq_.head.slice_dir;
};

float (&Acquisition::patient_table_position())[3] {
    return acq_.head.patient_table_position;
};

ISMRMRD_EncodingCounters &Acquisition::idx() {
    return acq_.head.idx;
};

int32_t (&Acquisition::user_int()) [ISMRMRD_USER_INTS] { return acq_.head.user_int; };

float (&Acquisition::user_float()) [ISMRMRD_USER_FLOATS] { return acq_.head.user_float; };

// Data and Trajectory accessors
complex_float_t * Acquisition::data() { return acq_.data; };

float * Acquisition::traj() { return acq_.traj; };

// Flag methods
bool Acquisition::isFlagSet(const uint64_t val) {
    return ismrmrd_is_flag_set(acq_.head.flags, val);
};
void Acquisition::setFlag(const uint64_t val) {
    ismrmrd_set_flag(&acq_.head.flags, val);
};
void Acquisition::clearFlag(const uint64_t val) {
    ismrmrd_clear_flag(&acq_.head.flags, val);
};
void Acquisition::clearAllFlags() {
    ismrmrd_clear_all_flags(&acq_.head.flags);
};

// TODO: Channel mask methods go here

//
// ImageHeader class Implementation
//

// Constructors
ImageHeader::ImageHeader() {
    ismrmrd_init_image_header(&head_);
};

// Accessors and mutators
const uint16_t &ImageHeader::version() {
    return head_.version;
};
const uint16_t &ImageHeader::data_type() {
    return head_.data_type;
};
void ImageHeader::data_type(uint16_t dtype) {
    // TODO function to check if type is valid
    head_.data_type = dtype;
};

const uint64_t &ImageHeader::flags() {
    return head_.flags;
};

uint32_t &ImageHeader::measurement_uid() {
    return head_.measurement_uid;
};

const uint16_t (&ImageHeader::matrix_size())[3] {
    return head_.matrix_size;
};

void ImageHeader::matrix_size(const uint16_t msize[3]) {
    head_.matrix_size[0] = msize[0];
    if (msize[1] > 1) {
        head_.matrix_size[1] = msize[1];
    } else {
        head_.matrix_size[1] = 1;
    }
    if (msize[2] > 0) {
        head_.matrix_size[2] = msize[2];
    } else {
        head_.matrix_size[2] = 1;
    }
};

float (&ImageHeader::field_of_view())[3] {
    return head_.field_of_view;
};

const uint16_t &ImageHeader::channels() {
    return head_.channels;
};

void ImageHeader::channels(const uint16_t num_channels) {
    if (num_channels > 1) {
        head_.channels = num_channels;
    } else {
        head_.channels = 1;
    }
};

float (&ImageHeader::position())[3] {
    return head_.position;
};

float (&ImageHeader::read_dir())[3] {
    return head_.read_dir;
};

float (&ImageHeader::phase_dir())[3] {
    return head_.phase_dir;
};

float (&ImageHeader::slice_dir())[3] {
    return head_.slice_dir;
};

float (&ImageHeader::patient_table_position())[3] {
    return head_.patient_table_position;
};

uint16_t &ImageHeader::average() {
    return head_.average;
};

uint16_t &ImageHeader::slice() {
    return head_.slice;
};

uint16_t &ImageHeader::contrast() {
    return head_.contrast;
};

uint16_t &ImageHeader::phase() {
    return head_.repetition;
};

uint16_t &ImageHeader::repetition() {
    return head_.repetition;
};

uint16_t &ImageHeader::set() {
    return head_.set;
};

uint32_t &ImageHeader::acquisition_time_stamp() {
    return head_.acquisition_time_stamp;
};

uint32_t (&ImageHeader::physiology_time_stamp()) [ISMRMRD_PHYS_STAMPS] {
    return head_.physiology_time_stamp;
};

uint16_t &ImageHeader::image_type() {
    return head_.image_type;
};

uint16_t &ImageHeader::image_index() {
    return head_.image_index;
};

uint16_t &ImageHeader::image_series_index() {
    return head_.image_series_index;
};

int32_t (&ImageHeader::user_int()) [ISMRMRD_USER_INTS] { 
    return head_.user_int;
};

float (&ImageHeader::user_float()) [ISMRMRD_USER_FLOATS] { 
    return head_.user_float;
};

const uint32_t &ImageHeader::attribute_string_len() {
    return head_.attribute_string_len;
};

// Flag methods
bool ImageHeader::isFlagSet(const uint64_t val) {
    return ismrmrd_is_flag_set(head_.flags, val);
};

void ImageHeader::setFlag(const uint64_t val) {
    ismrmrd_set_flag(&(head_.flags), val);
};

void ImageHeader::clearFlag(const uint64_t val) {
    ismrmrd_clear_flag(&(head_.flags), val);
};

void ImageHeader::clearAllFlags() {
    ismrmrd_clear_all_flags(&(head_.flags));
};

//
// Image class Implementation
//

// Constructors
Image::Image() {
    ismrmrd_init_image(&image_);
};

// Accessors and mutators
const uint16_t &Image::version() {
    return image_.head.version;
};

const uint16_t &Image::data_type() {
    return image_.head.data_type;
};

void Image::data_type(uint16_t dtype) {
    // TODO function to check if type is valid
    image_.head.data_type = dtype;
    ismrmrd_make_consistent_image(&image_);
};

const uint64_t &Image::flags() {
    return image_.head.flags;
};

uint32_t &Image::measurement_uid() {
    return image_.head.measurement_uid;
};

const uint16_t (&Image::matrix_size())[3] {
    return image_.head.matrix_size;
};

void Image::matrix_size(const uint16_t msize[3]) {
    image_.head.matrix_size[0] = msize[0];
    if (msize[1] > 1) {
        image_.head.matrix_size[1] = msize[1];
    } else {
        image_.head.matrix_size[1] = 1;
    }
    if (msize[2] > 0) {
        image_.head.matrix_size[2] = msize[2];
    } else {
        image_.head.matrix_size[2] = 1;
    }
    ismrmrd_make_consistent_image(&image_);
};

float (&Image::field_of_view())[3] {
    return image_.head.field_of_view;
};

const uint16_t &Image::channels() {
    return image_.head.channels;
};

void Image::channels(const uint16_t num_channels) {
    if (num_channels > 1) {
        image_.head.channels = num_channels;
    } else {
        image_.head.channels = 1;
    }
    ismrmrd_make_consistent_image(&image_);
};

float (&Image::position())[3] {
    return image_.head.position;
};

float (&Image::read_dir())[3] {
    return image_.head.read_dir;
};

float (&Image::phase_dir())[3] {
    return image_.head.phase_dir;
};

float (&Image::slice_dir())[3] {
    return image_.head.slice_dir;
};

float (&Image::patient_table_position())[3] {
    return image_.head.patient_table_position;
};

uint16_t &Image::average() {
    return image_.head.average;
};

uint16_t &Image::slice() {
    return image_.head.slice;
};

uint16_t &Image::contrast() {
    return image_.head.contrast;
};

uint16_t &Image::phase() {
    return image_.head.repetition;
};

uint16_t &Image::repetition() {
    return image_.head.repetition;
};

uint16_t &Image::set() {
    return image_.head.set;
};

uint32_t &Image::acquisition_time_stamp() {
    return image_.head.acquisition_time_stamp;
};

uint32_t (&Image::physiology_time_stamp()) [ISMRMRD_PHYS_STAMPS] { 
    return image_.head.physiology_time_stamp;
};

uint16_t &Image::image_type() {
    return image_.head.image_type;
};

uint16_t &Image::image_index() {
    return image_.head.image_index;
};

uint16_t &Image::image_series_index() {
    return image_.head.image_series_index;
};

int32_t (&Image::user_int()) [ISMRMRD_USER_INTS] { 
    return image_.head.user_int;
};

float (&Image::user_float()) [ISMRMRD_USER_FLOATS] {
    return image_.head.user_float;
};

const uint32_t &Image::attribute_string_len() {
    return image_.head.attribute_string_len;
};

// Flag methods
bool Image::isFlagSet(const uint64_t val) {
    return ismrmrd_is_flag_set(image_.head.flags, val);
};

void Image::setFlag(const uint64_t val) {
    ismrmrd_set_flag(&(image_.head.flags), val);
};

void Image::clearFlag(const uint64_t val) {
    ismrmrd_clear_flag(&(image_.head.flags), val);
};

void Image::clearAllFlags() {
    ismrmrd_clear_all_flags(&(image_.head.flags));
};

//
// Array class Implementation
//

NDArray::NDArray()
{
    ismrmrd_init_ndarray(&arr_);
}

NDArray::NDArray(const ISMRMRD_DataTypes dtype, const uint16_t ndim, const uint16_t dims[ISMRMRD_NDARRAY_MAXDIM])
{
    ismrmrd_init_ndarray(&arr_);
    setProperties(dtype, ndim, dims);
}

NDArray::NDArray(const ISMRMRD_DataTypes dtype, const std::vector<uint16_t> dimvec)
{
    ismrmrd_init_ndarray(&arr_);
    setProperties(dtype, dimvec);    
}

NDArray::NDArray(const NDArray &other)
{
    ismrmrd_init_ndarray(&arr_);
    ismrmrd_copy_ndarray(&arr_, &other.arr_);    
}

NDArray::NDArray(const ISMRMRD_NDArray *arr)
{
    ismrmrd_init_ndarray(&arr_);
    ismrmrd_copy_ndarray(&arr_, arr);
}

NDArray::~NDArray()
{
    ismrmrd_cleanup_ndarray(&arr_);
}

NDArray & NDArray::operator= (const NDArray &other)
{
    // Assignment makes a copy
    if (this != &other )
    {
        ismrmrd_init_ndarray(&arr_);
        ismrmrd_copy_ndarray(&arr_, &other.arr_);
    }
    return *this;
}

const uint16_t NDArray::version() {
    return arr_.version;
};

const ISMRMRD_DataTypes NDArray::data_type() {
    return static_cast<ISMRMRD_DataTypes>( arr_.data_type );
}

const uint16_t NDArray::ndim() {
    return  arr_.ndim;
};
    
const uint16_t (&NDArray::dims())[ISMRMRD_NDARRAY_MAXDIM] {
    return arr_.dims;
};
    
int NDArray::setProperties(const ISMRMRD_DataTypes dtype, const uint16_t ndim, const uint16_t dims[ISMRMRD_NDARRAY_MAXDIM]) {
    arr_.data_type = dtype;
    arr_.ndim = ndim;
    for (int n=0; n<ISMRMRD_NDARRAY_MAXDIM; n++) {
        arr_.dims[n] = dims[n];
    }
    int status = ismrmrd_make_consistent_ndarray(&arr_);
    return status;
}

int NDArray::setProperties(const ISMRMRD_DataTypes dtype, const std::vector<uint16_t> dimvec) {
    if (dimvec.size() > ISMRMRD_NDARRAY_MAXDIM) {
        // TODO throw exception
        return ISMRMRD_MEMORYERROR;
    }
    arr_.data_type = dtype;
    arr_.ndim = dimvec.size();
    for (int n=0; n<arr_.ndim; n++) {
        arr_.dims[n] = dimvec[n];
    }
    int status=ismrmrd_make_consistent_ndarray(&arr_);
    return status;
}

void * NDArray::data() {
    return arr_.data;
}

} // namespace ISMRMRD
