#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <stdexcept>

#include <iostream>
#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/version.h"

namespace ISMRMRD {

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
    memset(&head, 0, sizeof(head));
    head.version = ISMRMRD_VERSION_MAJOR;
    this->resize(num_samples, active_channels, trajectory_dimensions);
}

// Accessors and mutators
uint16_t Acquisition::getVersion() const {
    return head.version;
}

uint32_t Acquisition::getMeasurementUID() const {
    return head.measurement_uid;
}

void Acquisition::setMeasurementUID(uint32_t uid) {
    head.measurement_uid = uid;
}

uint32_t Acquisition::getScanCounter() const {
    return head.scan_counter;
}

void Acquisition::setScanCounter(uint32_t counter) {
    head.scan_counter = counter;
}

uint32_t Acquisition::getAcquisitionTimeStamp() const {
    return head.acquisition_time_stamp;
}

void Acquisition::setAcquisitionTimeStamp(uint32_t ts) {
    head.acquisition_time_stamp = ts;
}

uint32_t Acquisition::getPhysiologyTimeStamp(int idx) {
    // TODO: bounds checking
    return head.physiology_time_stamp[idx];
}

void Acquisition::setPhysiologyTimeStamp(int idx, uint32_t ts) {
    // TODO: bounds checking
    head.physiology_time_stamp[idx] = ts;
}

uint16_t Acquisition::getNumberOfSamples() const {
    return head.number_of_samples;
}

void Acquisition::setNumberOfSamples(uint16_t ns) {
    head.number_of_samples = ns;
    this->makeConsistent();
}

uint16_t Acquisition::getAvailableChannels() const {
    return head.available_channels;
}

void Acquisition::setAvailableChannels(uint16_t ac) {
    head.available_channels = ac;
    // TODO: compare against head.active_channels or makeConsistent?
}

uint16_t Acquisition::getActiveChannels() const {
    return head.active_channels;
}

void Acquisition::setActiveChannels(uint16_t ac) {
    head.active_channels = ac;
    this->makeConsistent();
}

uint64_t Acquisition::getChannelMask(int idx) {
    // TODO: bounds checking
    return head.channel_mask[idx];
}

void Acquisition::setChannelMask(int idx, uint64_t mask) {
    // TODO: bounds checking
    head.channel_mask[idx] = mask;
}

uint16_t Acquisition::getDiscardPre() const {
    return head.discard_pre;
}

void Acquisition::setDiscardPre(uint16_t dp) {
    head.discard_pre = dp;
}

uint16_t Acquisition::getDiscardPost() const {
    return head.discard_post;
}

void Acquisition::setDiscardPost(uint16_t dp) {
    head.discard_post = dp;
}

uint16_t Acquisition::getCenterSample() const {
    return head.center_sample;
}

void Acquisition::setCenterSample(uint16_t cs) {
    head.center_sample = cs;
}

uint16_t Acquisition::getEncodingSpaceRef() const {
    return head.encoding_space_ref;
}

void Acquisition::setEncodingSpaceRef(uint16_t esr) {
    head.encoding_space_ref = esr;
}

uint16_t Acquisition::getTrajectoryDimensions() const {
    return head.trajectory_dimensions;
}

void Acquisition::setTrajectoryDimensions(uint16_t td) {
    head.trajectory_dimensions = td;
}

float Acquisition::getSampleTime_us() const {
    return head.sample_time_us;
}

void Acquisition::setSampleTime_us(float time){
    head.sample_time_us = time;
}

float Acquisition::getPositionX() const {
    return head.position[0];
}

float Acquisition::getPositionY() const {
    return head.position[1];
}

float Acquisition::getPositionZ() const {
    return head.position[2];
}

void Acquisition::setPosition(float x, float y, float z) {
    setPositionX(x);
    setPositionY(y);
    setPositionZ(z);
}

void Acquisition::setPositionX(float x) {
    head.position[0] = x;
}

void Acquisition::setPositionY(float y) {
    head.position[1] = y;
}

void Acquisition::setPositionZ(float z) {
    head.position[2] = z;
}

float Acquisition::getReadDirectionX() const {
    return head.read_dir[0];
}

float Acquisition::getReadDirectionY() const {
    return head.read_dir[1];
}

