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
template <typename T> Image<T>::Image(uint16_t matrix_size_x,
                                      uint16_t matrix_size_y,
                                      uint16_t matrix_size_z,
                                      uint16_t channels)
{
    if (ismrmrd_init_image(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
    head.data_type = get_data_type<T>();
    resize(matrix_size_x, matrix_size_y, matrix_size_z, channels);
}

template <typename T> Image<T>::Image(const Image<T> &other) {
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

template <typename T> Image<T> & Image<T>::operator= (const Image<T> &other)
{
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

template <typename T> Image<T>::~Image() {
    if (ismrmrd_cleanup_image(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

// Image dimensions
template <typename T> void Image<T>::resize(uint16_t matrix_size_x,
                                            uint16_t matrix_size_y,
                                            uint16_t matrix_size_z,
                                            uint16_t channels)
{
    // TODO what if matrix_size_x = 0?
    if (matrix_size_y == 0) {
        matrix_size_y = 1;
    }
    if (matrix_size_z == 0) {
        matrix_size_z = 1;
    }
    if (channels == 0) {
        channels = 1;
    }
    
    head.matrix_size[0] = matrix_size_x;
    head.matrix_size[1] = matrix_size_y;
    head.matrix_size[2] = matrix_size_z;
    head.channels = channels;
    if (ismrmrd_make_consistent_image(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

template <typename T> uint16_t Image<T>::getMatrixSizeX() const
{
    return head.matrix_size[0];
}

template <typename T> void Image<T>::setMatrixSizeX(uint16_t matrix_size_x)
{
    // TODO what if matrix_size_x = 0?
    head.matrix_size[0] = matrix_size_x;
    if (ismrmrd_make_consistent_image(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

template <typename T> uint16_t Image<T>::getMatrixSizeY() const
{
    return head.matrix_size[1];
}

template <typename T> void Image<T>::setMatrixSizeY(uint16_t matrix_size_y)
{
    if (matrix_size_y == 0) {
        matrix_size_y = 1;
    }
    head.matrix_size[1] = matrix_size_y;
    if (ismrmrd_make_consistent_image(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

template <typename T> uint16_t Image<T>::getMatrixSizeZ() const
{
    return head.matrix_size[2];
}

template <typename T> void Image<T>::setMatrixSizeZ(uint16_t matrix_size_z)
{
    if (matrix_size_z == 0) {
        matrix_size_z = 1;
    }
    head.matrix_size[2] = matrix_size_z;
    if (ismrmrd_make_consistent_image(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

template <typename T> uint16_t Image<T>::getNumberOfChannels() const
{
    return head.channels;
}

template <typename T> void Image<T>::setNumberOfChannels(uint16_t channels)
{
    if (channels == 0) {
        channels = 1;
    }

    head.channels = channels;
    if (ismrmrd_make_consistent_image(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}


template <typename T> void Image<T>::setFieldOfView(float fov_x, float fov_y, float fov_z)
{
    head.field_of_view[0] = fov_x;
    head.field_of_view[1] = fov_y;
    head.field_of_view[2] = fov_z;
}

template <typename T> void Image<T>::setFieldOfViewX(float fov_x)
{
    head.field_of_view[0] = fov_x;
}

template <typename T> float Image<T>::getFieldOfViewX() const
{
    return head.field_of_view[0];
}

template <typename T> void Image<T>::setFieldOfViewY(float fov_y)
{
    head.field_of_view[1] = fov_y;
}

template <typename T> float Image<T>::getFieldOfViewY() const
{
    return head.field_of_view[1];
}

template <typename T> void Image<T>::setFieldOfViewZ(float fov_z)
{
    head.field_of_view[2] = fov_z;
}

template <typename T> float Image<T>::getFieldOfViewZ() const
{
    return head.field_of_view[2];
}


// Positions and orientations
template <typename T> void Image<T>::setPosition(float x, float y, float z)
{
    head.position[0] = x;
    head.position[1] = y;
    head.position[2] = z;
}

template <typename T> float Image<T>::getPositionX() const
{
    return head.position[0];
}

template <typename T> void Image<T>::setPositionX(float x)
{
    head.position[0] = x;
}

template <typename T> float Image<T>::getPositionY() const
{
    return head.position[1];
}

template <typename T> void Image<T>::setPositionY(float y)
{
    head.position[1] = y;
}

template <typename T> float Image<T>::getPositionZ() const
{
    return head.position[2];
}

template <typename T> void Image<T>::setPositionZ(float z)
{
    head.position[2] = z;
}


template <typename T> void Image<T>::setReadDirection(float x, float y, float z)
{
    head.read_dir[0] = x;
    head.read_dir[1] = y;
    head.read_dir[2] = z;
}

template <typename T> float Image<T>::getReadDirectionX() const
{
    return head.read_dir[0];
}

template <typename T> void Image<T>::setReadDirectionX(float x)
{
    head.read_dir[0] = x;
}

template <typename T> float Image<T>::getReadDirectionY() const
{
    return head.read_dir[1];
}

template <typename T> void Image<T>::setReadDirectionY(float y)
{
    head.read_dir[1] = y;
}

template <typename T> float Image<T>::getReadDirectionZ() const
{
    return head.read_dir[2];
}

template <typename T> void Image<T>::setReadDirectionZ(float z)
{
    head.read_dir[2] = z;    
}

    
template <typename T> void Image<T>::setPhaseDirection(float x, float y, float z)
{
    head.phase_dir[0] = x;
    head.phase_dir[1] = y;
    head.phase_dir[2] = z;
}

template <typename T> float Image<T>::getPhaseDirectionX() const
{
    return head.phase_dir[0];
}

template <typename T> void Image<T>::setPhaseDirectionX(float x)
{
    head.phase_dir[0] = x;
}

template <typename T> float Image<T>::getPhaseDirectionY() const
{
    return head.phase_dir[1];
}

template <typename T> void Image<T>::setPhaseDirectionY(float y)
{
    head.phase_dir[1] = y;
}

template <typename T> float Image<T>::getPhaseDirectionZ() const
{
    return head.phase_dir[2];
}

template <typename T> void Image<T>::setPhaseDirectionZ(float z)
{
    head.phase_dir[2] = z;
}

template <typename T> void Image<T>::setSliceDirection(float x, float y, float z)
{
    head.slice_dir[0] = x;
    head.slice_dir[1] = y;
    head.slice_dir[2] = z;
}

template <typename T> float Image<T>::getSliceDirectionX() const
{
    return head.slice_dir[0];
}

template <typename T> void Image<T>::setSliceDirectionX(float x)
{
    head.slice_dir[0] = x;
}

template <typename T> float Image<T>::getSliceDirectionY() const
{
    return head.slice_dir[1];
}

template <typename T> void Image<T>::setSliceDirectionY(float y)
{
    head.slice_dir[1] = y;
}

template <typename T> float Image<T>::getSliceDirectionZ() const
{
    return head.slice_dir[2];
}

template <typename T> void Image<T>::setSliceDirectionZ(float z)
{
    head.slice_dir[2] = z;
}
    
template <typename T> void Image<T>::setPatientTablePosition(float x, float y, float z)
{
    head.patient_table_position[0] = x;
    head.patient_table_position[1] = y;
    head.patient_table_position[2] = z;
}

template <typename T> float Image<T>::getPatientTablePositionX() const
{
    return head.patient_table_position[0];
}

template <typename T> void Image<T>::setPatientTablePositionX(float x)
{
    head.patient_table_position[0] = x;
}

template <typename T> float Image<T>::getPatientTablePositionY() const
{
    return head.patient_table_position[1];
}

template <typename T> void Image<T>::setPatientTablePositionY(float y)
{
    head.patient_table_position[1] = y;
}

template <typename T> float Image<T>::getPatientTablePositionZ() const
{
    return head.patient_table_position[2];
}

template <typename T> void Image<T>::setPatientTablePositionZ(float z)
{
    head.patient_table_position[2] = z;
}

template <typename T> uint16_t Image<T>::getVersion() const
{
    return head.version;
}

template <typename T> ISMRMRD_DataTypes Image<T>::getDataType() const
{
    return static_cast<ISMRMRD_DataTypes>(head.data_type);
}

template <typename T> uint32_t Image<T>::getMeasurementUid() const
{
    return head.measurement_uid;
}

template <typename T> void Image<T>::setMeasurementUid(uint32_t measurement_uid)
{
    head.measurement_uid = measurement_uid;
}


template <typename T> uint16_t Image<T>::getAverage() const
{
    return head.average;
}

template <typename T> void  Image<T>::setAverage(uint16_t average)
{
    head.average = average;
}

template <typename T> uint16_t Image<T>::getSlice() const
{
    return head.slice;
}

template <typename T> void  Image<T>::setSlice(uint16_t slice)
{
    head.slice = slice;
}

template <typename T> uint16_t Image<T>::getContrast() const
{
    return head.contrast;
}

template <typename T> void  Image<T>::setContrast(uint16_t contrast)
{
    head.contrast = contrast;
}

template <typename T> uint16_t Image<T>::getPhase() const
{
    return head.phase;
}

template <typename T> void  Image<T>::setPhase(uint16_t phase)
{
    head.phase = phase;
}
    
template <typename T> uint16_t Image<T>::getRepetition() const
{
    return head.repetition;
}

template <typename T> void  Image<T>::setRepetition(uint16_t repetition)
{
    head.repetition = repetition;
}

template <typename T> uint16_t Image<T>::getSet() const
{
    return head.set;
}

template <typename T> void  Image<T>::setSet(uint16_t set)
{
    head.set = set;
}

template <typename T> uint32_t Image<T>::getAcquisitionTimeStamp() const
{
    return head.acquisition_time_stamp;
}

template <typename T> void  Image<T>::setAcquisitionTimeStamp(uint32_t acquisition_time_stamp)
{
    head.acquisition_time_stamp = acquisition_time_stamp;
}

template <typename T> uint32_t Image<T>::getPhysiologyTimeStamp(unsigned int stamp_id) const
{
    return head.physiology_time_stamp[stamp_id];
}

template <typename T> void  Image<T>::setPhysiologyTimeStamp(unsigned int stamp_id, uint32_t value)
{
    head.physiology_time_stamp[stamp_id] = value;
}

template <typename T> uint16_t Image<T>::getImageType() const
{
    return head.image_type;
}

template <typename T> void Image<T>::setImageType(uint16_t image_type)
{
    head.image_type = image_type;
}

template <typename T> uint16_t Image<T>::getImageIndex() const
{
    return head.image_index;
}

template <typename T> void Image<T>::setImageIndex(uint16_t image_index)
{
    head.image_index = image_index;
}

template <typename T> uint16_t Image<T>::getImageSeriesIndex() const
{
    return head.image_series_index;
}

template <typename T> void Image<T>::setImageSeriesIndex(uint16_t image_series_index)
{
    head.image_series_index = image_series_index;
}

    
// User parameters
template <typename T> float Image<T>::getUserFloat(unsigned int index) const
{
    return head.user_float[index];
}

template <typename T> void Image<T>::setUserFloat(unsigned int index, float value)
{
    head.user_float[index] = value;
}

template <typename T> int32_t Image<T>::getUserInt(unsigned int index) const
{
    return head.user_int[index];
}

template <typename T> void Image<T>::setUserInt(unsigned int index, int32_t value)
{
    head.user_int[index] = value;
}


// Flag methods
template <typename T> uint64_t Image<T>::getFlags() const {
    return head.flags;
}

template <typename T> bool Image<T>::isFlagSet(const uint64_t val) const {
    return ismrmrd_is_flag_set(head.flags, val);
}

template <typename T> void Image<T>::setFlag(const uint64_t val) {
    ismrmrd_set_flag(&(head.flags), val);
}

template <typename T> void Image<T>::clearFlag(const uint64_t val) {
    ismrmrd_clear_flag(&(head.flags), val);
}

template <typename T> void Image<T>::clearAllFlags() {
    ismrmrd_clear_all_flags(&(head.flags));
}

// Header
template <typename T> ImageHeader &Image<T>::getHead() {
    // This returns a reference
    return *static_cast<ImageHeader *>(&head);
}

template <typename T> void Image<T>::setHead(const ImageHeader &other) {
    if (other.data_type != head.data_type) {
        throw std::runtime_error("Cannot assign a header of a different data type.");
    }
    
    memcpy(&head, &other, sizeof(ImageHeader));
    if (ismrmrd_make_consistent_image(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

// Attribute string
template <typename T> void Image<T>::getAttributeString(std::string &attr) const
{
    attr = std::string(attribute_string);
}

template <typename T> void Image<T>::setAttributeString(const std::string attr)
{
    head.attribute_string_len = attr.length();
    attribute_string = (char *)realloc(attribute_string, attr.length()+1);
    // TODO error check?
    strcpy(attribute_string, attr.c_str());
}

template <typename T> const size_t Image<T>::getAttributeStringLength()
{
    return head.attribute_string_len;
}

// Data
template <typename T> T * const Image<T>::getData() const {
     return static_cast<T*>(data);
}

template <typename T> size_t Image<T>::getNumberOfDataElements() const {
    size_t num = 1;
    num *= head.matrix_size[0];
    num *= head.matrix_size[1];
    num *= head.matrix_size[2];
    num *= head.channels;
    return ismrmrd_size_of_image_data(this);
}

template <typename T> size_t Image<T>::getDataSize() const {
    return ismrmrd_size_of_image_data(this);
}


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

template <typename T> void NDArray<T>::resize(const std::vector<size_t> dimvec) {
    if (dimvec.size() > ISMRMRD_NDARRAY_MAXDIM) {
        throw std::runtime_error("Input vector dimvec is too long.");
    }
    ndim = dimvec.size();
    for (int n=0; n<ndim; n++) {
        dims[n] = dimvec[n];
    }
    if (ismrmrd_make_consistent_ndarray(this) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
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
template <> EXPORTISMRMRD ISMRMRD_DataTypes get_data_type<uint16_t>()
{
    return ISMRMRD_USHORT;
}

template <> EXPORTISMRMRD inline ISMRMRD_DataTypes get_data_type<int16_t>()
{
    return ISMRMRD_SHORT;
}

template <> EXPORTISMRMRD inline ISMRMRD_DataTypes get_data_type<uint32_t>()
{
    return ISMRMRD_UINT;
}

template <> EXPORTISMRMRD inline ISMRMRD_DataTypes get_data_type<int32_t>()
{
    return ISMRMRD_INT;
}

template <> EXPORTISMRMRD inline ISMRMRD_DataTypes get_data_type<float>()
{
    return ISMRMRD_FLOAT;
}

template <> EXPORTISMRMRD inline ISMRMRD_DataTypes get_data_type<double>()
{
    return ISMRMRD_DOUBLE;
}

template <> EXPORTISMRMRD inline ISMRMRD_DataTypes get_data_type<complex_float_t>()
{
    return ISMRMRD_CXFLOAT;
}

template <> EXPORTISMRMRD inline ISMRMRD_DataTypes get_data_type<complex_double_t>()
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
