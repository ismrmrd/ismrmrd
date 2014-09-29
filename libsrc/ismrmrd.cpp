#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <stdexcept>

#include "ismrmrd/ismrmrd.h"

namespace ISMRMRD {

std::string build_exception_string(void)
{
    char *file = NULL, *func = NULL, *msg = NULL;
    int line = 0, code = 0;
    std::stringstream stream;
    while (ismrmrd_pop_error(&file, &line, &func, &code, &msg)) {
        stream << "ISMRMRD " << ismrmrd_strerror(code) << " in " << func <<
                " (" << file << ":" << line << ": " << msg << std::endl;
    }
    return stream.str();
}


// Internal function to control the allowed data types for NDArrays
template <typename T>  ISMRMRD_DataTypes get_data_type();
template <> ISMRMRD_DataTypes get_data_type<uint16_t>()
{
    return ISMRMRD_USHORT;
}

template <> inline ISMRMRD_DataTypes get_data_type<int16_t>()
{
    return ISMRMRD_SHORT;
}

template <> inline ISMRMRD_DataTypes get_data_type<uint32_t>()
{
    return ISMRMRD_UINT;
}

template <> inline ISMRMRD_DataTypes get_data_type<int32_t>()
{
    return ISMRMRD_INT;
}

template <> inline ISMRMRD_DataTypes get_data_type<float>()
{
    return ISMRMRD_FLOAT;
}

template <> inline ISMRMRD_DataTypes get_data_type<double>()
{
    return ISMRMRD_DOUBLE;
}

template <> inline ISMRMRD_DataTypes get_data_type<complex_float_t>()
{
    return ISMRMRD_CXFLOAT;
}

template <> inline ISMRMRD_DataTypes get_data_type<complex_double_t>()
{
    return ISMRMRD_CXDOUBLE;
}

//
// AcquisitionHeader class implementation
//
// Constructors
AcquisitionHeader::AcquisitionHeader() {
    if (ismrmrd_init_acquisition_header(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
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
    if (ismrmrd_init_acquisition(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

Acquisition::Acquisition(const Acquisition &other) {
    int err = 0;
    // This is a deep copy
    err = ismrmrd_init_acquisition(this);
    if (err) {
        throw std::runtime_error(build_exception_string());
    }
    err = ismrmrd_copy_acquisition(this, &other);
    if (err) {
        throw std::runtime_error(build_exception_string());
    }
}

Acquisition & Acquisition::operator= (const Acquisition &other) {
    // Assignment makes a copy
    int err = 0;
    if (this != &other )
    {
        err = ismrmrd_init_acquisition(this);
        if (err) {
            throw std::runtime_error(build_exception_string());
        }
        err = ismrmrd_copy_acquisition(this, &other);
        if (err) {
            throw std::runtime_error(build_exception_string());
        }
    }
    return *this;
}

Acquisition::~Acquisition() {
    if (ismrmrd_cleanup_acquisition(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
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
    if (ismrmrd_make_consistent_acquisition(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

uint16_t &Acquisition::available_channels() {
    return head.available_channels;
}

const uint16_t &Acquisition::active_channels() {
    return head.active_channels;
}

void Acquisition::active_channels(uint16_t num_channels) {
    head.active_channels = num_channels;
    if (ismrmrd_make_consistent_acquisition(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
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
    if (ismrmrd_make_consistent_acquisition(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
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
    if (ismrmrd_make_consistent_acquisition(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
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
    if (ismrmrd_init_image_header(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
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
    if (ismrmrd_init_image(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
};

Image::Image(const Image &other) {
    int err = 0;
    // This is a deep copy
    err = ismrmrd_init_image(this);
    if (err) {
        throw std::runtime_error(build_exception_string());
    }
    err = ismrmrd_copy_image(this, &other);
    if (err) {
        throw std::runtime_error(build_exception_string());
    }
}

Image & Image::operator= (const Image &other) {
    int err = 0;
    // Assignment makes a copy
    if (this != &other )
    {
        err = ismrmrd_init_image(this);
        if (err) {
            throw std::runtime_error(build_exception_string());
        }
        err = ismrmrd_copy_image(this, &other);
        if (err) {
            throw std::runtime_error(build_exception_string());
        }
    }
    return *this;
}

Image::~Image() {
    if (ismrmrd_cleanup_image(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
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
    if (ismrmrd_make_consistent_image(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
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
    if (ismrmrd_make_consistent_image(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
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
    if (ismrmrd_make_consistent_image(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
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
    if (ismrmrd_make_consistent_image(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
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
    if (ismrmrd_init_ndarray(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
    data_type = get_data_type<T>();
}

template <typename T> NDArray<T>::NDArray(const std::vector<size_t> dimvec)
{
    if (ismrmrd_init_ndarray(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
    data_type = get_data_type<T>();
    resize(dimvec);
}

template <typename T> NDArray<T>::NDArray(const NDArray<T> &other)
{
    int err = 0;
    err = ismrmrd_init_ndarray(this);
    if (err) {
        throw std::runtime_error(build_exception_string());
    }
    err = ismrmrd_copy_ndarray(this, &other);
    if (err) {
        throw std::runtime_error(build_exception_string());
    }
}

template <typename T> NDArray<T>::~NDArray()
{
    if (ismrmrd_cleanup_ndarray(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

template <typename T> NDArray<T> & NDArray<T>::operator= (const NDArray<T> &other)
{
    int err = 0;
    // Assignment makes a copy
    if (this != &other )
    {
        err = ismrmrd_init_ndarray(this);
        if (err) {
            throw std::runtime_error(build_exception_string());
        }
        err = ismrmrd_copy_ndarray(this, &other);
        if (err) {
            throw std::runtime_error(build_exception_string());
        }
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
template EXPORTISMRMRD class NDArray<uint16_t>;
template EXPORTISMRMRD class NDArray<int16_t>;
template EXPORTISMRMRD class NDArray<uint32_t>;
template EXPORTISMRMRD class NDArray<int32_t>;
template EXPORTISMRMRD class NDArray<float>;
template EXPORTISMRMRD class NDArray<double>;
template EXPORTISMRMRD class NDArray<complex_float_t>;
template EXPORTISMRMRD class NDArray<complex_double_t>;

} // namespace ISMRMRD