float Acquisition::getReadDirectionZ() const {
    return head.read_dir[2];
}

void Acquisition::setReadDirection(float x, float y, float z) {
    setReadDirectionX(x);
    setReadDirectionY(y);
    setReadDirectionZ(z);
}

void Acquisition::setReadDirectionX(float x) {
    head.read_dir[0] = x;
}

void Acquisition::setReadDirectionY(float y) {
    head.read_dir[1] = y;
}

void Acquisition::setReadDirectionZ(float z) {
    head.read_dir[2] = z;
}

float Acquisition::getPhaseDirectionX() const {
    return head.phase_dir[0];
}

float Acquisition::getPhaseDirectionY() const {
    return head.phase_dir[1];
}

float Acquisition::getPhaseDirectionZ() const {
    return head.phase_dir[2];
}

void Acquisition::setPhaseDirection(float x, float y, float z) {
    setPhaseDirectionX(x);
    setPhaseDirectionY(y);
    setPhaseDirectionZ(z);
}

void Acquisition::setPhaseDirectionX(float x) {
    head.phase_dir[0] = x;
}

void Acquisition::setPhaseDirectionY(float y) {
    head.phase_dir[1] = y;
}

void Acquisition::setPhaseDirectionZ(float z) {
    head.phase_dir[2] = z;
}

float Acquisition::getSliceDirectionX() const {
    return head.slice_dir[0];
}

float Acquisition::getSliceDirectionY() const {
    return head.slice_dir[1];
}

float Acquisition::getSliceDirectionZ() const {
    return head.slice_dir[2];
}

void Acquisition::setSliceDirection(float x, float y, float z) {
    setSliceDirectionX(x);
    setSliceDirectionY(y);
    setSliceDirectionZ(z);
}

void Acquisition::setSliceDirectionX(float x) {
    head.slice_dir[0] = x;
}

void Acquisition::setSliceDirectionY(float y) {
    head.slice_dir[1] = y;
}

void Acquisition::setSliceDirectionZ(float z) {
    head.slice_dir[2] = z;
}

float Acquisition::getPatientTablePositionX() const {
    return head.patient_table_position[0];
}

float Acquisition::getPatientTablePositionY() const {
    return head.patient_table_position[1];
}

float Acquisition::getPatientTablePositionZ() const {
    return head.patient_table_position[2];
}

void Acquisition::setPatientTablePosition(float x, float y, float z) {
    setPatientTablePositionX(x);
    setPatientTablePositionY(y);
    setPatientTablePositionZ(z);
}

void Acquisition::setPatientTablePositionX(float x) {
    head.patient_table_position[0] = x;
}

void Acquisition::setPatientTablePositionY(float y) {
    head.patient_table_position[1] = y;
}

void Acquisition::setPatientTablePositionZ(float z) {
    head.patient_table_position[2] = z;
}

EncodingCounters& Acquisition::getEncodingCounters() {
    return head.idx;
}

const EncodingCounters& Acquisition::getEncodingCounters() const {
    return head.idx;
}

void Acquisition::setEncodingCounters(const EncodingCounters& idx) {
    head.idx = idx;
}

int32_t Acquisition::getUserInt(int idx) const {
    // TODO: bounds checking
    return head.user_int[idx];
}

void Acquisition::setUserInt(int idx, int32_t val) {
    // TODO: bounds checking
    head.user_int[idx] = val;
}

float Acquisition::getUserFloat(int idx) const {
    // TODO: bounds checking
    return head.user_float[idx];
}

void Acquisition::setUserFloat(int idx, float val) {
    // TODO: bounds checking
    head.user_float[idx] = val;
}

size_t Acquisition::getNumberOfDataElements() const {
    return head.number_of_samples * head.active_channels;
}

size_t Acquisition::getDataSize() const {
    return head.number_of_samples * head.active_channels * sizeof(std::complex<float>);
}

size_t Acquisition::getNumberOfTrajElements() const {
    return head.number_of_samples * head.trajectory_dimensions;
}

size_t Acquisition::getTrajSize() const {
    return head.number_of_samples * head.trajectory_dimensions * sizeof(float);
}

AcquisitionHeader& Acquisition::getHead() {
    return head;
}

const AcquisitionHeader & Acquisition::getHead() const {
    return head;
}

