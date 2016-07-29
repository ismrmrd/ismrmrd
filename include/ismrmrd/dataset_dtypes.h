#pragma once
#ifndef ISMRMRD_DATASET_DTYPES_H
#define ISMRMRD_DATASET_DTYPES_H

#include "ismrmrd/ismrmrd.h"

#include <H5Cpp.h>

#ifndef H5_NO_NAMESPACE
    using namespace H5;
#endif // H5_NO_NAMESPACE

namespace ISMRMRD {


template <typename T> struct AcquisitionHeader_with_data
{
    AcquisitionHeader head;
    hvl_t traj;
    hvl_t data;
};

// TODO 1. Image contents will need to be stored separately to retain the ability to view the image.
template <typename T> struct ImageHeader_with_data
{
	ImageHeader head;
	hvl_t data;
};

// TODO 2. Define WaveformHeader_with_data

struct IndexEntry
{
    uint32_t stream;
    // TODO 3. Add substream
    uint32_t index;
};

template <typename T>
DataType get_hdf5_data_type();

template <> DataType get_hdf5_data_type<IndexEntry>()
{
    CompType dtype(sizeof(IndexEntry));

    dtype.insertMember("stream", HOFFSET(IndexEntry, stream), PredType::NATIVE_UINT32);
    dtype.insertMember("index", HOFFSET(IndexEntry, index), PredType::NATIVE_UINT32);

    return dtype;
}

template <> DataType get_hdf5_data_type<EncodingCounters>()
{
    CompType dtype(sizeof(EncodingCounters));

    dtype.insertMember("kspace_encode_step_0", HOFFSET(EncodingCounters, kspace_encode_step_0), PredType::NATIVE_UINT32);
    dtype.insertMember("kspace_encode_step_1", HOFFSET(EncodingCounters, kspace_encode_step_1), PredType::NATIVE_UINT32);
    dtype.insertMember("kspace_encode_step_2", HOFFSET(EncodingCounters, kspace_encode_step_2), PredType::NATIVE_UINT32);
    dtype.insertMember("average", HOFFSET(EncodingCounters, average), PredType::NATIVE_UINT32);
    dtype.insertMember("slice", HOFFSET(EncodingCounters, slice), PredType::NATIVE_UINT32);
    dtype.insertMember("contrast", HOFFSET(EncodingCounters, contrast), PredType::NATIVE_UINT32);
    dtype.insertMember("phase", HOFFSET(EncodingCounters, phase), PredType::NATIVE_UINT32);
    dtype.insertMember("repetition", HOFFSET(EncodingCounters, repetition), PredType::NATIVE_UINT32);
    dtype.insertMember("set", HOFFSET(EncodingCounters, set), PredType::NATIVE_UINT32);
    dtype.insertMember("segment", HOFFSET(EncodingCounters, segment), PredType::NATIVE_UINT32);

    std::vector<hsize_t> dims(1, ISMRMRD_USER_INTS);
    DataType array_type = ArrayType(PredType::NATIVE_UINT32, 1, &dims[0]);
    dtype.insertMember("user", HOFFSET(EncodingCounters, user), array_type);

    return dtype;
}

template <> DataType get_hdf5_data_type<AcquisitionHeader>()
{
  CompType dtype (sizeof(AcquisitionHeader));

  dtype.insertMember("time_stamp", HOFFSET(AcquisitionHeader, time_stamp),  PredType::NATIVE_UINT64);
  dtype.insertMember("flags",  HOFFSET(AcquisitionHeader, flags),  PredType::NATIVE_UINT64);
  dtype.insertMember("scan_counter",  HOFFSET(AcquisitionHeader, scan_counter),  PredType::NATIVE_UINT32);

  std::vector<hsize_t> dims(1, ISMRMRD_PHYS_STAMPS);
  DataType array_type = ArrayType(PredType::NATIVE_UINT32, 1, &dims[0]);
  dtype.insertMember("physiology_time_stamp", HOFFSET(AcquisitionHeader, physiology_time_stamp), array_type);
  dtype.insertMember("number_of_samples",  HOFFSET(AcquisitionHeader, number_of_samples),  PredType::NATIVE_UINT32);
  dtype.insertMember("available_channels",  HOFFSET(AcquisitionHeader, available_channels),  PredType::NATIVE_UINT32);
  dtype.insertMember("active_channels",  HOFFSET(AcquisitionHeader, active_channels),  PredType::NATIVE_UINT32);

  dims[0] = ISMRMRD_CHANNEL_MASKS;
  DataType mask_array_type = ArrayType(PredType::NATIVE_UINT64, 1, &dims[0]);
  dtype.insertMember("channel_mask",  HOFFSET(AcquisitionHeader, channel_mask), mask_array_type);
  dtype.insertMember("discard_pre",  HOFFSET(AcquisitionHeader, discard_pre),  PredType::NATIVE_UINT32);
  dtype.insertMember("discard_post",  HOFFSET(AcquisitionHeader, discard_post),  PredType::NATIVE_UINT32);
  dtype.insertMember("center_sample",  HOFFSET(AcquisitionHeader, center_sample),  PredType::NATIVE_UINT32);
  dtype.insertMember("encoding_space_ref",  HOFFSET(AcquisitionHeader, encoding_space_ref),  PredType::NATIVE_UINT32);
  dtype.insertMember("trajectory_dimensions", HOFFSET(AcquisitionHeader, trajectory_dimensions),  PredType::NATIVE_UINT32);
  dtype.insertMember("dwell_time_ns",  HOFFSET(AcquisitionHeader, dwell_time_ns),  PredType::NATIVE_UINT32);

  dims[0] = ISMRMRD_POSITION_LENGTH;
  DataType position_array_type = ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]);
  dtype.insertMember("position",  HOFFSET(AcquisitionHeader, position), position_array_type);

  dims[0] = ISMRMRD_POSITION_LENGTH;
  DataType read_dir_array_type = ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]);
  dtype.insertMember("read_dir",  HOFFSET(AcquisitionHeader, read_dir), read_dir_array_type);

  dims[0] = ISMRMRD_POSITION_LENGTH;
  DataType phase_dir_array_type = ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]);
  dtype.insertMember("phase_dir",  HOFFSET(AcquisitionHeader, phase_dir), phase_dir_array_type);

  dims[0] = ISMRMRD_POSITION_LENGTH;
  DataType slice_dir_array_type = ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]);
  dtype.insertMember("slice_dir",  HOFFSET(AcquisitionHeader, slice_dir), slice_dir_array_type);

  dims[0] = ISMRMRD_POSITION_LENGTH;
  DataType table_array_type = ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]);
  dtype.insertMember("patient_table_position",  HOFFSET(AcquisitionHeader, patient_table_position), table_array_type);

  DataType ec_type = get_hdf5_data_type<EncodingCounters>();
  dtype.insertMember("idx",  HOFFSET(AcquisitionHeader, idx), ec_type);

  dims[0] = ISMRMRD_USER_INTS;
  DataType user_int_array_type = ArrayType(PredType::NATIVE_INT32, 1, &dims[0]);
  dtype.insertMember("user_int",  HOFFSET(AcquisitionHeader, user_int), user_int_array_type);

  dims[0] = ISMRMRD_USER_FLOATS;
  DataType user_float_array_type = ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]);
  dtype.insertMember("user_float",  HOFFSET(AcquisitionHeader, user_float), user_float_array_type);

  return dtype;
}


