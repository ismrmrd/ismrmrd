#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <stdexcept>

#include <iostream>
#include "ismrmrd/ismrmrd.h"

namespace ISMRMRD {

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

// Channel mask methods
bool AcquisitionHeader::isChannelActive(uint16_t channel_id) {
    return ismrmrd_is_channel_on(channel_mask, channel_id);
}
void AcquisitionHeader::setChannelActive(uint16_t channel_id) {
    ismrmrd_set_channel_on(channel_mask, channel_id);
}
void AcquisitionHeader::setChannelNotActive(uint16_t channel_id) {
    ismrmrd_set_channel_off(channel_mask, channel_id);
}
void AcquisitionHeader::setAllChannelsNotActive() {
    ismrmrd_set_all_channels_off(channel_mask);
}


//
// Acquisition class Implementation
//
// Constructors, assignment operator, destructor
Acquisition::Acquisition() {
    if (ismrmrd_init_acquisition(&acq) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}


Acquisition::Acquisition(uint16_t num_samples, uint16_t active_channels, uint16_t trajectory_dimensions){
    if (ismrmrd_init_acquisition(&acq) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
    this->resize(num_samples,active_channels,trajectory_dimensions);
}

Acquisition::Acquisition(const Acquisition &other) {
    int err = 0;
    // This is a deep copy
    err = ismrmrd_init_acquisition(&acq);
    if (err) {
        throw std::runtime_error(build_exception_string());
    }
    err = ismrmrd_copy_acquisition(&acq, &other.acq);
    if (err) {
        throw std::runtime_error(build_exception_string());
    }
}

Acquisition & Acquisition::operator= (const Acquisition &other) {
    // Assignment makes a copy
    int err = 0;
    if (this != &other )
    {
        err = ismrmrd_init_acquisition(&acq);
        if (err) {
            throw std::runtime_error(build_exception_string());
        }
        err = ismrmrd_copy_acquisition(&acq, &other.acq);
        if (err) {
            throw std::runtime_error(build_exception_string());
        }
    }
    return *this;
}

Acquisition::~Acquisition() {
    if (ismrmrd_cleanup_acquisition(&acq) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

// Accessors and mutators
const uint16_t &Acquisition::version() {
    return acq.head.version;
}

const uint64_t &Acquisition::flags() {
    return acq.head.flags;
}

uint32_t &Acquisition::measurement_uid() {
    return acq.head.measurement_uid;
}

uint32_t &Acquisition::scan_counter() {
    return acq.head.scan_counter;
}

uint32_t &Acquisition::acquisition_time_stamp() {
    return acq.head.acquisition_time_stamp;
}

uint32_t (&Acquisition::physiology_time_stamp()) [ISMRMRD_PHYS_STAMPS] {
    return acq.head.physiology_time_stamp;
}

const uint16_t &Acquisition::number_of_samples() {
    return acq.head.number_of_samples;
}

uint16_t &Acquisition::available_channels() {
    return acq.head.available_channels;
}

const uint16_t &Acquisition::active_channels() {
    return acq.head.active_channels;
}

const uint64_t (&Acquisition::channel_mask()) [ISMRMRD_CHANNEL_MASKS] {
    return acq.head.channel_mask;
}

uint16_t &Acquisition::discard_pre() {
    return acq.head.discard_pre;
}

uint16_t &Acquisition::discard_post() {
    return acq.head.discard_post;
}

uint16_t &Acquisition::center_sample() {
    return acq.head.center_sample;
}

uint16_t &Acquisition::encoding_space_ref() {
    return acq.head.encoding_space_ref;
}

const uint16_t &Acquisition::trajectory_dimensions() {
    return acq.head.trajectory_dimensions;
}

float &Acquisition::sample_time_us() {
    return acq.head.sample_time_us;
}

float (&Acquisition::position())[3] {
    return acq.head.position;
}

float (&Acquisition::read_dir())[3] {
    return acq.head.read_dir;
}

float (&Acquisition::phase_dir())[3] {
    return acq.head.phase_dir;
}

float (&Acquisition::slice_dir())[3] {
    return acq.head.slice_dir;
}

float (&Acquisition::patient_table_position())[3] {
    return acq.head.patient_table_position;
}

ISMRMRD_EncodingCounters &Acquisition::idx() {
    return acq.head.idx;
}

int32_t (&Acquisition::user_int()) [ISMRMRD_USER_INTS] {
    return acq.head.user_int;
}

float (&Acquisition::user_float()) [ISMRMRD_USER_FLOATS] {
    return acq.head.user_float;
}

// Sizes
size_t Acquisition::getNumberOfDataElements() const {
    size_t num = acq.head.number_of_samples * acq.head.active_channels;
    return num;
}

size_t Acquisition::getDataSize() const {
    size_t num = acq.head.number_of_samples * acq.head.active_channels;
    return num*sizeof(complex_float_t);
}

size_t Acquisition::getNumberOfTrajElements() const {
    size_t num = acq.head.number_of_samples * acq.head.trajectory_dimensions;
    return num;
}

size_t Acquisition::getTrajSize() const {
    size_t num = acq.head.number_of_samples * acq.head.trajectory_dimensions;
    return num*sizeof(float);
}

// Data and Trajectory accessors
const AcquisitionHeader & Acquisition::getHead() const {
    // This returns a reference
    return *static_cast<const AcquisitionHeader *>(&acq.head);
}

void Acquisition::setHead(const AcquisitionHeader &other) {
    memcpy(&acq.head, &other, sizeof(AcquisitionHeader));
    if (ismrmrd_make_consistent_acquisition(&acq) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

void Acquisition::resize(uint16_t num_samples, uint16_t active_channels, uint16_t trajectory_dimensions){
       acq.head.number_of_samples = num_samples;
       acq.head.active_channels = active_channels;
       acq.head.trajectory_dimensions = trajectory_dimensions;
       if (ismrmrd_make_consistent_acquisition(&acq) != ISMRMRD_NOERROR) {
           throw std::runtime_error(build_exception_string());
       }
}

const complex_float_t * Acquisition::getDataPtr() const {
    return acq.data;
}

complex_float_t * Acquisition::getDataPtr() {
    return acq.data;
}

void Acquisition::setData(complex_float_t * data) {
    memcpy(acq.data,data,this->getNumberOfDataElements()*sizeof(complex_float_t));
}

complex_float_t & Acquisition::data(uint16_t sample, uint16_t channel){
       size_t index = size_t(sample)+size_t(channel)*size_t(acq.head.number_of_samples);
       return acq.data[index];
}

const float * Acquisition::getTrajPtr() const {
    return acq.traj;
}

float * Acquisition::getTrajPtr() {
    return acq.traj;
}

void Acquisition::setTraj(float* traj) {
       memcpy(acq.traj,traj,this->getNumberOfTrajElements()*sizeof(float));
}

float & Acquisition::traj(uint16_t dimension, uint16_t sample){
    size_t index = size_t(sample)*size_t(acq.head.trajectory_dimensions)+size_t(dimension);
    return acq.traj[index];
}

complex_float_t * Acquisition::data_begin() const{
       return acq.data;
}

complex_float_t * Acquisition::data_end() const {
       return acq.data+size_t(acq.head.number_of_samples)*size_t(acq.head.active_channels);
}

float * Acquisition::traj_begin() const {
       return acq.traj;
}

float * Acquisition::traj_end() const {
       return acq.traj+size_t(acq.head.number_of_samples)*size_t(acq.head.trajectory_dimensions);
}

// Flag methods
bool Acquisition::isFlagSet(const uint64_t val) {
    return ismrmrd_is_flag_set(acq.head.flags, val);
}
void Acquisition::setFlag(const uint64_t val) {
    ismrmrd_set_flag(&acq.head.flags, val);
}
void Acquisition::clearFlag(const uint64_t val) {
    ismrmrd_clear_flag(&acq.head.flags, val);
}
void Acquisition::clearAllFlags() {
    ismrmrd_clear_all_flags(&acq.head.flags);
}

// Channel mask methods
bool Acquisition::isChannelActive(uint16_t channel_id) {
    return ismrmrd_is_channel_on(acq.head.channel_mask, channel_id);
}
void Acquisition::setChannelActive(uint16_t channel_id) {
    ismrmrd_set_channel_on(acq.head.channel_mask, channel_id);
}
void Acquisition::setChannelNotActive(uint16_t channel_id) {
    ismrmrd_set_channel_off(acq.head.channel_mask, channel_id);
}
void Acquisition::setAllChannelsNotActive() {
    ismrmrd_set_all_channels_off(acq.head.channel_mask);
}


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
    if (ismrmrd_init_image(&im) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
    im.head.data_type = static_cast<uint16_t>(get_data_type<T>());
    resize(matrix_size_x, matrix_size_y, matrix_size_z, channels);
}

template <typename T> Image<T>::Image(const Image<T> &other) {
    int err = 0;
    // This is a deep copy
    err = ismrmrd_init_image(&im);
    if (err) {
        throw std::runtime_error(build_exception_string());
    }
    err = ismrmrd_copy_image(&im, &other.im);
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
        err = ismrmrd_init_image(&im);
        if (err) {
            throw std::runtime_error(build_exception_string());
        }
        err = ismrmrd_copy_image(&im, &other.im);
        if (err) {
            throw std::runtime_error(build_exception_string());
        }
    }
    return *this;
}

template <typename T> Image<T>::~Image() {
    if (ismrmrd_cleanup_image(&im) != ISMRMRD_NOERROR) {
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
    
    im.head.matrix_size[0] = matrix_size_x;
    im.head.matrix_size[1] = matrix_size_y;
    im.head.matrix_size[2] = matrix_size_z;
    im.head.channels = channels;
    if (ismrmrd_make_consistent_image(&im) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

template <typename T> uint16_t Image<T>::getMatrixSizeX() const
{
    return im.head.matrix_size[0];
}

template <typename T> void Image<T>::setMatrixSizeX(uint16_t matrix_size_x)
{
    // TODO what if matrix_size_x = 0?
    im.head.matrix_size[0] = matrix_size_x;
    if (ismrmrd_make_consistent_image(&im) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

template <typename T> uint16_t Image<T>::getMatrixSizeY() const
{
    return im.head.matrix_size[1];
}

template <typename T> void Image<T>::setMatrixSizeY(uint16_t matrix_size_y)
{
    if (matrix_size_y == 0) {
        matrix_size_y = 1;
    }
    im.head.matrix_size[1] = matrix_size_y;
    if (ismrmrd_make_consistent_image(&im) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

template <typename T> uint16_t Image<T>::getMatrixSizeZ() const
{
    return im.head.matrix_size[2];
}

template <typename T> void Image<T>::setMatrixSizeZ(uint16_t matrix_size_z)
{
    if (matrix_size_z == 0) {
        matrix_size_z = 1;
    }
    im.head.matrix_size[2] = matrix_size_z;
    if (ismrmrd_make_consistent_image(&im) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

template <typename T> uint16_t Image<T>::getNumberOfChannels() const
{
    return im.head.channels;
}

template <typename T> void Image<T>::setNumberOfChannels(uint16_t channels)
{
    if (channels == 0) {
        channels = 1;
    }

    im.head.channels = channels;
    if (ismrmrd_make_consistent_image(&im) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}


template <typename T> void Image<T>::setFieldOfView(float fov_x, float fov_y, float fov_z)
{
    im.head.field_of_view[0] = fov_x;
    im.head.field_of_view[1] = fov_y;
    im.head.field_of_view[2] = fov_z;
}

template <typename T> void Image<T>::setFieldOfViewX(float fov_x)
{
    im.head.field_of_view[0] = fov_x;
}

template <typename T> float Image<T>::getFieldOfViewX() const
{
    return im.head.field_of_view[0];
}

template <typename T> void Image<T>::setFieldOfViewY(float fov_y)
{
    im.head.field_of_view[1] = fov_y;
}

template <typename T> float Image<T>::getFieldOfViewY() const
{
    return im.head.field_of_view[1];
}

template <typename T> void Image<T>::setFieldOfViewZ(float fov_z)
{
    im.head.field_of_view[2] = fov_z;
}

template <typename T> float Image<T>::getFieldOfViewZ() const
{
    return im.head.field_of_view[2];
}


// Positions and orientations
template <typename T> void Image<T>::setPosition(float x, float y, float z)
{
    im.head.position[0] = x;
    im.head.position[1] = y;
    im.head.position[2] = z;
}

template <typename T> float Image<T>::getPositionX() const
{
    return im.head.position[0];
}

template <typename T> void Image<T>::setPositionX(float x)
{
    im.head.position[0] = x;
}

template <typename T> float Image<T>::getPositionY() const
{
    return im.head.position[1];
}

template <typename T> void Image<T>::setPositionY(float y)
{
    im.head.position[1] = y;
}

template <typename T> float Image<T>::getPositionZ() const
{
    return im.head.position[2];
}

template <typename T> void Image<T>::setPositionZ(float z)
{
    im.head.position[2] = z;
}


template <typename T> void Image<T>::setReadDirection(float x, float y, float z)
{
    im.head.read_dir[0] = x;
    im.head.read_dir[1] = y;
    im.head.read_dir[2] = z;
}

template <typename T> float Image<T>::getReadDirectionX() const
{
    return im.head.read_dir[0];
}

template <typename T> void Image<T>::setReadDirectionX(float x)
{
    im.head.read_dir[0] = x;
}

template <typename T> float Image<T>::getReadDirectionY() const
{
    return im.head.read_dir[1];
}

template <typename T> void Image<T>::setReadDirectionY(float y)
{
    im.head.read_dir[1] = y;
}

template <typename T> float Image<T>::getReadDirectionZ() const
{
    return im.head.read_dir[2];
}

template <typename T> void Image<T>::setReadDirectionZ(float z)
{
    im.head.read_dir[2] = z;    
}

    
template <typename T> void Image<T>::setPhaseDirection(float x, float y, float z)
{
    im.head.phase_dir[0] = x;
    im.head.phase_dir[1] = y;
    im.head.phase_dir[2] = z;
}

template <typename T> float Image<T>::getPhaseDirectionX() const
{
    return im.head.phase_dir[0];
}

template <typename T> void Image<T>::setPhaseDirectionX(float x)
{
    im.head.phase_dir[0] = x;
}

template <typename T> float Image<T>::getPhaseDirectionY() const
{
    return im.head.phase_dir[1];
}

template <typename T> void Image<T>::setPhaseDirectionY(float y)
{
    im.head.phase_dir[1] = y;
}

template <typename T> float Image<T>::getPhaseDirectionZ() const
{
    return im.head.phase_dir[2];
}

template <typename T> void Image<T>::setPhaseDirectionZ(float z)
{
    im.head.phase_dir[2] = z;
}

template <typename T> void Image<T>::setSliceDirection(float x, float y, float z)
{
    im.head.slice_dir[0] = x;
    im.head.slice_dir[1] = y;
    im.head.slice_dir[2] = z;
}

template <typename T> float Image<T>::getSliceDirectionX() const
{
    return im.head.slice_dir[0];
}

template <typename T> void Image<T>::setSliceDirectionX(float x)
{
    im.head.slice_dir[0] = x;
}

template <typename T> float Image<T>::getSliceDirectionY() const
{
    return im.head.slice_dir[1];
}

template <typename T> void Image<T>::setSliceDirectionY(float y)
{
    im.head.slice_dir[1] = y;
}

template <typename T> float Image<T>::getSliceDirectionZ() const
{
    return im.head.slice_dir[2];
}

template <typename T> void Image<T>::setSliceDirectionZ(float z)
{
    im.head.slice_dir[2] = z;
}
    
template <typename T> void Image<T>::setPatientTablePosition(float x, float y, float z)
{
    im.head.patient_table_position[0] = x;
    im.head.patient_table_position[1] = y;
    im.head.patient_table_position[2] = z;
}

template <typename T> float Image<T>::getPatientTablePositionX() const
{
    return im.head.patient_table_position[0];
}

template <typename T> void Image<T>::setPatientTablePositionX(float x)
{
    im.head.patient_table_position[0] = x;
}

template <typename T> float Image<T>::getPatientTablePositionY() const
{
    return im.head.patient_table_position[1];
}

template <typename T> void Image<T>::setPatientTablePositionY(float y)
{
    im.head.patient_table_position[1] = y;
}

template <typename T> float Image<T>::getPatientTablePositionZ() const
{
    return im.head.patient_table_position[2];
}

template <typename T> void Image<T>::setPatientTablePositionZ(float z)
{
    im.head.patient_table_position[2] = z;
}

template <typename T> uint16_t Image<T>::getVersion() const
{
    return im.head.version;
}

template <typename T> ISMRMRD_DataTypes Image<T>::getDataType() const
{
    return static_cast<ISMRMRD_DataTypes>(im.head.data_type);
}

template <typename T> uint32_t Image<T>::getMeasurementUid() const
{
    return im.head.measurement_uid;
}

template <typename T> void Image<T>::setMeasurementUid(uint32_t measurement_uid)
{
    im.head.measurement_uid = measurement_uid;
}


template <typename T> uint16_t Image<T>::getAverage() const
{
    return im.head.average;
}

template <typename T> void  Image<T>::setAverage(uint16_t average)
{
    im.head.average = average;
}

template <typename T> uint16_t Image<T>::getSlice() const
{
    return im.head.slice;
}

template <typename T> void  Image<T>::setSlice(uint16_t slice)
{
    im.head.slice = slice;
}

template <typename T> uint16_t Image<T>::getContrast() const
{
    return im.head.contrast;
}

template <typename T> void  Image<T>::setContrast(uint16_t contrast)
{
    im.head.contrast = contrast;
}

template <typename T> uint16_t Image<T>::getPhase() const
{
    return im.head.phase;
}

template <typename T> void  Image<T>::setPhase(uint16_t phase)
{
    im.head.phase = phase;
}
    
template <typename T> uint16_t Image<T>::getRepetition() const
{
    return im.head.repetition;
}

template <typename T> void  Image<T>::setRepetition(uint16_t repetition)
{
    im.head.repetition = repetition;
}

template <typename T> uint16_t Image<T>::getSet() const
{
    return im.head.set;
}

template <typename T> void  Image<T>::setSet(uint16_t set)
{
    im.head.set = set;
}

template <typename T> uint32_t Image<T>::getAcquisitionTimeStamp() const
{
    return im.head.acquisition_time_stamp;
}

template <typename T> void  Image<T>::setAcquisitionTimeStamp(uint32_t acquisition_time_stamp)
{
    im.head.acquisition_time_stamp = acquisition_time_stamp;
}

template <typename T> uint32_t Image<T>::getPhysiologyTimeStamp(unsigned int stamp_id) const
{
    return im.head.physiology_time_stamp[stamp_id];
}

template <typename T> void  Image<T>::setPhysiologyTimeStamp(unsigned int stamp_id, uint32_t value)
{
    im.head.physiology_time_stamp[stamp_id] = value;
}

template <typename T> uint16_t Image<T>::getImageType() const
{
    return im.head.image_type;
}

template <typename T> void Image<T>::setImageType(uint16_t image_type)
{
    im.head.image_type = image_type;
}

template <typename T> uint16_t Image<T>::getImageIndex() const
{
    return im.head.image_index;
}

template <typename T> void Image<T>::setImageIndex(uint16_t image_index)
{
    im.head.image_index = image_index;
}

template <typename T> uint16_t Image<T>::getImageSeriesIndex() const
{
    return im.head.image_series_index;
}

template <typename T> void Image<T>::setImageSeriesIndex(uint16_t image_series_index)
{
    im.head.image_series_index = image_series_index;
}

    
// User parameters
template <typename T> float Image<T>::getUserFloat(unsigned int index) const
{
    return im.head.user_float[index];
}

template <typename T> void Image<T>::setUserFloat(unsigned int index, float value)
{
    im.head.user_float[index] = value;
}

template <typename T> int32_t Image<T>::getUserInt(unsigned int index) const
{
    return im.head.user_int[index];
}

template <typename T> void Image<T>::setUserInt(unsigned int index, int32_t value)
{
    im.head.user_int[index] = value;
}


// Flag methods
template <typename T> uint64_t Image<T>::getFlags() const {
    return im.head.flags;
}

template <typename T> bool Image<T>::isFlagSet(const uint64_t val) const {
    return ismrmrd_is_flag_set(im.head.flags, val);
}

template <typename T> void Image<T>::setFlag(const uint64_t val) {
    ismrmrd_set_flag(&(im.head.flags), val);
}

template <typename T> void Image<T>::setFlags(const uint64_t val) {
    ismrmrd_set_flags(&(im.head.flags), val);
}

template <typename T> void Image<T>::clearFlag(const uint64_t val) {
    ismrmrd_clear_flag(&(im.head.flags), val);
}

template <typename T> void Image<T>::clearAllFlags() {
    ismrmrd_clear_all_flags(&(im.head.flags));
}

// Header
template <typename T> ImageHeader &Image<T>::getHead() {
    // This returns a reference
    return *static_cast<ImageHeader *>(&im.head);
}

template <typename T> const ImageHeader &Image<T>::getHead() const {
    // This returns a reference
    return *static_cast<const ImageHeader *>(&im.head);
}

template <typename T> void Image<T>::setHead(const ImageHeader &other) {
    if (other.data_type != im.head.data_type) {
        throw std::runtime_error("Cannot assign a header of a different data type.");
    }
    
    memcpy(&im.head, &other, sizeof(ImageHeader));
    if (ismrmrd_make_consistent_image(&im) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

// Attribute string
template <typename T> void Image<T>::getAttributeString(std::string &attr) const
{
   if (im.attribute_string)
      attr.assign(im.attribute_string);
   else
      attr.assign("");
}

template <typename T> const char *Image<T>::getAttributeString() const
{
   return im.attribute_string;
}

template <typename T> void Image<T>::setAttributeString(const std::string &attr)
{
    this->setAttributeString(attr.c_str());
}

template <typename T> void Image<T>::setAttributeString(const char *attr)
{
    // Get the string length
    size_t length = strlen(attr);

    // Allocate space plus a null terminator and check for success
    char *newPointer = (char *)realloc(im.attribute_string, (length+1) * sizeof(*im.attribute_string));
    if (NULL==newPointer) {
        throw std::runtime_error(build_exception_string());
    }

    // Make changes only if reallocation was successful
    im.attribute_string = newPointer;
    im.head.attribute_string_len = static_cast<uint32_t>(length);

    // Set the null terminator and copy the string
    im.attribute_string[length] = '\0';
    strncpy(im.attribute_string, attr, length);
}

template <typename T> size_t Image<T>::getAttributeStringLength() const
{
    return im.head.attribute_string_len;
}

// Data
template <typename T> T * Image<T>::getDataPtr() {
     return static_cast<T*>(im.data);
}

template <typename T> const T * Image<T>::getDataPtr() const {
     return static_cast<const T*>(im.data);
}

template <typename T> size_t Image<T>::getNumberOfDataElements() const {
    size_t num = 1;
    num *= im.head.matrix_size[0];
    num *= im.head.matrix_size[1];
    num *= im.head.matrix_size[2];
    num *= im.head.channels;
    return num;
}

template <typename T> size_t Image<T>::getDataSize() const {
    return ismrmrd_size_of_image_data(&im);
}

template <typename T> T * Image<T>::begin() {
     return static_cast<T*>(im.data);
}

template <typename T> T * Image<T>::end() {
     return static_cast<T*>(im.data)+this->getNumberOfDataElements();
}

template <typename T> T & Image<T>::operator () (uint16_t ix, uint16_t iy, uint16_t iz, uint16_t channel) {
     size_t index = ix \
             + size_t(im.head.matrix_size[0])*iy \
             + size_t(im.head.matrix_size[1])*size_t(im.head.matrix_size[0])*iz \
             + size_t(im.head.matrix_size[1])*size_t(im.head.matrix_size[0])*size_t(im.head.matrix_size[2])*channel;
     return static_cast<T*>(im.data)[index];
}

//
// Array class Implementation
//
template <typename T> NDArray<T>::NDArray()
{
    if (ismrmrd_init_ndarray(&arr) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
    arr.data_type = static_cast<uint16_t>(get_data_type<T>());
}

template <typename T> NDArray<T>::NDArray(const std::vector<size_t> dimvec)
{
    if (ismrmrd_init_ndarray(&arr) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
    arr.data_type = static_cast<uint16_t>(get_data_type<T>());
    resize(dimvec);
}

template <typename T> NDArray<T>::NDArray(const NDArray<T> &other)
{
    int err = 0;
    err = ismrmrd_init_ndarray(&arr);
    if (err) {
        throw std::runtime_error(build_exception_string());
    }
    err = ismrmrd_copy_ndarray(&arr, &other.arr);
    if (err) {
        throw std::runtime_error(build_exception_string());
    }
}

template <typename T> NDArray<T>::~NDArray()
{
    if (ismrmrd_cleanup_ndarray(&arr) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

template <typename T> NDArray<T> & NDArray<T>::operator= (const NDArray<T> &other)
{
    int err = 0;
    // Assignment makes a copy
    if (this != &other )
    {
        err = ismrmrd_init_ndarray(&arr);
        if (err) {
            throw std::runtime_error(build_exception_string());
        }
        err = ismrmrd_copy_ndarray(&arr, &other.arr);
        if (err) {
            throw std::runtime_error(build_exception_string());
        }
    }
    return *this;
}

template <typename T> uint16_t NDArray<T>::getVersion() const {
    return arr.version;
};

template <typename T> ISMRMRD_DataTypes NDArray<T>::getDataType() const {
    return static_cast<ISMRMRD_DataTypes>( arr.data_type );
}

template <typename T> uint16_t NDArray<T>::getNDim() const {
    return  arr.ndim;
};
    
template <typename T> const size_t (&NDArray<T>::getDims())[ISMRMRD_NDARRAY_MAXDIM] {
    return arr.dims;
};

template <typename T> void NDArray<T>::resize(const std::vector<size_t> dimvec) {
    if (dimvec.size() > ISMRMRD_NDARRAY_MAXDIM) {
        throw std::runtime_error("Input vector dimvec is too long.");
    }
    arr.ndim = static_cast<uint16_t>(dimvec.size());
    for (int n=0; n<arr.ndim; n++) {
        arr.dims[n] = dimvec[n];
    }
    if (ismrmrd_make_consistent_ndarray(&arr) != ISMRMRD_NOERROR) {
        throw std::runtime_error(build_exception_string());
    }
}

template <typename T> T * NDArray<T>::getDataPtr() {
    return static_cast<T*>(arr.data);
}

template <typename T> const T * NDArray<T>::getDataPtr() const {
    return static_cast<T*>(arr.data);
}

template <typename T> size_t NDArray<T>::getDataSize() const {
    return ismrmrd_size_of_ndarray_data(&arr);
}

template <typename T> size_t NDArray<T>::getNumberOfElements() const {
    size_t num = 1;
    for (int n = 0; n < arr.ndim; n++) {
        size_t v = arr.dims[n];
        // This is necessary to prevent bad GCC loop optimization!
        if (v > 0) {
            num *= v;
        }
    }
    return num;
}

template <typename T> T * NDArray<T>::begin() {
    return static_cast<T*>(arr.data);
}

template <typename T> T * NDArray<T>::end() {
    return static_cast<T*>(arr.data)+this->getNumberOfElements();
}

template <typename T> T & NDArray<T>::operator () (uint16_t x, uint16_t y, uint16_t z, uint16_t w, uint16_t n, uint16_t m, uint16_t l){
       size_t index = 0;
       uint16_t indices[ISMRMRD_NDARRAY_MAXDIM] = {x,y,z,w,n,m,l};
       size_t stride = 1;
       for (uint16_t i = 0; i < arr.ndim; i++){
               index += indices[i]*stride;
               stride *= arr.dims[i];
       }

       return static_cast<T*>(arr.data)[index];
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


// Helper function for generating exception message from ISMRMRD error stack
std::string build_exception_string(void)
{
    char *file = NULL, *func = NULL, *msg = NULL;
    int line = 0, code = 0;
    std::stringstream stream;
    for (int i = 0; ismrmrd_pop_error(&file, &line, &func, &code, &msg); ++i) {
        if (i > 0) {
            stream << std::endl;
        }
        stream << "ISMRMRD " << ismrmrd_strerror(code) << " in " << func <<
                " (" << file << ":" << line << ": " << msg;
    }
    return stream.str();
}


} // namespace ISMRMRD
