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

// Sizes
const size_t Acquisition::getNumberOfDataElements() {
    size_t num = head.number_of_samples * head.active_channels;
    return num;
}

const size_t Acquisition::getNumberOfTrajElements() {
    size_t num = head.number_of_samples * head.trajectory_dimensions;
    return num;
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
template <typename T> Image<T>::Image()
{
    ismrmrd_init_image(this);
    data_type = get_data_type<T>();
};

template <typename T> Image<T>::Image(const Image<T> &other) {
    // This is a deep copy
    ismrmrd_init_image(this);
    ismrmrd_copy_image(this, &other);
}

template <typename T> Image<T> & Image<T>::operator= (const Image<T> &other)
{
    // Assignment makes a copy
    if (this != &other )
    {
        ismrmrd_init_image(this);
        ismrmrd_copy_image(this, &other);
    }
    return *this;
}

template <typename T> Image<T>::~Image() {
    ismrmrd_cleanup_image(this);
}

// Accessors and mutators
template <typename T> const uint16_t & Image<T>::version() {
    return head.version;
};

template <typename T> const uint16_t &Image<T>::data_type() {
    return head.data_type;
};

template <typename T> void Image<T>::data_type(uint16_t dtype) {
    // TODO function to check if type is valid
    head.data_type = dtype;
    ismrmrd_make_consistent_image(this);
};

template <typename T> const uint64_t &Image<T>::flags() {
    return head.flags;
};

template <typename T> uint32_t &Image<T>::measurement_uid() {
    return head.measurement_uid;
};

template <typename T> const uint16_t (&Image<T>::matrix_size())[3] {
    return head.matrix_size;
};

template <typename T> void Image<T>::matrix_size(const uint16_t msize[3]) {
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

template <typename T> float (&Image<T>::field_of_view())[3] {
    return head.field_of_view;
};

template <typename T> const uint16_t &Image<T>::channels() {
    return head.channels;
};

template <typename T> void Image<T>::channels(const uint16_t num_channels) {
    if (num_channels > 1) {
        head.channels = num_channels;
    } else {
        head.channels = 1;
    }
    ismrmrd_make_consistent_image(this);
};

template <typename T> float (&Image<T>::position())[3] {
    return head.position;
};

template <typename T> float (&Image<T>::read_dir())[3] {
    return head.read_dir;
};

template <typename T> float (&Image<T>::phase_dir())[3] {
    return head.phase_dir;
};

template <typename T> float (&Image<T>::slice_dir())[3] {
    return head.slice_dir;
};

template <typename T> float (&Image<T>::patient_table_position())[3] {
    return head.patient_table_position;
};

template <typename T> uint16_t &Image<T>::average() {
    return head.average;
};

template <typename T> uint16_t &Image<T>::slice() {
    return head.slice;
};

template <typename T> uint16_t &Image<T>::contrast() {
    return head.contrast;
};

######YOU ARE HERE ########
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
template <typename T> NDArray<T>::NDArray()
{
    ismrmrd_init_ndarray(this);
    data_type = get_data_type<T>();
}

template <typename T> NDArray<T>::NDArray(const std::vector<size_t> dimvec)
{
    ismrmrd_init_ndarray(this);
    data_type = get_data_type<T>();
    resize(dimvec);
}

template <typename T> NDArray<T>::NDArray(const NDArray<T> &other)
{
    ismrmrd_init_ndarray(this);
    ismrmrd_copy_ndarray(this, &other);
}

template <typename T> NDArray<T>::~NDArray()
{
    ismrmrd_cleanup_ndarray(this);
}

template <typename T> NDArray<T> & NDArray<T>::operator= (const NDArray<T> &other)
{
    // Assignment makes a copy
    if (this != &other )
    {
        ismrmrd_init_ndarray(this);
        ismrmrd_copy_ndarray(this, &other);
    }
    return *this;
}

template <typename T> const uint16_t NDArray<T>::getVersion() {
    return version;
};

template <typename T> const ISMRMRD_DataTypes NDArray<T>::getDataType() {
    return static_cast<ISMRMRD_DataTypes>( data_type );
}

template <typename T> const uint16_t NDArray<T>::getNDim() {
    return  ndim;
};
    
template <typename T> const size_t (&NDArray<T>::getDims())[ISMRMRD_NDARRAY_MAXDIM] {
    return dims;
};

template <typename T> int NDArray<T>::resize(const std::vector<size_t> dimvec) {
    if (dimvec.size() > ISMRMRD_NDARRAY_MAXDIM) {
        // TODO throw exception
        return ISMRMRD_MEMORYERROR;
    }
    ndim = dimvec.size();
    for (int n=0; n<ndim; n++) {
        dims[n] = dimvec[n];
    }
    int status=ismrmrd_make_consistent_ndarray(this);
    return status;
}

template <typename T> T * NDArray<T>::getData() {
    return static_cast<T*>(data);
}

template <typename T> const size_t NDArray<T>::getDataSize() {
    return ismrmrd_size_of_ndarray_data(this);
}

template <typename T> const size_t NDArray<T>::getNumberOfElements() {
    size_t num = 1;
    for (int n = 0; n < ndim; n++) {
        num *= dims[n];
    }
    return num;
}


// Specializations
// Allowed data types for Images and NDArrays
EXPORTISMRMRD template <> ISMRMRD_DataTypes get_data_type<uint16_t>()
{
    return ISMRMRD_USHORT;
}

EXPORTISMRMRD template <> inline ISMRMRD_DataTypes get_data_type<int16_t>()
{
    return ISMRMRD_SHORT;
}

EXPORTISMRMRD template <> inline ISMRMRD_DataTypes get_data_type<uint32_t>()
{
    return ISMRMRD_UINT;
}

EXPORTISMRMRD template <> inline ISMRMRD_DataTypes get_data_type<int32_t>()
{
    return ISMRMRD_INT;
}

EXPORTISMRMRD template <> inline ISMRMRD_DataTypes get_data_type<float>()
{
    return ISMRMRD_FLOAT;
}

EXPORTISMRMRD template <> inline ISMRMRD_DataTypes get_data_type<double>()
{
    return ISMRMRD_DOUBLE;
}

EXPORTISMRMRD template <> inline ISMRMRD_DataTypes get_data_type<complex_float_t>()
{
    return ISMRMRD_CXFLOAT;
}

EXPORTISMRMRD template <> inline ISMRMRD_DataTypes get_data_type<complex_double_t>()
{
    return ISMRMRD_CXDOUBLE;
}

// Images
template EXPORTISMRMRD class Image<uint16_t>;
template EXPORTISMRMRD class Image<int16_t>;
template EXPORTISMRMRD class Image<uint32_t>;
template EXPORTISMRMRD class Image<int32_t>;
template EXPORTISMRMRD class Image<float>;
template EXPORTISMRMRD class Image<double>;
template EXPORTISMRMRD class Image<complex_float_t>;
template EXPORTISMRMRD class Image<complex_double_t>;

// NDArrays
template EXPORTISMRMRD class NDArray<uint16_t>;
template EXPORTISMRMRD class NDArray<int16_t>;
template EXPORTISMRMRD class NDArray<uint32_t>;
template EXPORTISMRMRD class NDArray<int32_t>;
template EXPORTISMRMRD class NDArray<float>;
template EXPORTISMRMRD class NDArray<double>;
template EXPORTISMRMRD class NDArray<complex_float_t>;
template EXPORTISMRMRD class NDArray<complex_double_t>;

} // namespace ISMRMRD