void Acquisition::setHead(const AcquisitionHeader &other) {
    this->head = other;
    this->makeConsistent();
}

void Acquisition::resize(uint16_t num_samples, uint16_t active_channels, uint16_t trajectory_dimensions){
    head.number_of_samples = num_samples;
    head.active_channels = active_channels;
    head.trajectory_dimensions = trajectory_dimensions;
    this->makeConsistent();
}

void Acquisition::makeConsistent() {
    if (head.available_channels < head.active_channels) {
        head.available_channels = head.active_channels;
    }
    size_t traj_nelem = head.number_of_samples * head.trajectory_dimensions;
    traj.resize(traj_nelem);
    size_t data_nelem = head.number_of_samples * head.active_channels;
    data.resize(data_nelem);
}

std::vector<std::complex<float> >& Acquisition::getData() {
    return data;
}

const std::vector<std::complex<float> >& Acquisition::getData() const {
    return data;
}

void Acquisition::setData(const std::vector<std::complex<float> >& data) {
    // TODO: bounds checking
    this->data = data;
}

std::complex<float>& Acquisition::at(uint16_t sample, uint16_t channel){
    // TODO: bounds checking
    size_t index = size_t(sample) + size_t(channel) * size_t(head.number_of_samples);
    return data[index];
}

const std::vector<float>& Acquisition::getTraj() const {
    return traj;
}

void Acquisition::setTraj(const std::vector<float>& traj) {
    this->traj = traj;
}

float& Acquisition::trajAt(uint16_t dimension, uint16_t sample){
    size_t index = size_t(sample) * size_t(head.trajectory_dimensions) + size_t(dimension);
    return traj[index];
}

uint64_t Acquisition::getFlags() const {
    return head.flags;
}

void Acquisition::setFlags(uint64_t val) {
    head.flags = val;
}

bool Acquisition::isFlagSet(uint64_t val) const {
    uint64_t bitmask = 1 << (val - 1);
    return (head.flags & bitmask) > 0;
}

void Acquisition::setFlag(uint64_t val) {
    uint64_t bitmask = 1 << (val - 1);
    head.flags |= bitmask;
}

void Acquisition::clearFlag(uint64_t val) {
    uint64_t bitmask = 1 << (val - 1);
    head.flags &= ~bitmask;
}

void Acquisition::clearAllFlags() {
    head.flags = 0;
}

bool Acquisition::isChannelActive(uint16_t chan) const {
    uint64_t bitmask = 1 << (chan % 64);
    size_t offset = chan / 64;
    return (head.channel_mask[offset] & bitmask) > 0;
}

void Acquisition::setChannelActive(uint16_t chan) {
    uint64_t bitmask = 1 << (chan % 64);
    size_t offset = chan / 64;
    head.channel_mask[offset] |= bitmask;
}

void Acquisition::setChannelNotActive(uint16_t chan) {
    uint64_t bitmask = 1 << (chan % 64);
    size_t offset = chan / 64;
    head.channel_mask[offset] &= ~bitmask;
}

void Acquisition::setAllChannelsNotActive() {
    for (size_t offset = 0; offset<ISMRMRD_CHANNEL_MASKS; offset++) {
        head.channel_mask[offset] = 0;
    }
}


template <typename T> Image<T>::Image(uint16_t matrix_size_x,
                                      uint16_t matrix_size_y,
                                      uint16_t matrix_size_z,
                                      uint16_t channels)
{
    memset(&head, 0, sizeof(head));
    head.version = ISMRMRD_VERSION_MAJOR;
    head.storage_type = static_cast<uint16_t>(get_storage_type<T>());
    this->resize(matrix_size_x, matrix_size_y, matrix_size_z, channels);
}

// Image dimensions
template <typename T> void Image<T>::resize(uint16_t matrix_size_x,
                                            uint16_t matrix_size_y,
                                            uint16_t matrix_size_z,
                                            uint16_t channels)
{
    head.matrix_size[0] = matrix_size_x;
    head.matrix_size[1] = matrix_size_y;
    head.matrix_size[2] = matrix_size_z;
    head.channels = channels;
    this->makeConsistent();
}

