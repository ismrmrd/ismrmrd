//#include <stdlib.h>
#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/version.h"

#include <sstream>
#include <stdexcept>
#include <iostream>
#include <algorithm>

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
template <> EXPORTISMRMRD StorageType get_storage_type<char>()
{
    return ISMRMRD_CHAR;
}

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

template <> EXPORTISMRMRD StorageType get_storage_type<uint64_t>()
{
    return ISMRMRD_ULONG;
}

template <> EXPORTISMRMRD StorageType get_storage_type<int64_t>()
{
    return ISMRMRD_LONG;
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


template <typename T>
Acquisition<T>::Acquisition
(
  uint32_t num_samples,
  uint32_t active_channels,
  uint32_t trajectory_dimensions
)
{
  memset (&head_, 0, sizeof(head_));
  head_.ent_head.signature    = ISMRMRD_SIGNATURE;
  head_.ent_head.entity_type  = ISMRMRD_MRACQUISITION;
  head_.ent_head.storage_type = get_storage_type<T>();
  this->resize (num_samples, active_channels, trajectory_dimensions);
}

// Accessors and mutators
template <typename T>
void Acquisition<T>::setStream (uint32_t stream_number)
{
  head_.ent_head.stream = stream_number;
}

template <typename T> uint32_t Acquisition<T>::getScanCounter() const {
    return head_.scan_counter;
}

template <typename T> void Acquisition<T>::setScanCounter(uint32_t counter) {
    head_.scan_counter = counter;
}

template <typename T> uint64_t Acquisition<T>::getTimeStamp() const {
    return head_.time_stamp;
}

template <typename T> void Acquisition<T>::setTimeStamp(uint64_t ts) {
    head_.time_stamp = ts;
}

template <typename T>
uint32_t Acquisition<T>::getPhysiologyTimeStamp (int idx)
{
  if (idx < 0 || idx >= ISMRMRD_PHYS_STAMPS)
  {
    throw std::runtime_error ("Physiology Time Stamp index out of bounds");
  }

  return head_.physiology_time_stamp[idx];
}

template <typename T>
void Acquisition<T>::setPhysiologyTimeStamp (int idx, uint32_t ts)
{
  if (idx < 0 || idx >= ISMRMRD_PHYS_STAMPS)
  {
    throw std::runtime_error ("Physiology Time Stamp index out of bounds");
  }

  head_.physiology_time_stamp[idx] = ts;
}

template <typename T>
uint32_t Acquisition<T>::getNumberOfSamples() const
{
  return head_.number_of_samples;
}

template <typename T>
void Acquisition<T>::setNumberOfSamples (uint32_t ns)
{
  head_.number_of_samples = ns;
  this->makeConsistent();
}

template <typename T> uint32_t Acquisition<T>::getAvailableChannels() const {
    return head_.available_channels;
}

template <typename T> void Acquisition<T>::setAvailableChannels(uint32_t ac) {
    // TODO: compare against head.active_channels or makeConsistent?
    head_.available_channels = ac;
}

template <typename T> uint32_t Acquisition<T>::getActiveChannels() const {
    return head_.active_channels;
}

template <typename T> void Acquisition<T>::setActiveChannels(uint32_t ac) {
    head_.active_channels = ac;
    this->makeConsistent();
}

template <typename T>
uint64_t Acquisition<T>::getChannelMask (int idx)
{
  if (idx < 0 || idx >= ISMRMRD_CHANNEL_MASKS)
  {
    throw std::runtime_error ("Channel Mask index out of bounds");
  }

  return head_.channel_mask[idx];
}

template <typename T>
void Acquisition<T>::setChannelMask (int idx, uint64_t mask)
{
  if (idx < 0 || idx >= ISMRMRD_CHANNEL_MASKS)
  {
    throw std::runtime_error ("Channel Mask index out of bounds");
  }

    head_.channel_mask[idx] = mask;
}

template <typename T> uint32_t Acquisition<T>::getDiscardPre() const {
    return head_.discard_pre;
}

template <typename T> void Acquisition<T>::setDiscardPre(uint32_t dp) {
    head_.discard_pre = dp;
}

template <typename T> uint32_t Acquisition<T>::getDiscardPost() const {
    return head_.discard_post;
}

template <typename T> void Acquisition<T>::setDiscardPost(uint32_t dp) {
    head_.discard_post = dp;
}

template <typename T> uint32_t Acquisition<T>::getCenterSample() const {
    return head_.center_sample;
}

template <typename T> void Acquisition<T>::setCenterSample(uint32_t cs) {
    head_.center_sample = cs;
}

template <typename T> uint32_t Acquisition<T>::getEncodingSpaceRef() const {
    return head_.encoding_space_ref;
}

template <typename T> void Acquisition<T>::setEncodingSpaceRef(uint32_t esr) {
    head_.encoding_space_ref = esr;
}

template <typename T> uint32_t Acquisition<T>::getTrajectoryDimensions() const {
    return head_.trajectory_dimensions;
}

template <typename T> void Acquisition<T>::setTrajectoryDimensions(uint32_t td) {
    head_.trajectory_dimensions = td;
    this->makeConsistent();
}

template <typename T> uint32_t Acquisition<T>::getDwellTime_ns() const {
    return head_.dwell_time_ns;
}

template <typename T> void Acquisition<T>::setDwellTime_ns(uint32_t time){
    head_.dwell_time_ns = time;
}

template <typename T> float Acquisition<T>::getPositionX() const {
    return head_.position[0];
}

template <typename T> float Acquisition<T>::getPositionY() const {
    return head_.position[1];
}

template <typename T> float Acquisition<T>::getPositionZ() const {
    return head_.position[2];
}

template <typename T> void Acquisition<T>::setPosition(float x, float y, float z) {
    setPositionX(x);
    setPositionY(y);
    setPositionZ(z);
}

template <typename T> void Acquisition<T>::setPositionX(float x) {
    head_.position[0] = x;
}

template <typename T> void Acquisition<T>::setPositionY(float y) {
    head_.position[1] = y;
}

template <typename T> void Acquisition<T>::setPositionZ(float z) {
    head_.position[2] = z;
}

template <typename T> float Acquisition<T>::getReadDirectionX() const {
    return head_.read_dir[0];
}

template <typename T> float Acquisition<T>::getReadDirectionY() const {
    return head_.read_dir[1];
}

template <typename T> float Acquisition<T>::getReadDirectionZ() const {
    return head_.read_dir[2];
}

template <typename T> void Acquisition<T>::setReadDirection(float x, float y, float z) {
    setReadDirectionX(x);
    setReadDirectionY(y);
    setReadDirectionZ(z);
}

template <typename T> void Acquisition<T>::setReadDirectionX(float x) {
    head_.read_dir[0] = x;
}

template <typename T> void Acquisition<T>::setReadDirectionY(float y) {
    head_.read_dir[1] = y;
}

template <typename T> void Acquisition<T>::setReadDirectionZ(float z) {
    head_.read_dir[2] = z;
}

template <typename T> float Acquisition<T>::getPhaseDirectionX() const {
    return head_.phase_dir[0];
}

template <typename T> float Acquisition<T>::getPhaseDirectionY() const {
    return head_.phase_dir[1];
}

template <typename T> float Acquisition<T>::getPhaseDirectionZ() const {
    return head_.phase_dir[2];
}

template <typename T> void Acquisition<T>::setPhaseDirection(float x, float y, float z) {
    setPhaseDirectionX(x);
    setPhaseDirectionY(y);
    setPhaseDirectionZ(z);
}

template <typename T> void Acquisition<T>::setPhaseDirectionX(float x) {
    head_.phase_dir[0] = x;
}

template <typename T> void Acquisition<T>::setPhaseDirectionY(float y) {
    head_.phase_dir[1] = y;
}

template <typename T> void Acquisition<T>::setPhaseDirectionZ(float z) {
    head_.phase_dir[2] = z;
}

template <typename T> float Acquisition<T>::getSliceDirectionX() const {
    return head_.slice_dir[0];
}

template <typename T> float Acquisition<T>::getSliceDirectionY() const {
    return head_.slice_dir[1];
}

template <typename T> float Acquisition<T>::getSliceDirectionZ() const {
    return head_.slice_dir[2];
}

template <typename T> void Acquisition<T>::setSliceDirection(float x, float y, float z) {
    setSliceDirectionX(x);
    setSliceDirectionY(y);
    setSliceDirectionZ(z);
}

template <typename T> void Acquisition<T>::setSliceDirectionX(float x) {
    head_.slice_dir[0] = x;
}

template <typename T> void Acquisition<T>::setSliceDirectionY(float y) {
    head_.slice_dir[1] = y;
}

template <typename T> void Acquisition<T>::setSliceDirectionZ(float z) {
    head_.slice_dir[2] = z;
}

template <typename T> float Acquisition<T>::getPatientTablePositionX() const {
    return head_.patient_table_position[0];
}

template <typename T> float Acquisition<T>::getPatientTablePositionY() const {
    return head_.patient_table_position[1];
}

template <typename T> float Acquisition<T>::getPatientTablePositionZ() const {
    return head_.patient_table_position[2];
}

template <typename T> void Acquisition<T>::setPatientTablePosition(float x, float y, float z) {
    setPatientTablePositionX(x);
    setPatientTablePositionY(y);
    setPatientTablePositionZ(z);
}

template <typename T> void Acquisition<T>::setPatientTablePositionX(float x) {
    head_.patient_table_position[0] = x;
}

template <typename T> void Acquisition<T>::setPatientTablePositionY(float y) {
    head_.patient_table_position[1] = y;
}

template <typename T> void Acquisition<T>::setPatientTablePositionZ(float z) {
    head_.patient_table_position[2] = z;
}

template <typename T>
EncodingCounters& Acquisition<T>::getEncodingCounters()
{
  return head_.idx;
}

template <typename T>
const EncodingCounters& Acquisition<T>::getEncodingCounters() const
{
  return head_.idx;
}

template <typename T>
void Acquisition<T>::setEncodingCounters (const EncodingCounters& idx)
{
  head_.idx = idx;
}

template <typename T>
int32_t Acquisition<T>::getUserInt (int idx) const
{
  if (idx < 0 || idx >= ISMRMRD_USER_INTS)
  {
    throw std::runtime_error ("User int index out of bounds");
  }
  return head_.user_int[idx];
}

template <typename T>
void Acquisition<T>::setUserInt (int idx, int32_t val)
{
  if (idx < 0 || idx >= ISMRMRD_USER_INTS)
  {
    throw std::runtime_error ("User int index out of bounds");
  }
  head_.user_int[idx] = val;
}

template <typename T>
float Acquisition<T>::getUserFloat (int idx) const
{
  if (idx < 0 || idx >= ISMRMRD_USER_FLOATS)
  {
    throw std::runtime_error ("User float index out of bounds");
  }
  return head_.user_float[idx];
}

template <typename T>
void Acquisition<T>::setUserFloat (int idx, float val)
{
  if (idx < 0 || idx >= ISMRMRD_USER_FLOATS)
  {
    throw std::runtime_error ("User float index out of bounds");
  }
  head_.user_float[idx] = val;
}

template <typename T> size_t Acquisition<T>::getNumberOfDataElements() const {
    return head_.number_of_samples * head_.active_channels;
}

template <typename T> size_t Acquisition<T>::getNumberOfTrajElements() const {
    return head_.number_of_samples * head_.trajectory_dimensions;
}

template <typename T> AcquisitionHeader& Acquisition<T>::getHead() {
    return head_;
}

template <typename T> const AcquisitionHeader & Acquisition<T>::getHead() const {
    return head_;
}

template <typename T> void Acquisition<T>::setHead(const AcquisitionHeader &other) {
    this->head_ = other;
    this->makeConsistent();
}

template <typename T>
void Acquisition<T>::resize
(
  uint32_t num_samples,
  uint32_t active_channels,
  uint32_t trajectory_dimensions
)
{
  head_.number_of_samples     = num_samples;
  head_.active_channels       = active_channels;
  head_.trajectory_dimensions = trajectory_dimensions;
  this->makeConsistent();
}

template <typename T>
void Acquisition<T>::makeConsistent()
{
  if (head_.available_channels < head_.active_channels)
  {
    head_.available_channels = head_.active_channels;
  }
  traj_.resize (head_.number_of_samples * head_.trajectory_dimensions);
  data_.resize (head_.number_of_samples * head_.active_channels);
}

template <typename T> std::vector<std::complex<T> >& Acquisition<T>::getData() {
    return data_;
}

template <typename T> const std::vector<std::complex<T> >& Acquisition<T>::getData() const {
    return data_;
}

template <typename T> void Acquisition<T>::setData(const std::vector<std::complex<T> >& data) {
    if (data.size() != getNumberOfDataElements()) {
        throw std::runtime_error("data size does not match size specified by header");
    }
    this->data_ = data;
}

template <typename T>
std::complex<T>& Acquisition<T>::at (uint32_t sample, uint32_t channel)
{
  if (sample >= getNumberOfSamples())
  {
    throw std::runtime_error("sample greater than number of samples");
  }

  if (channel >= getActiveChannels())
  {
    throw std::runtime_error("channel greater than number of active channels");
  }

  return data_[sample + channel * getNumberOfSamples()];
}

template <typename T> const std::vector<float>& Acquisition<T>::getTraj() const {
    return traj_;
}

template <typename T> void Acquisition<T>::setTraj(const std::vector<float>& traj) {
    this->traj_ = traj;
}

template <typename T>
float& Acquisition<T>::trajAt (uint32_t dimension, uint32_t sample)
{
  if (sample >= getNumberOfSamples())
  {
    throw std::runtime_error("sample greater than number of samples");
  }
  if (dimension >= getTrajectoryDimensions())
  {
    throw std::runtime_error("trajectory greater than trajectory_dimensions");
  }
  return traj_[sample * head_.trajectory_dimensions + dimension];
}

template <typename T> uint64_t Acquisition<T>::getFlags() const {
    return head_.flags;
}

template <typename T> void Acquisition<T>::setFlags(uint64_t val) {
    head_.flags = val;
}

template <typename T> bool Acquisition<T>::isFlagSet(uint64_t val) const {
    uint64_t bitmask = 1UL << (val - 1UL);
    return (head_.flags & bitmask) > 0;
}

template <typename T> void Acquisition<T>::setFlag(uint64_t val) {
    uint64_t bitmask = 1UL << (val - 1UL);
    head_.flags |= bitmask;
}

template <typename T> void Acquisition<T>::clearFlag(uint64_t val) {
    uint64_t bitmask = 1UL << (val - 1UL);
    head_.flags &= ~bitmask;
}

template <typename T> void Acquisition<T>::clearAllFlags() {
    head_.flags = 0;
}

template <typename T> bool Acquisition<T>::isChannelActive(uint32_t chan) const {
    uint64_t bitmask = 1UL << (chan % 64UL);
    size_t offset = chan / 64UL;
    return (head_.channel_mask[offset] & bitmask) > 0;
}

template <typename T> void Acquisition<T>::setChannelActive(uint32_t chan) {
    uint64_t bitmask = 1UL << (chan % 64UL);
    size_t offset = chan / 64UL;
    head_.channel_mask[offset] |= bitmask;
}

template <typename T> void Acquisition<T>::setChannelNotActive(uint32_t chan) {
    uint64_t bitmask = 1UL << (chan % 64UL);
    size_t offset = chan / 64UL;
    head_.channel_mask[offset] &= ~bitmask;
}

template <typename T> void Acquisition<T>::setAllChannelsNotActive() {
    for (size_t offset = 0; offset<ISMRMRD_CHANNEL_MASKS; offset++) {
        head_.channel_mask[offset] = 0;
    }
}


template <typename T>
uint32_t Acquisition<T>::getStream () const
{
  return head_.ent_head.stream;
}

template <typename T>
uint32_t Acquisition<T>::getSignature () const
{
  return head_.ent_head.signature;
}

template <typename T>
EntityType Acquisition<T>::getEntityType () const
{
  return static_cast<EntityType>(head_.ent_head.entity_type);
}

template <typename T>
StorageType Acquisition<T>::getStorageType () const
{
  return static_cast<StorageType>(head_.ent_head.storage_type);
}

template <typename T>
std::vector<unsigned char> Acquisition<T>::serialize()
{
  if (this->head_.ent_head.signature != ISMRMRD_SIGNATURE)
  {
    throw std::runtime_error ("The header signature not recognized");
  }

  if (this->head_.ent_head.entity_type != ISMRMRD_MRACQUISITION)
  {
    throw std::runtime_error
      ("The header entity type does not match the class type");
  }

  if (this->head_.ent_head.storage_type != get_storage_type<T>()) {
    throw std::runtime_error("Header storage type does not match class");
  }

  size_t bytes = sizeof(AcquisitionHeader) +
                 traj_.size() * sizeof(float) +
                 data_.size() * sizeof(std::complex<T>);

  std::vector<unsigned char> buffer;
  buffer.reserve(bytes);

  std::copy ((unsigned char*)(&this->head_),
             ((unsigned char*)(&this->head_)) + sizeof(AcquisitionHeader),
              std::back_inserter(buffer));

  std::copy ((unsigned char*)(&this->traj_[0]),
             (unsigned char*)(&this->traj_[0] + traj_.size()),
             std::back_inserter(buffer));

  std::copy ((unsigned char*)(&this->data_[0]),
             (unsigned char*)(&this->data_[0] + data_.size()),
             std::back_inserter(buffer));

  if (buffer.size() != bytes)
  {
    throw std::runtime_error
      ("Serialized buffer size does not match expected buffer size");
  }
    
  return buffer; // Should not be copied on newer compilers
                 // due to return value optimization.
}
    
template <typename T>
void Acquisition<T>::deserialize (const std::vector<unsigned char>& buffer)
{
  if (buffer.size() < sizeof(AcquisitionHeader))
  {
    throw std::runtime_error ("Buffer is too small to contain an Acquisition");
  }

  AcquisitionHeader* h_ptr = (AcquisitionHeader*)&buffer[0];

  if (h_ptr->ent_head.signature != ISMRMRD_SIGNATURE)
  {
    throw std::runtime_error ("The header signature not recognized");
  }

  if (h_ptr->ent_head.entity_type != ISMRMRD_MRACQUISITION)
  {
    throw std::runtime_error
      ("The header entity type does not match the class type");
  }

  if (h_ptr->ent_head.storage_type != get_storage_type<T>())
  {
    throw std::runtime_error("Header storage type does not match class");
  }
    
  size_t expected_bytes =
    sizeof(AcquisitionHeader) +
    h_ptr->trajectory_dimensions * h_ptr->number_of_samples * sizeof(float) +
    h_ptr->number_of_samples * h_ptr->active_channels * sizeof(std::complex<T>);

  if (expected_bytes != buffer.size())
  {
    std::stringstream ss;
    ss << "Unexpected buffer length " << buffer.size()
       << ", expected: " << expected_bytes;
    throw std::runtime_error(ss.str());
  }
    
  this->setHead(*h_ptr);
  size_t traj_start = sizeof(AcquisitionHeader);
  size_t data_start = traj_start +
    h_ptr->trajectory_dimensions * h_ptr->number_of_samples * sizeof(float);

  std::copy (&buffer[traj_start],
             &buffer[data_start],
             (unsigned char*)(&this->traj_[0]));

  std::copy (&buffer[data_start],
             &buffer[expected_bytes],
             (unsigned char*)(&this->data_[0]));
}



template <typename T>
 Image<T>::Image
(
  uint32_t matrix_size_x,
  uint32_t matrix_size_y,
  uint32_t matrix_size_z,
  uint32_t channels
)
{
    memset (&head_, 0, sizeof(head_));
    head_.ent_head.signature = ISMRMRD_SIGNATURE;
    head_.ent_head.entity_type = ISMRMRD_IMAGE;
    head_.ent_head.storage_type = static_cast<uint32_t>(get_storage_type<T>());
    this->resize (matrix_size_x, matrix_size_y, matrix_size_z, channels);
}

// Image dimensions
template <typename T> void Image<T>::resize(uint32_t matrix_size_x,
                                            uint32_t matrix_size_y,
                                            uint32_t matrix_size_z,
                                            uint32_t channels)
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

template <typename T>
void Image<T>::setStream (uint32_t stream_number)
{
  head_.ent_head.stream = stream_number;
}

template <typename T> uint32_t Image<T>::getMatrixSizeX() const
{
    return head_.matrix_size[0];
}

template <typename T> uint32_t Image<T>::getMatrixSizeY() const
{
    return head_.matrix_size[1];
}

template <typename T> uint32_t Image<T>::getMatrixSizeZ() const
{
    return head_.matrix_size[2];
}

template <typename T> void Image<T>::setMatrixSizeX(uint32_t x)
{
    head_.matrix_size[0] = x;
    this->makeConsistent();
}

template <typename T> void Image<T>::setMatrixSizeY(uint32_t y)
{
    head_.matrix_size[1] = y;
    this->makeConsistent();
}

template <typename T> void Image<T>::setMatrixSizeZ(uint32_t z)
{
    head_.matrix_size[2] = z;
    this->makeConsistent();
}

template <typename T> uint32_t Image<T>::getNumberOfChannels() const
{
    return head_.channels;
}

template <typename T> void Image<T>::setNumberOfChannels(uint32_t channels)
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

template <typename T> uint32_t Image<T>::getAverage() const
{
    return head_.average;
}

template <typename T> void  Image<T>::setAverage(uint32_t average)
{
    head_.average = average;
}

template <typename T> uint32_t Image<T>::getSlice() const
{
    return head_.slice;
}

template <typename T> void  Image<T>::setSlice(uint32_t slice)
{
    head_.slice = slice;
}

template <typename T> uint32_t Image<T>::getContrast() const
{
    return head_.contrast;
}

template <typename T> void  Image<T>::setContrast(uint32_t contrast)
{
    head_.contrast = contrast;
}

template <typename T> uint32_t Image<T>::getPhase() const
{
    return head_.phase;
}

template <typename T> void  Image<T>::setPhase(uint32_t phase)
{
    head_.phase = phase;
}

template <typename T> uint32_t Image<T>::getRepetition() const
{
    return head_.repetition;
}

template <typename T> void  Image<T>::setRepetition(uint32_t repetition)
{
    head_.repetition = repetition;
}

template <typename T> uint32_t Image<T>::getSet() const
{
    return head_.set;
}

template <typename T> void  Image<T>::setSet(uint32_t set)
{
    head_.set = set;
}

template <typename T> uint64_t Image<T>::getTimeStamp() const
{
    return head_.time_stamp;
}

template <typename T> void  Image<T>::setTimeStamp(uint64_t time_stamp)
{
    head_.time_stamp = time_stamp;
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

template <typename T> uint32_t Image<T>::getImageType() const
{
    return head_.image_type;
}

template <typename T> void Image<T>::setImageType(uint32_t image_type)
{
    head_.image_type = image_type;
}

template <typename T> uint32_t Image<T>::getImageIndex() const
{
    return head_.image_index;
}

template <typename T> void Image<T>::setImageIndex(uint32_t image_index)
{
    head_.image_index = image_index;
}

template <typename T> uint32_t Image<T>::getImageSeriesIndex() const
{
    return head_.image_series_index;
}

template <typename T> void Image<T>::setImageSeriesIndex(uint32_t image_series_index)
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
template <typename T>
uint64_t Image<T>::getFlags() const
{
  return head_.flags;
}

template <typename T>
void Image<T>::setFlags (uint64_t val)
{
  head_.flags = val;
}

template <typename T>
bool Image<T>::isFlagSet(uint64_t val) const
{
  uint64_t bitmask = 1UL << (val - 1UL);
  return (head_.flags & bitmask) > 0;
}

template <typename T>
void Image<T>::setFlag (uint64_t val)
{
  uint64_t bitmask = 1UL << (val - 1UL);
  head_.flags |= bitmask;
}

template <typename T>
void Image<T>::clearFlag(uint64_t val)
{
  uint64_t bitmask = 1UL << (val - 1UL);
  head_.flags &= ~bitmask;
}

template <typename T>
void Image<T>::clearAllFlags()
{
  head_.flags = 0;
}

template <typename T> ImageHeader &Image<T>::getHead() {
    return head_;
}

template <typename T> const ImageHeader &Image<T>::getHead() const {
    return head_;
}

template <typename T>
void Image<T>::setHead (const ImageHeader& other)
{
  if (other.ent_head.storage_type != head_.ent_head.storage_type)
  {
    throw std::runtime_error
      ("Cannot assign a header of a different data type.");
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

template <typename T>
T& Image<T>::at (uint32_t ix, uint32_t iy, uint32_t iz, uint32_t channel)
{
  // TODO: bounds checking
  size_t sx = getMatrixSizeX();
  size_t sy = getMatrixSizeY();
  size_t sz = getMatrixSizeZ();
  size_t index = ix + (sx * iy) + (sy * sx * iz) + (sy * sx * sz * channel);
  return data_[index];
}

template <typename T>
uint32_t Image<T>::getStream () const
{
  return head_.ent_head.stream;
}

template <typename T>
uint32_t Image<T>::getSignature ()   const
{
  return head_.ent_head.signature;
}

template <typename T>
EntityType Image<T>::getEntityType ()  const
{
  return static_cast<EntityType>(head_.ent_head.entity_type);
}

template <typename T>
StorageType Image<T>::getStorageType () const
{
  return static_cast<StorageType>(head_.ent_head.storage_type);
}

template <typename T>
std::vector<unsigned char> Image<T>::serialize()
{
  if (this->head_.ent_head.signature != ISMRMRD_SIGNATURE)
  {
    throw std::runtime_error ("The header signature not recognized");
  }

  if (this->head_.ent_head.entity_type != ISMRMRD_IMAGE)
  {
    throw std::runtime_error("The header entity type does not match the class type");
  }

  if (this->head_.ent_head.storage_type != get_storage_type<T>())
  {
    throw std::runtime_error("Header storage type does not match class");
  }

  size_t bytes = sizeof(ImageHeader) +
                 attribute_string_.size() +
                 data_.size() * sizeof(T);

  std::vector<unsigned char> buffer;
  buffer.reserve(bytes);

  std::copy ((unsigned char*)(&this->head_),
             ((unsigned char*)(&this->head_)) + sizeof(head_),
             std::back_inserter(buffer));

  std::copy ((unsigned char*)(&this->attribute_string_[0]),
             (unsigned char*)(&this->attribute_string_[0] +
               attribute_string_.size()),
             std::back_inserter(buffer));

  std::copy ((unsigned char*)(&this->data_[0]),
             (unsigned char*)(&this->data_[0] + data_.size()),
             std::back_inserter(buffer));

  if (buffer.size() != bytes)
  {
    throw std::runtime_error
      ("Serialized buffer size does not match expected buffer size");
  }
    
  return buffer; // Should not be copied on newer compilers
                 // due to return value optimization.
}
    
template <typename T>
void Image<T>::deserialize (const std::vector<unsigned char>& buffer)
{
  if (buffer.size() < sizeof(ImageHeader))
  {
    throw std::runtime_error("Buffer is too small to contain an Acquisition");
  }

  ImageHeader* h_ptr = (ImageHeader*)&buffer[0];

  if (h_ptr->ent_head.signature != ISMRMRD_SIGNATURE)
  {
    throw std::runtime_error ("The header signature not recognized");
  }

  if (h_ptr->ent_head.entity_type != ISMRMRD_IMAGE)
  {
    throw std::runtime_error
      ("The header entity type does not match the class type");
  }

  if (h_ptr->ent_head.storage_type != get_storage_type<T>())
  {
    throw std::runtime_error ("Header storage type does not match class");
  }
    
  size_t expected_bytes = sizeof(ImageHeader) + h_ptr->attribute_string_len +
                          h_ptr->matrix_size[0] * h_ptr->matrix_size[1] *
                          h_ptr->matrix_size[2] * h_ptr->channels * sizeof(T);

  if (expected_bytes != buffer.size())
  {
    std::stringstream ss;
    ss << "Unexpected buffer length " << buffer.size()
       << ", expected: " << expected_bytes;
    throw std::runtime_error(ss.str());
  }
  
  this->setHead(*h_ptr);
  size_t attr_start = sizeof(AcquisitionHeader);
  size_t data_start = attr_start + h_ptr->attribute_string_len;

  this->attribute_string_ = std::string ((char*)&buffer[attr_start],
                                         h_ptr->attribute_string_len);
  std::copy (&buffer[data_start],
             &buffer[expected_bytes],
             (unsigned char*)(&this->data_[0]));
}
    
//
// Array class Implementation
//
template <typename T>
NDArray<T>::NDArray()
: signature_ (ISMRMRD_SIGNATURE)
{ }

template <typename T>
NDArray<T>::NDArray (const std::vector<size_t>& dims)
: signature_ (ISMRMRD_SIGNATURE)
{
    resize(dims);
}

template <typename T> uint32_t NDArray<T>::getSignature() const {
    return signature_;
};

template <typename T>
StorageType NDArray<T>::getStorageType() const
{
  return static_cast<StorageType>(get_storage_type<T>());
}

template <typename T> uint32_t NDArray<T>::getNDim() const {
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

template <typename T> T& NDArray<T>::at(uint32_t x, uint32_t y, uint32_t z,
        uint32_t w, uint32_t n, uint32_t m, uint32_t l)
{
    // TODO: bounds checking
    size_t index = 0;
    uint32_t indices[ISMRMRD_NDARRAY_MAXDIM] = {x,y,z,w,n,m,l};
    size_t stride = 1;
    for (uint32_t i = 0; i < dims_.size(); i++){
        index += indices[i] * stride;
        stride *= dims_[i];
    }

    return data_[index];
}

// Acquisitions
template EXPORTISMRMRD class Acquisition<int16_t>;
template EXPORTISMRMRD class Acquisition<int32_t>;
template EXPORTISMRMRD class Acquisition<float>;
template EXPORTISMRMRD class Acquisition<double>;

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