template <> DataType get_hdf5_data_type<AcquisitionHeader_with_data<double> >()
{
    CompType dtype(sizeof(AcquisitionHeader_with_data<double>));

    DataType head_type = get_hdf5_data_type<AcquisitionHeader>();

    DataType realv_type = DataType(H5Tvlen_create(PredType::NATIVE_FLOAT.getId()));

    dtype.insertMember("head", HOFFSET(AcquisitionHeader_with_data<double>, head), head_type);
    dtype.insertMember("traj", HOFFSET(AcquisitionHeader_with_data<double>, traj), realv_type);
    dtype.insertMember("data", HOFFSET(AcquisitionHeader_with_data<double>, data), realv_type);

    return dtype;
}

template <> DataType get_hdf5_data_type<AcquisitionHeader_with_data<float> >()
{
    CompType dtype(sizeof(AcquisitionHeader_with_data<float>));

    DataType head_type = get_hdf5_data_type<AcquisitionHeader>();

    DataType realv_type = DataType(H5Tvlen_create(PredType::NATIVE_FLOAT.getId()));

    dtype.insertMember("head", HOFFSET(AcquisitionHeader_with_data<float>, head), head_type);
    dtype.insertMember("traj", HOFFSET(AcquisitionHeader_with_data<float>, traj), realv_type);
    dtype.insertMember("data", HOFFSET(AcquisitionHeader_with_data<float>, data), realv_type);

    return dtype;
}

template <> DataType get_hdf5_data_type<AcquisitionHeader_with_data<int16_t> >()
{
    CompType dtype(sizeof(AcquisitionHeader_with_data<int16_t>));

    DataType head_type = get_hdf5_data_type<AcquisitionHeader>();

    DataType realv_float_type = DataType(H5Tvlen_create(PredType::NATIVE_FLOAT.getId()));
    DataType realv_int_type = DataType(H5Tvlen_create(PredType::NATIVE_INT16.getId()));

    dtype.insertMember("head", HOFFSET(AcquisitionHeader_with_data<int16_t>, head), head_type);
    dtype.insertMember("traj", HOFFSET(AcquisitionHeader_with_data<int16_t>, traj), realv_float_type);
    dtype.insertMember("data", HOFFSET(AcquisitionHeader_with_data<int16_t>, data), realv_int_type);

    return dtype;
}

template <> DataType get_hdf5_data_type<AcquisitionHeader_with_data<int32_t> >()
{
    CompType dtype(sizeof(AcquisitionHeader_with_data<int32_t>));

    DataType head_type = get_hdf5_data_type<AcquisitionHeader>();

    DataType realv_float_type = DataType(H5Tvlen_create(PredType::NATIVE_FLOAT.getId()));
    DataType realv_int_type = DataType(H5Tvlen_create(PredType::NATIVE_INT32.getId()));

    dtype.insertMember("head", HOFFSET(AcquisitionHeader_with_data<int32_t>, head), head_type);
    dtype.insertMember("traj", HOFFSET(AcquisitionHeader_with_data<int32_t>, traj), realv_float_type);
    dtype.insertMember("data", HOFFSET(AcquisitionHeader_with_data<int32_t>, data), realv_int_type);

    return dtype;
}

} // namespace ISMRMRD

#endif // ISMRMRD_DATASET_DTYPES_H