template <typename T> void Image<T>::makeConsistent() {
    // TODO what if matrix_size[0] = 0?

    if (head.matrix_size[1] == 0) {
        head.matrix_size[1] = 1;
    }
    if (head.matrix_size[2] == 0) {
        head.matrix_size[2] = 1;
    }
    if (head.channels == 0) {
        head.channels = 1;
    }
    data.resize(getNumberOfDataElements());

    attribute_string.resize(head.attribute_string_len);
}

template <typename T> uint16_t Image<T>::getMatrixSizeX() const
{
    return head.matrix_size[0];
}

template <typename T> uint16_t Image<T>::getMatrixSizeY() const
{
    return head.matrix_size[1];
}

template <typename T> uint16_t Image<T>::getMatrixSizeZ() const
{
    return head.matrix_size[2];
}

template <typename T> void Image<T>::setMatrixSizeX(uint16_t x)
{
    head.matrix_size[0] = x;
    this->makeConsistent();
}

template <typename T> void Image<T>::setMatrixSizeY(uint16_t y)
{
    head.matrix_size[1] = y;
    this->makeConsistent();
}

template <typename T> void Image<T>::setMatrixSizeZ(uint16_t z)
{
    head.matrix_size[2] = z;
    this->makeConsistent();
}

template <typename T> uint16_t Image<T>::getNumberOfChannels() const
{
    return head.channels;
}

template <typename T> void Image<T>::setNumberOfChannels(uint16_t channels)
{
    head.channels = channels;
    this->makeConsistent();
}


template <typename T> float Image<T>::getFieldOfViewX() const
{
    return head.field_of_view[0];
}

template <typename T> float Image<T>::getFieldOfViewY() const
{
    return head.field_of_view[1];
}

template <typename T> float Image<T>::getFieldOfViewZ() const
{
    return head.field_of_view[2];
}

template <typename T> void Image<T>::setFieldOfView(float fov_x, float fov_y, float fov_z)
{
    setFieldOfViewX(fov_x);
    setFieldOfViewY(fov_y);
    setFieldOfViewZ(fov_z);
}

template <typename T> void Image<T>::setFieldOfViewX(float fov_x)
{
    head.field_of_view[0] = fov_x;
}

template <typename T> void Image<T>::setFieldOfViewY(float fov_y)
{
    head.field_of_view[1] = fov_y;
}

template <typename T> void Image<T>::setFieldOfViewZ(float fov_z)
{
    head.field_of_view[2] = fov_z;
}

// Positions and orientations
template <typename T> float Image<T>::getPositionX() const
{
    return head.position[0];
}

template <typename T> float Image<T>::getPositionY() const
{
    return head.position[1];
}

template <typename T> float Image<T>::getPositionZ() const
{
    return head.position[2];
}

template <typename T> void Image<T>::setPosition(float x, float y, float z)
{
    setPositionX(x);
    setPositionY(y);
    setPositionZ(z);
}

template <typename T> void Image<T>::setPositionX(float x)
{
    head.position[0] = x;
}

template <typename T> void Image<T>::setPositionY(float y)
{
    head.position[1] = y;
}

template <typename T> void Image<T>::setPositionZ(float z)
{
    head.position[2] = z;
}


template <typename T> float Image<T>::getReadDirectionX() const
{
    return head.read_dir[0];
}

template <typename T> float Image<T>::getReadDirectionY() const
{
    return head.read_dir[1];
}

template <typename T> float Image<T>::getReadDirectionZ() const
{
    return head.read_dir[2];
}

template <typename T> void Image<T>::setReadDirection(float x, float y, float z)
{
    setReadDirectionX(x);
    setReadDirectionY(y);
    setReadDirectionZ(z);
}

template <typename T> void Image<T>::setReadDirectionX(float x)
{
    head.read_dir[0] = x;
}

template <typename T> void Image<T>::setReadDirectionY(float y)
{
    head.read_dir[1] = y;
}

template <typename T> void Image<T>::setReadDirectionZ(float z)
{
    head.read_dir[2] = z;
}


template <typename T> float Image<T>::getPhaseDirectionX() const
{
    return head.phase_dir[0];
}

template <typename T> float Image<T>::getPhaseDirectionY() const
{
    return head.phase_dir[1];
}

template <typename T> float Image<T>::getPhaseDirectionZ() const
{
    return head.phase_dir[2];
}

