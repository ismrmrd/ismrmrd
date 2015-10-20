//#include <stdlib.h>
#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/version.h"

#include <sstream>
#include <stdexcept>
#include <iostream>

#include <string.h>


namespace ISMRMRD {

/** Returns the size in bytes of a StorageType */
/*
static size_t sizeof_storage_type(int storage_type)
{
    size_t size = 0;
    switch (storage_type) {
        case ISMRMRD_USHORT:
            size = sizeof(uint16_t);
            break;
        case ISMRMRD_SHORT:
            size = sizeof(int16_t);
            break;
        case ISMRMRD_UINT:
            size = sizeof(uint32_t);
            break;
        case ISMRMRD_INT:
            size = sizeof(int32_t);
            break;
        case ISMRMRD_FLOAT:
            size = sizeof(float);
            break;
        case ISMRMRD_DOUBLE:
            size = sizeof(double);
            break;
        case ISMRMRD_CXFLOAT:
            size = sizeof(std::complex<float>);
            break;
        case ISMRMRD_CXDOUBLE:
            size = sizeof(std::complex<double>);
            break;
        default:
            size = 0;
    }
    return size;
}
*/

// Allowed data types for Images and NDArrays
template <> EXPORTISMRMRD StorageType get_storage_type<uint16_t>()
{
    return ISMRMRD_USHORT;
}

template <> EXPORTISMRMRD StorageType get_storage_type<int16_t>()
{
    return ISMRMRD_SHORT;
}

template <> EXPORTISMRMRD StorageType get_storage_type<uint32_t>()
{
    return ISMRMRD_UINT;
}

template <> EXPORTISMRMRD StorageType get_storage_type<int32_t>()
{
    return ISMRMRD_INT;
}

template <> EXPORTISMRMRD StorageType get_storage_type<float>()
{
    return ISMRMRD_FLOAT;
}

template <> EXPORTISMRMRD StorageType get_storage_type<double>()
{
    return ISMRMRD_DOUBLE;
}

template <> EXPORTISMRMRD StorageType get_storage_type<std::complex<float> >()
{
    return ISMRMRD_CXFLOAT;
}

template <> EXPORTISMRMRD StorageType get_storage_type<std::complex<double> >()
{
    return ISMRMRD_CXDOUBLE;
}


bool operator==(const AcquisitionHeader& h1, const AcquisitionHeader& h2)
{
    return memcmp(&h1, &h2, sizeof(h1)) == 0;
}

bool operator==(const ImageHeader& h1, const ImageHeader& h2)
{
    return memcmp(&h1, &h2, sizeof(h1)) == 0;
}

Acquisition::Acquisition(uint16_t num_samples, uint16_t active_channels, uint16_t trajectory_dimensions)
{
    memset(&head_, 0, sizeof(head_));
    head_.version = ISMRMRD_VERSION_MAJOR;
    head_.storage_type = ISMRMRD_CXFLOAT;
    this->resize(num_samples, active_channels, trajectory_dimensions);
}

// Accessors and mutators
uint16_t Acquisition::getVersion() const {
    return head_.version;
}

StorageType Acquisition::getStorageType() const {
    return static_cast<StorageType>(head_.storage_type);
}

uint16_t Acquisition::getStreamNumber() const {
    return head_.stream_number;
}

void Acquisition::setStreamNumber(uint16_t stream_number) {
    head_.stream_number = stream_number;
}

uint32_t Acquisition::getMeasurementUID() const {
    return head_.measurement_uid;
}

void Acquisition::setMeasurementUID(uint32_t uid) {
    head_.measurement_uid = uid;
}

uint32_t Acquisition::getScanCounter() const {
    return head_.scan_counter;
}

void Acquisition::setScanCounter(uint32_t counter) {
    head_.scan_counter = counter;
}

uint32_t Acquisition::getAcquisitionTimeStamp() const {
    return head_.acquisition_time_stamp;
}

void Acquisition::setAcquisitionTimeStamp(uint32_t ts) {
    head_.acquisition_time_stamp = ts;
}

uint32_t Acquisition::getPhysiologyTimeStamp(int idx) {
    // TODO: bounds checking
    return head_.physiology_time_stamp[idx];
}

void Acquisition::setPhysiologyTimeStamp(int idx, uint32_t ts) {
    // TODO: bounds checking
    head_.physiology_time_stamp[idx] = ts;
}

uint16_t Acquisition::getNumberOfSamples() const {
    return head_.number_of_samples;
}

void Acquisition::setNumberOfSamples(uint16_t ns) {
    head_.number_of_samples = ns;
    this->makeConsistent();
}

uint16_t Acquisition::getAvailableChannels() const {
    return head_.available_channels;
}

void Acquisition::setAvailableChannels(uint16_t ac) {
    // TODO: compare against head.active_channels or makeConsistent?
    head_.available_channels = ac;
}

uint16_t Acquisition::getActiveChannels() const {
    return head_.active_channels;
}

void Acquisition::setActiveChannels(uint16_t ac) {
    head_.active_channels = ac;
    this->makeConsistent();
}

uint64_t Acquisition::getChannelMask(int idx) {
    // TODO: bounds checking
    return head_.channel_mask[idx];
}

void Acquisition::setChannelMask(int idx, uint64_t mask) {
    // TODO: bounds checking
    head_.channel_mask[idx] = mask;
}

uint16_t Acquisition::getDiscardPre() const {
    return head_.discard_pre;
}

void Acquisition::setDiscardPre(uint16_t dp) {
    head_.discard_pre = dp;
}

uint16_t Acquisition::getDiscardPost() const {
    return head_.discard_post;
}

void Acquisition::setDiscardPost(uint16_t dp) {
    head_.discard_post = dp;
}

uint16_t Acquisition::getCenterSample() const {
    return head_.center_sample;
}

void Acquisition::setCenterSample(uint16_t cs) {
    head_.center_sample = cs;
}

uint16_t Acquisition::getEncodingSpaceRef() const {
    return head_.encoding_space_ref;
}

void Acquisition::setEncodingSpaceRef(uint16_t esr) {
    head_.encoding_space_ref = esr;
}

uint16_t Acquisition::getTrajectoryDimensions() const {
    return head_.trajectory_dimensions;
}

void Acquisition::setTrajectoryDimensions(uint16_t td) {
    head_.trajectory_dimensions = td;
}

float Acquisition::getSampleTime_us() const {
    return head_.sample_time_us;
}

void Acquisition::setSampleTime_us(float time){
    head_.sample_time_us = time;
}

float Acquisition::getPositionX() const {
    return head_.position[0];
}

float Acquisition::getPositionY() const {
    return head_.position[1];
}

float Acquisition::getPositionZ() const {
    return head_.position[2];
}

void Acquisition::setPosition(float x, float y, float z) {
    setPositionX(x);
    setPositionY(y);
    setPositionZ(z);
}

void Acquisition::setPositionX(float x) {
    head_.position[0] = x;
}

void Acquisition::setPositionY(float y) {
    head_.position[1] = y;
}

void Acquisition::setPositionZ(float z) {
    head_.position[2] = z;
}

float Acquisition::getReadDirectionX() const {
    return head_.read_dir[0];
}

float Acquisition::getReadDirectionY() const {
    return head_.read_dir[1];
}

float Acquisition::getReadDirectionZ() const {
    return head_.read_dir[2];
}

void Acquisition::setReadDirection(float x, float y, float z) {
    setReadDirectionX(x);
    setReadDirectionY(y);
    setReadDirectionZ(z);
}

void Acquisition::setReadDirectionX(float x) {
    head_.read_dir[0] = x;
}

void Acquisition::setReadDirectionY(float y) {
    head_.read_dir[1] = y;
}

void Acquisition::setReadDirectionZ(float z) {
    head_.read_dir[2] = z;
}

float Acquisition::getPhaseDirectionX() const {
    return head_.phase_dir[0];
}

float Acquisition::getPhaseDirectionY() const {
    return head_.phase_dir[1];
}

float Acquisition::getPhaseDirectionZ() const {
    return head_.phase_dir[2];
}

void Acquisition::setPhaseDirection(float x, float y, float z) {
    setPhaseDirectionX(x);
    setPhaseDirectionY(y);
    setPhaseDirectionZ(z);
}

void Acquisition::setPhaseDirectionX(float x) {
    head_.phase_dir[0] = x;
}

void Acquisition::setPhaseDirectionY(float y) {
    head_.phase_dir[1] = y;
}

void Acquisition::setPhaseDirectionZ(float z) {
    head_.phase_dir[2] = z;
}

float Acquisition::getSliceDirectionX() const {
    return head_.slice_dir[0];
}

float Acquisition::getSliceDirectionY() const {
    return head_.slice_dir[1];
}

float Acquisition::getSliceDirectionZ() const {
    return head_.slice_dir[2];
}

void Acquisition::setSliceDirection(float x, float y, float z) {
    setSliceDirectionX(x);
    setSliceDirectionY(y);
    setSliceDirectionZ(z);
}

void Acquisition::setSliceDirectionX(float x) {
    head_.slice_dir[0] = x;
}

void Acquisition::setSliceDirectionY(float y) {
    head_.slice_dir[1] = y;
}

void Acquisition::setSliceDirectionZ(float z) {
    head_.slice_dir[2] = z;
}

float Acquisition::getPatientTablePositionX() const {
    return head_.patient_table_position[0];
}

float Acquisition::getPatientTablePositionY() const {
    return head_.patient_table_position[1];
}

float Acquisition::getPatientTablePositionZ() const {
    return head_.patient_table_position[2];
}

void Acquisition::setPatientTablePosition(float x, float y, float z) {
    setPatientTablePositionX(x);
    setPatientTablePositionY(y);
    setPatientTablePositionZ(z);
}

void Acquisition::setPatientTablePositionX(float x) {
    head_.patient_table_position[0] = x;
}

void Acquisition::setPatientTablePositionY(float y) {
    head_.patient_table_position[1] = y;
}

void Acquisition::setPatientTablePositionZ(float z) {
    head_.patient_table_position[2] = z;
}

EncodingCounters& Acquisition::getEncodingCounters() {
    return head_.idx;
}

const EncodingCounters& Acquisition::getEncodingCounters() const {
    return head_.idx;
}

void Acquisition::setEncodingCounters(const EncodingCounters& idx) {
    head_.idx = idx;
}

int32_t Acquisition::getUserInt(int idx) const {
    // TODO: bounds checking
    return head_.user_int[idx];
}

void Acquisition::setUserInt(int idx, int32_t val) {
    // TODO: bounds checking
    head_.user_int[idx] = val;
}

float Acquisition::getUserFloat(int idx) const {
    // TODO: bounds checking
    return head_.user_float[idx];
}

void Acquisition::setUserFloat(int idx, float val) {
    // TODO: bounds checking
    head_.user_float[idx] = val;
}

size_t Acquisition::getNumberOfDataElements() const {
    return head_.number_of_samples * head_.active_channels;
}

size_t Acquisition::getNumberOfTrajElements() const {
    return head_.number_of_samples * head_.trajectory_dimensions;
}

AcquisitionHeader& Acquisition::getHead() {
    return head_;
}

const AcquisitionHeader & Acquisition::getHead() const {
    return head_;
}

void Acquisition::setHead(const AcquisitionHeader &other) {
    this->head_ = other;
    this->makeConsistent();
}

void Acquisition::resize(uint16_t num_samples, uint16_t active_channels, uint16_t trajectory_dimensions){
    head_.number_of_samples = num_samples;
    head_.active_channels = active_channels;
    head_.trajectory_dimensions = trajectory_dimensions;
    this->makeConsistent();
}

void Acquisition::makeConsistent() {
    if (head_.available_channels < head_.active_channels) {
        head_.available_channels = head_.active_channels;
    }
    traj_.resize(head_.number_of_samples * head_.trajectory_dimensions);
    data_.resize(head_.number_of_samples * head_.active_channels);
}

std::vector<std::complex<float> >& Acquisition::getData() {
    return data_;
}

const std::vector<std::complex<float> >& Acquisition::getData() const {
    return data_;
}

void Acquisition::setData(const std::vector<std::complex<float> >& data) {
    if (data.size() != getNumberOfDataElements()) {
        throw std::runtime_error("data size does not match size specified by header");
    }
    this->data_ = data;
}

std::complex<float>& Acquisition::at(uint16_t sample, uint16_t channel){
    if (sample >= getNumberOfSamples()) {
        throw std::runtime_error("sample greater than number of samples");
    }
    if (channel >= getActiveChannels()) {
        throw std::runtime_error("channel greater than number of active channels");
    }
    return data_[sample + channel * getNumberOfSamples()];
}

const std::vector<float>& Acquisition::getTraj() const {
    return traj_;
}

void Acquisition::setTraj(const std::vector<float>& traj) {
    this->traj_ = traj;
}

float& Acquisition::trajAt(uint16_t dimension, uint16_t sample){
    if (sample >= getNumberOfSamples()) {
        throw std::runtime_error("sample greater than number of samples");
    }
    if (dimension >= getTrajectoryDimensions()) {
        throw std::runtime_error("trajectory greater than trajectory_dimensions");
    }
    return traj_[sample * head_.trajectory_dimensions + dimension];
}

uint64_t Acquisition::getFlags() const {
    return head_.flags;
}

void Acquisition::setFlags(uint64_t val) {
    head_.flags = val;
}

bool Acquisition::isFlagSet(uint64_t val) const {
    uint64_t bitmask = 1UL << (val - 1UL);
    return (head_.flags & bitmask) > 0;
}

void Acquisition::setFlag(uint64_t val) {
    uint64_t bitmask = 1UL << (val - 1UL);
    head_.flags |= bitmask;
}

void Acquisition::clearFlag(uint64_t val) {
    uint64_t bitmask = 1UL << (val - 1UL);
    head_.flags &= ~bitmask;
}

void Acquisition::clearAllFlags() {
    head_.flags = 0;
}

bool Acquisition::isChannelActive(uint16_t chan) const {
    uint64_t bitmask = 1UL << (chan % 64UL);
    size_t offset = chan / 64UL;
    return (head_.channel_mask[offset] & bitmask) > 0;
}

void Acquisition::setChannelActive(uint16_t chan) {
    uint64_t bitmask = 1UL << (chan % 64UL);
    size_t offset = chan / 64UL;
    head_.channel_mask[offset] |= bitmask;
}

void Acquisition::setChannelNotActive(uint16_t chan) {
    uint64_t bitmask = 1UL << (chan % 64UL);
    size_t offset = chan / 64UL;
    head_.channel_mask[offset] &= ~bitmask;
}

void Acquisition::setAllChannelsNotActive() {
    for (size_t offset = 0; offset<ISMRMRD_CHANNEL_MASKS; offset++) {
        head_.channel_mask[offset] = 0;
    }
}


template <typename T> Image<T>::Image(uint16_t matrix_size_x,
                                      uint16_t matrix_size_y,
                                      uint16_t matrix_size_z,
                                      uint16_t channels)
{
    memset(&head_, 0, sizeof(head_));
    head_.version = ISMRMRD_VERSION_MAJOR;
    head_.storage_type = static_cast<uint16_t>(get_storage_type<T>());
    this->resize(matrix_size_x, matrix_size_y, matrix_size_z, channels);
}

// Image dimensions
template <typename T> void Image<T>::resize(uint16_t matrix_size_x,
                                            uint16_t matrix_size_y,
                                            uint16_t matrix_size_z,
                                            uint16_t channels)
{
    head_.matrix_size[0] = matrix_size_x;
    head_.matrix_size[1] = matrix_size_y;
    head_.matrix_size[2] = matrix_size_z;
    head_.channels = channels;
    this->makeConsistent();
}

template <typename T> void Image<T>::makeConsistent() {
    // TODO what if matrix_size[0] = 0?

    if (head_.matrix_size[1] == 0) {
        head_.matrix_size[1] = 1;
    }
    if (head_.matrix_size[2] == 0) {
        head_.matrix_size[2] = 1;
    }
    if (head_.channels == 0) {
        head_.channels = 1;
    }
    data_.resize(getNumberOfElements());

    attribute_string_.resize(head_.attribute_string_len);
}

template <typename T> uint16_t Image<T>::getMatrixSizeX() const
{
    return head_.matrix_size[0];
}

template <typename T> uint16_t Image<T>::getMatrixSizeY() const
{
    return head_.matrix_size[1];
}

template <typename T> uint16_t Image<T>::getMatrixSizeZ() const
{
    return head_.matrix_size[2];
}

template <typename T> void Image<T>::setMatrixSizeX(uint16_t x)
{
    head_.matrix_size[0] = x;
    this->makeConsistent();
}

template <typename T> void Image<T>::setMatrixSizeY(uint16_t y)
{
    head_.matrix_size[1] = y;
    this->makeConsistent();
}

template <typename T> void Image<T>::setMatrixSizeZ(uint16_t z)
{
    head_.matrix_size[2] = z;
    this->makeConsistent();
}

template <typename T> uint16_t Image<T>::getNumberOfChannels() const
{
    return head_.channels;
}

template <typename T> void Image<T>::setNumberOfChannels(uint16_t channels)
{
    head_.channels = channels;
    this->makeConsistent();
}


template <typename T> float Image<T>::getFieldOfViewX() const
{
    return head_.field_of_view[0];
}

template <typename T> float Image<T>::getFieldOfViewY() const
{
    return head_.field_of_view[1];
}

template <typename T> float Image<T>::getFieldOfViewZ() const
{
    return head_.field_of_view[2];
}

template <typename T> void Image<T>::setFieldOfView(float fov_x, float fov_y, float fov_z)
{
    setFieldOfViewX(fov_x);
    setFieldOfViewY(fov_y);
    setFieldOfViewZ(fov_z);
}

template <typename T> void Image<T>::setFieldOfViewX(float fov_x)
{
    head_.field_of_view[0] = fov_x;
}

template <typename T> void Image<T>::setFieldOfViewY(float fov_y)
{
    head_.field_of_view[1] = fov_y;
}

template <typename T> void Image<T>::setFieldOfViewZ(float fov_z)
{
    head_.field_of_view[2] = fov_z;
}

// Positions and orientations
template <typename T> float Image<T>::getPositionX() const
{
    return head_.position[0];
}

template <typename T> float Image<T>::getPositionY() const
{
    return head_.position[1];
}

template <typename T> float Image<T>::getPositionZ() const
{
    return head_.position[2];
}

template <typename T> void Image<T>::setPosition(float x, float y, float z)
{
    setPositionX(x);
    setPositionY(y);
    setPositionZ(z);
}

template <typename T> void Image<T>::setPositionX(float x)
{
    head_.position[0] = x;
}

template <typename T> void Image<T>::setPositionY(float y)
{
    head_.position[1] = y;
}

template <typename T> void Image<T>::setPositionZ(float z)
{
    head_.position[2] = z;
}


template <typename T> float Image<T>::getReadDirectionX() const
{
    return head_.read_dir[0];
}

template <typename T> float Image<T>::getReadDirectionY() const
{
    return head_.read_dir[1];
}

template <typename T> float Image<T>::getReadDirectionZ() const
{
    return head_.read_dir[2];
}

template <typename T> void Image<T>::setReadDirection(float x, float y, float z)
{
    setReadDirectionX(x);
    setReadDirectionY(y);
    setReadDirectionZ(z);
}

template <typename T> void Image<T>::setReadDirectionX(float x)
{
    head_.read_dir[0] = x;
}

template <typename T> void Image<T>::setReadDirectionY(float y)
{
    head_.read_dir[1] = y;
}

template <typename T> void Image<T>::setReadDirectionZ(float z)
{
    head_.read_dir[2] = z;
}


template <typename T> float Image<T>::getPhaseDirectionX() const
{
    return head_.phase_dir[0];
}

template <typename T> float Image<T>::getPhaseDirectionY() const
{
    return head_.phase_dir[1];
}

template <typename T> float Image<T>::getPhaseDirectionZ() const
{
    return head_.phase_dir[2];
}

template <typename T> void Image<T>::setPhaseDirection(float x, float y, float z)
{
    setPhaseDirectionX(x);
    setPhaseDirectionY(y);
    setPhaseDirectionZ(z);
}

template <typename T> void Image<T>::setPhaseDirectionX(float x)
{
    head_.phase_dir[0] = x;
}

template <typename T> void Image<T>::setPhaseDirectionY(float y)
{
    head_.phase_dir[1] = y;
}

template <typename T> void Image<T>::setPhaseDirectionZ(float z)
{
    head_.phase_dir[2] = z;
}

template <typename T> float Image<T>::getSliceDirectionX() const
{
    return head_.slice_dir[0];
}

template <typename T> float Image<T>::getSliceDirectionY() const
{
    return head_.slice_dir[1];
}

template <typename T> float Image<T>::getSliceDirectionZ() const
{
    return head_.slice_dir[2];
}

template <typename T> void Image<T>::setSliceDirection(float x, float y, float z)
{
    setSliceDirectionX(x);
    setSliceDirectionY(y);
    setSliceDirectionZ(z);
}

template <typename T> void Image<T>::setSliceDirectionX(float x)
{
    head_.slice_dir[0] = x;
}

template <typename T> void Image<T>::setSliceDirectionY(float y)
{
    head_.slice_dir[1] = y;
}

template <typename T> void Image<T>::setSliceDirectionZ(float z)
{
    head_.slice_dir[2] = z;
}

template <typename T> float Image<T>::getPatientTablePositionX() const
{
    return head_.patient_table_position[0];
}

template <typename T> float Image<T>::getPatientTablePositionY() const
{
    return head_.patient_table_position[1];
}

template <typename T> float Image<T>::getPatientTablePositionZ() const
{
    return head_.patient_table_position[2];
}

template <typename T> void Image<T>::setPatientTablePosition(float x, float y, float z)
{
    setPatientTablePositionX(x);
    setPatientTablePositionY(y);
    setPatientTablePositionZ(z);
}

template <typename T> void Image<T>::setPatientTablePositionX(float x)
{
    head_.patient_table_position[0] = x;
}

template <typename T> void Image<T>::setPatientTablePositionY(float y)
{
    head_.patient_table_position[1] = y;
}

template <typename T> void Image<T>::setPatientTablePositionZ(float z)
{
    head_.patient_table_position[2] = z;
}

template <typename T> uint16_t Image<T>::getVersion() const
{
    return head_.version;
}

template <typename T> StorageType Image<T>::getStorageType() const
{
    return static_cast<StorageType>(head_.storage_type);
}

template <typename T> uint32_t Image<T>::getMeasurementUID() const
{
    return head_.measurement_uid;
}

template <typename T> void Image<T>::setMeasurementUID(uint32_t measurement_uid)
{
    head_.measurement_uid = measurement_uid;
}

template <typename T> uint16_t Image<T>::getAverage() const
{
    return head_.average;
}

template <typename T> void  Image<T>::setAverage(uint16_t average)
{
    head_.average = average;
}

template <typename T> uint16_t Image<T>::getSlice() const
{
    return head_.slice;
}

template <typename T> void  Image<T>::setSlice(uint16_t slice)
{
    head_.slice = slice;
}

template <typename T> uint16_t Image<T>::getContrast() const
{
    return head_.contrast;
}

template <typename T> void  Image<T>::setContrast(uint16_t contrast)
{
    head_.contrast = contrast;
}

template <typename T> uint16_t Image<T>::getPhase() const
{
    return head_.phase;
}

template <typename T> void  Image<T>::setPhase(uint16_t phase)
{
    head_.phase = phase;
}

template <typename T> uint16_t Image<T>::getRepetition() const
{
    return head_.repetition;
}

template <typename T> void  Image<T>::setRepetition(uint16_t repetition)
{
    head_.repetition = repetition;
}

template <typename T> uint16_t Image<T>::getSet() const
{
    return head_.set;
}

template <typename T> void  Image<T>::setSet(uint16_t set)
{
    head_.set = set;
}

template <typename T> uint32_t Image<T>::getAcquisitionTimeStamp() const
{
    return head_.acquisition_time_stamp;
}

template <typename T> void  Image<T>::setAcquisitionTimeStamp(uint32_t acquisition_time_stamp)
{
    head_.acquisition_time_stamp = acquisition_time_stamp;
}

template <typename T> uint32_t Image<T>::getPhysiologyTimeStamp(unsigned int idx) const
{
    // TODO: bounds checking
    return head_.physiology_time_stamp[idx];
}

template <typename T> void  Image<T>::setPhysiologyTimeStamp(unsigned int idx, uint32_t value)
{
    // TODO: bounds checking
    head_.physiology_time_stamp[idx] = value;
}

template <typename T> uint16_t Image<T>::getImageType() const
{
    return head_.image_type;
}

template <typename T> void Image<T>::setImageType(uint16_t image_type)
{
    head_.image_type = image_type;
}

template <typename T> uint16_t Image<T>::getImageIndex() const
{
    return head_.image_index;
}

template <typename T> void Image<T>::setImageIndex(uint16_t image_index)
{
    head_.image_index = image_index;
}

template <typename T> uint16_t Image<T>::getImageSeriesIndex() const
{
    return head_.image_series_index;
}

template <typename T> void Image<T>::setImageSeriesIndex(uint16_t image_series_index)
{
    head_.image_series_index = image_series_index;
}

template <typename T> int32_t Image<T>::getUserInt(unsigned int index) const
{
    // TODO: bounds checking
    return head_.user_int[index];
}

template <typename T> void Image<T>::setUserInt(unsigned int index, int32_t value)
{
    // TODO: bounds checking
    head_.user_int[index] = value;
}

template <typename T> float Image<T>::getUserFloat(unsigned int index) const
{
    // TODO: bounds checking
    return head_.user_float[index];
}

template <typename T> void Image<T>::setUserFloat(unsigned int index, float value)
{
    // TODO: bounds checking
    head_.user_float[index] = value;
}

// Flag methods
template <typename T> uint64_t Image<T>::getFlags() const {
    return head_.flags;
}

template <typename T> void Image<T>::setFlags(uint64_t val) {
    head_.flags = val;
}

template <typename T> bool Image<T>::isFlagSet(uint64_t val) const {
    uint64_t bitmask = 1UL << (val - 1UL);
    return (head_.flags & bitmask) > 0;
}

template <typename T> void Image<T>::setFlag(uint64_t val) {
    uint64_t bitmask = 1UL << (val - 1UL);
    head_.flags |= bitmask;
}

template <typename T> void Image<T>::clearFlag(uint64_t val) {
    uint64_t bitmask = 1UL << (val - 1UL);
    head_.flags &= ~bitmask;
}

template <typename T> void Image<T>::clearAllFlags() {
    head_.flags = 0;
}

template <typename T> ImageHeader &Image<T>::getHead() {
    return head_;
}

template <typename T> const ImageHeader &Image<T>::getHead() const {
    return head_;
}

template <typename T> void Image<T>::setHead(const ImageHeader& other) {
    if (other.storage_type != head_.storage_type) {
        throw std::runtime_error("Cannot assign a header of a different data type.");
    }

    this->head_ = other;
    this->makeConsistent();
}

// Attribute string
template <typename T> void Image<T>::getAttributeString(std::string& attr) const
{
    attr.assign(attribute_string_);
}

template <typename T> const std::string& Image<T>::getAttributeString() const
{
   return attribute_string_;
}

template <typename T> void Image<T>::setAttributeString(const std::string& attr)
{
    attribute_string_ = attr;
    head_.attribute_string_len = attr.size();
}

template <typename T> size_t Image<T>::getAttributeStringLength() const
{
    return head_.attribute_string_len;
}

template <typename T> std::vector<T>& Image<T>::getData() {
    return data_;
}

template <typename T> const std::vector<T>& Image<T>::getData() const {
     return data_;
}

template <typename T> size_t Image<T>::getNumberOfElements() const {
    return head_.matrix_size[0] * head_.matrix_size[1] *
            head_.matrix_size[2] * head_.channels;
}

template <typename T> T& Image<T>::at(uint16_t ix, uint16_t iy, uint16_t iz, uint16_t channel) {
    // TODO: bounds checking
    size_t sx = getMatrixSizeX();
    size_t sy = getMatrixSizeY();
    size_t sz = getMatrixSizeZ();
    size_t index = ix + (sx * iy) + (sy * sx * iz) + (sy * sx * sz * channel);
    return data_[index];
}

//
// Array class Implementation
//
template <typename T> NDArray<T>::NDArray()
    : version_(ISMRMRD_VERSION_MAJOR)
{ }

template <typename T> NDArray<T>::NDArray(const std::vector<size_t>& dims)
    : version_(ISMRMRD_VERSION_MAJOR)
{
    resize(dims);
}

template <typename T> uint16_t NDArray<T>::getVersion() const {
    return version_;
};

template <typename T> StorageType NDArray<T>::getStorageType() const {
    return static_cast<StorageType>(get_storage_type<T>());
}

template <typename T> uint16_t NDArray<T>::getNDim() const {
    return dims_.size();
};

template <typename T> const std::vector<size_t>& NDArray<T>::getDims() {
    return dims_;
};

template <typename T> void NDArray<T>::resize(const std::vector<size_t>& dims) {
    this->dims_ = dims;
    this->makeConsistent();
}

template <typename T> void NDArray<T>::makeConsistent() {
    data_.resize(getNumberOfElements());
}

template <typename T> std::vector<T>& NDArray<T>::getData() {
    return data_;
}

template <typename T> const std::vector<T>& NDArray<T>::getData() const {
    return data_;
}

template <typename T> size_t NDArray<T>::getNumberOfElements() const {
    if (dims_.size() == 0) {
        return 0;
    }

    size_t size = 1;
    for (std::vector<size_t>::const_iterator it = dims_.begin(); it != dims_.end(); ++it) {
        // This is necessary to prevent bad GCC loop optimization!
        if (*it != 0) {
            size *= *it;
        }
    }
    return size;
}

template <typename T> T& NDArray<T>::at(uint16_t x, uint16_t y, uint16_t z,
        uint16_t w, uint16_t n, uint16_t m, uint16_t l)
{
    // TODO: bounds checking
    size_t index = 0;
    uint16_t indices[ISMRMRD_NDARRAY_MAXDIM] = {x,y,z,w,n,m,l};
    size_t stride = 1;
    for (uint16_t i = 0; i < dims_.size(); i++){
        index += indices[i] * stride;
        stride *= dims_[i];
    }

    return data_[index];
}

// Images
template EXPORTISMRMRD class Image<uint16_t>;
template EXPORTISMRMRD class Image<int16_t>;
template EXPORTISMRMRD class Image<uint32_t>;
template EXPORTISMRMRD class Image<int32_t>;
template EXPORTISMRMRD class Image<float>;
template EXPORTISMRMRD class Image<double>;
template EXPORTISMRMRD class Image<std::complex<float> >;
template EXPORTISMRMRD class Image<std::complex<double> >;

// NDArrays
template EXPORTISMRMRD class NDArray<uint16_t>;
template EXPORTISMRMRD class NDArray<int16_t>;
template EXPORTISMRMRD class NDArray<uint32_t>;
template EXPORTISMRMRD class NDArray<int32_t>;
template EXPORTISMRMRD class NDArray<float>;
template EXPORTISMRMRD class NDArray<double>;
template EXPORTISMRMRD class NDArray<std::complex<float> >;
template EXPORTISMRMRD class NDArray<std::complex<double> >;


int sign_of_directions(float read_dir[3], float phase_dir[3], float slice_dir[3]) {
    float r11 = read_dir[0], r12 = phase_dir[0], r13 = slice_dir[0];
    float r21 = read_dir[1], r22 = phase_dir[1], r23 = slice_dir[1];
    float r31 = read_dir[2], r32 = phase_dir[2], r33 = slice_dir[2];

    /* Determinant should be 1 or -1 */
    float deti = (r11 * r22 * r33) + (r12 * r23 * r31) + (r21 * r32 * r13) -
                 (r13 * r22 * r31) - (r12 * r21 * r33) - (r11 * r23 * r32);

    if (deti < 0) {
        return -1;
    } else {
        return 1;
    }
}

void directions_to_quaternion(float read_dir[3], float phase_dir[3],
                                      float slice_dir[3], float quat[4]) {
    float r11 = read_dir[0], r12 = phase_dir[0], r13 = slice_dir[0];
    float r21 = read_dir[1], r22 = phase_dir[1], r23 = slice_dir[1];
    float r31 = read_dir[2], r32 = phase_dir[2], r33 = slice_dir[2];

    double a = 1, b = 0, c = 0, d = 0, s = 0;
    double trace = 0;
    double xd, yd, zd;

    /* verify the sign of the rotation*/
    if (sign_of_directions(read_dir, phase_dir, slice_dir) < 0) {
        /* flip 3rd column */
        r13 = -r13;
        r23 = -r23;
        r33 = -r33;
    }

    /* Compute quaternion parameters */
    /* http://www.cs.princeton.edu/~gewang/projects/darth/stuff/quat_faq.html#Q55 */
    trace = 1.0 + r11 + r22 + r33;
    if (trace > 0.00001) { /* simplest case */
        s = sqrt(trace) * 2;
        a = (r32 - r23) / s;
        b = (r13 - r31) / s;
        c = (r21 - r12) / s;
        d = 0.25 * s;
    } else {
        /* trickier case...
         * determine which major diagonal element has
         * the greatest value... */
        xd = 1.0 + r11 - (r22 + r33); /* 4**b**b */
        yd = 1.0 + r22 - (r11 + r33); /* 4**c**c */
        zd = 1.0 + r33 - (r11 + r22); /* 4**d**d */
        /* if r11 is the greatest */
        if (xd > 1.0) {
            s = 2.0 * sqrt(xd);
            a = 0.25 * s;
            b = (r21 + r12) / s;
            c = (r31 + r13) / s;
            d = (r32 - r23) / s;
        }
        /* else if r22 is the greatest */
        else if (yd > 1.0) {
            s = 2.0 * sqrt(yd);
            a = (r21 + r12) / s;
            b = 0.25 * s;
            c = (r32 + r23) / s;
            d = (r13 - r31) / s;
        }
        /* else, r33 must be the greatest */
        else {
            s = 2.0 * sqrt(zd);
            a = (r13 + r31) / s;
            b = (r23 + r32) / s;
            c = 0.25 * s;
            d = (r21 - r12) / s;
        }

        if (a < 0.0) {
            b = -b;
            c = -c;
            d = -d;
            a = -a;
        }
    }

    quat[0] = (float)a;
    quat[1] = (float)b;
    quat[2] = (float)c;
    quat[3] = (float)d;
}

/* http://www.cs.princeton.edu/~gewang/projects/darth/stuff/quat_faq.html#Q54 */
void quaternion_to_directions(float quat[4], float read_dir[3],
                                      float phase_dir[3], float slice_dir[3]) {
    float a = quat[0], b = quat[1], c = quat[2], d = quat[3];

    read_dir[0] = 1.0f - 2.0f * (b * b + c * c);
    phase_dir[0] = 2.0f * (a * b - c * d);
    slice_dir[0] = 2.0f * (a * c + b * d);

    read_dir[1] = 2.0f * (a * b + c * d);
    phase_dir[1] = 1.0f - 2.0f * (a * a + c * c);
    slice_dir[1] = 2.0f * (b * c - a * d);

    read_dir[2] = 2.0f * (a * c - b * d);
    phase_dir[2] = 2.0f * (b * c + a * d);
    slice_dir[2] = 1.0f - 2.0f * (a * a + b * b);
}

} // namespace ISMRMRD