template <typename T> void Image<T>::setPhaseDirection(float x, float y, float z)
{
    setPhaseDirectionX(x);
    setPhaseDirectionY(y);
    setPhaseDirectionZ(z);
}

template <typename T> void Image<T>::setPhaseDirectionX(float x)
{
    head.phase_dir[0] = x;
}

template <typename T> void Image<T>::setPhaseDirectionY(float y)
{
    head.phase_dir[1] = y;
}

template <typename T> void Image<T>::setPhaseDirectionZ(float z)
{
    head.phase_dir[2] = z;
}

template <typename T> float Image<T>::getSliceDirectionX() const
{
    return head.slice_dir[0];
}

template <typename T> float Image<T>::getSliceDirectionY() const
{
    return head.slice_dir[1];
}

template <typename T> float Image<T>::getSliceDirectionZ() const
{
    return head.slice_dir[2];
}

template <typename T> void Image<T>::setSliceDirection(float x, float y, float z)
{
    setSliceDirectionX(x);
    setSliceDirectionY(y);
    setSliceDirectionZ(z);
}

template <typename T> void Image<T>::setSliceDirectionX(float x)
{
    head.slice_dir[0] = x;
}

template <typename T> void Image<T>::setSliceDirectionY(float y)
{
    head.slice_dir[1] = y;
}

template <typename T> void Image<T>::setSliceDirectionZ(float z)
{
    head.slice_dir[2] = z;
}

template <typename T> float Image<T>::getPatientTablePositionX() const
{
    return head.patient_table_position[0];
}

template <typename T> float Image<T>::getPatientTablePositionY() const
{
    return head.patient_table_position[1];
}

template <typename T> float Image<T>::getPatientTablePositionZ() const
{
    return head.patient_table_position[2];
}

template <typename T> void Image<T>::setPatientTablePosition(float x, float y, float z)
{
    setPatientTablePositionX(x);
    setPatientTablePositionY(y);
    setPatientTablePositionZ(z);
}

template <typename T> void Image<T>::setPatientTablePositionX(float x)
{
    head.patient_table_position[0] = x;
}

template <typename T> void Image<T>::setPatientTablePositionY(float y)
{
    head.patient_table_position[1] = y;
}

template <typename T> void Image<T>::setPatientTablePositionZ(float z)
{
    head.patient_table_position[2] = z;
}

template <typename T> uint16_t Image<T>::getVersion() const
{
    return head.version;
}

template <typename T> StorageType Image<T>::getStorageType() const
{
    return static_cast<StorageType>(head.storage_type);
}

template <typename T> uint32_t Image<T>::getMeasurementUID() const
{
    return head.measurement_uid;
}

template <typename T> void Image<T>::setMeasurementUID(uint32_t measurement_uid)
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

template <typename T> uint32_t Image<T>::getPhysiologyTimeStamp(unsigned int idx) const
{
    // TODO: bounds checking
    return head.physiology_time_stamp[idx];
}

template <typename T> void  Image<T>::setPhysiologyTimeStamp(unsigned int idx, uint32_t value)
{
    // TODO: bounds checking
    head.physiology_time_stamp[idx] = value;
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

template <typename T> int32_t Image<T>::getUserInt(unsigned int index) const
{
    // TODO: bounds checking
    return head.user_int[index];
}

template <typename T> void Image<T>::setUserInt(unsigned int index, int32_t value)
{
    // TODO: bounds checking
    head.user_int[index] = value;
}

template <typename T> float Image<T>::getUserFloat(unsigned int index) const
{
    // TODO: bounds checking
    return head.user_float[index];
}

template <typename T> void Image<T>::setUserFloat(unsigned int index, float value)
{
    // TODO: bounds checking
    head.user_float[index] = value;
}

// Flag methods
template <typename T> uint64_t Image<T>::getFlags() const {
    return head.flags;
}

template <typename T> void Image<T>::setFlags(uint64_t val) {
    head.flags = val;
}

template <typename T> bool Image<T>::isFlagSet(uint64_t val) const {
    uint64_t bitmask = 1 << (val - 1);
    return (head.flags & bitmask) > 0;
}

template <typename T> void Image<T>::setFlag(uint64_t val) {
    uint64_t bitmask = 1 << (val - 1);
    head.flags |= bitmask;
}

template <typename T> void Image<T>::clearFlag(uint64_t val) {
    uint64_t bitmask = 1 << (val - 1);
    head.flags &= ~bitmask;
}

template <typename T> void Image<T>::clearAllFlags() {
    head.flags = 0;
}

template <typename T> ImageHeader &Image<T>::getHead() {
    return head;
}

template <typename T> const ImageHeader &Image<T>::getHead() const {
    return head;
}

template <typename T> void Image<T>::setHead(const ImageHeader& other) {
    if (other.storage_type != head.storage_type) {
        throw std::runtime_error("Cannot assign a header of a different data type.");
    }

    this->head = other;
    this->makeConsistent();
}

// Attribute string
template <typename T> void Image<T>::getAttributeString(std::string& attr) const
{
    attr.assign(attribute_string);
}

template <typename T> const std::string& Image<T>::getAttributeString() const
{
   return attribute_string;
}

template <typename T> void Image<T>::setAttributeString(const std::string& attr)
{
    attribute_string = attr;
    head.attribute_string_len = attr.size();
}

template <typename T> size_t Image<T>::getAttributeStringLength() const
{
    return head.attribute_string_len;
}

template <typename T> std::vector<T>& Image<T>::getData() {
    return data;
}

template <typename T> const std::vector<T>& Image<T>::getData() const {
     return data;
}

template <typename T> size_t Image<T>::getNumberOfDataElements() const {
    return head.matrix_size[0] * head.matrix_size[1] *
            head.matrix_size[2] * head.channels;
}

template <typename T> size_t Image<T>::getDataSize() const {
    return getNumberOfDataElements() * sizeof(T);
}

template <typename T> T& Image<T>::at(uint16_t ix, uint16_t iy, uint16_t iz, uint16_t channel) {
    // TODO: bounds checking
    size_t sx = getMatrixSizeX();
    size_t sy = getMatrixSizeY();
    size_t sz = getMatrixSizeZ();
    size_t index = ix + (sx * iy) + (sy * sx * iz) + (sy * sx * sz * channel);
    return data[index];
}

//
// Array class Implementation
//
template <typename T> NDArray<T>::NDArray()
    : version(ISMRMRD_VERSION_MAJOR)
{ }

template <typename T> NDArray<T>::NDArray(const std::vector<size_t>& dims)
    : version(ISMRMRD_VERSION_MAJOR)
{
    resize(dims);
}

template <typename T> uint16_t NDArray<T>::getVersion() const {
    return version;
};

template <typename T> StorageType NDArray<T>::getStorageType() const {
    return static_cast<StorageType>(get_storage_type<T>());
}

template <typename T> uint16_t NDArray<T>::getNDim() const {
    return dims.size();
};

template <typename T> const std::vector<size_t>& NDArray<T>::getDims() {
    return dims;
};

template <typename T> void NDArray<T>::resize(const std::vector<size_t>& dims) {
    this->dims = dims;
    this->makeConsistent();
}

template <typename T> void NDArray<T>::makeConsistent() {
    data.resize(getNumberOfElements());
}

template <typename T> std::vector<T>& NDArray<T>::getData() {
    return data;
}

template <typename T> const std::vector<T>& NDArray<T>::getData() const {
    return data;
}

template <typename T> size_t NDArray<T>::getNumberOfElements() const {
    size_t num = 1;
    for (std::vector<size_t>::const_iterator it = dims.begin(); it != dims.end(); ++it) {
        // This is necessary to prevent bad GCC loop optimization!
        if (*it > 0) {
            num *= *it;
        }
    }
    return num;
}

template <typename T> size_t NDArray<T>::getDataSize() const {
    return getNumberOfElements() * sizeof(T);
}

template <typename T> T& NDArray<T>::at(uint16_t x, uint16_t y, uint16_t z,
        uint16_t w, uint16_t n, uint16_t m, uint16_t l)
{
    // TODO: bounds checking
    size_t index = 0;
    uint16_t indices[ISMRMRD_NDARRAY_MAXDIM] = {x,y,z,w,n,m,l};
    size_t stride = 1;
    for (uint16_t i = 0; i < dims.size(); i++){
        index += indices[i] * stride;
        stride *= dims[i];
    }

    return data[index];
}

// Specializations
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
