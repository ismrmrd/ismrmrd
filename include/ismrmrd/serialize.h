/* Serialization for ISMRMRD MR Raw Data Structures        */
/* Authors:                                                */
/*    L. Martyn Klassen (martyn.klassen@uwo.ca)            */

/**
 * @file serialize.h
 * @defgroup cxxapi C++ API
 */

#pragma once
#ifndef ISMRMRD_SERIALIZE_H
#define ISMRMRD_SERIALIZE_H

#include "ismrmrd/version.h"

#if ISMRMRD_SERIALIZATION_SUPPORT

#include <stdexcept>

#define CEREAL_FUTURE_EXPERIMENTAL

#include "ismrmrd.h"
#include "waveform.h"
#include "xml.h"
#include <cereal/archives/adapters.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <limits>

// Streaming is version dependent
// This file must be updated to reflect the current version of ISMRMRD
// to ensure that compatibility has been properly tested
#if (ISMRMRD_VERSION_MAJOR != 1) | (ISMRMRD_VERSION_MINOR != 14) | (ISMRMRD_VERSION_PATCH != 2)
#error "Version mismatch for streaming"
#endif

// Cereal version requires update if ISMRMRD data structures change
// Not all ISMRMRD data structures are versioned, so serialization versions must be maintained separately
#define ISMRMRD_SERIALIZE_VERSION 0

// Access to the protected members of ISMRMRD C++ classes via friend
namespace ISMRMRD {

EXPORTISMRMRD void decompress_acquisition(ISMRMRD::ISMRMRD_Acquisition &acq, std::vector<uint8_t> &buffer);
EXPORTISMRMRD void compress_acquisition(ISMRMRD::ISMRMRD_Acquisition const &acq, std::vector<uint8_t> &buffer, unsigned int compression_precision = 0, float compression_tolerance = 0.0);

EXPORTISMRMRD void decompress_image(ISMRMRD::ISMRMRD_Image &image, std::vector<uint8_t> &buffer);
EXPORTISMRMRD void compress_image(ISMRMRD::ISMRMRD_Image const &image, std::vector<uint8_t> &buffer, unsigned int compression_precision = 0, float compression_tolerance = 0.0);

EXPORTISMRMRD void compress_acquisition_nhlbi(ISMRMRD::ISMRMRD_Acquisition const &acq, std::vector<uint8_t> &buffer, float tolerance = -1, uint8_t precision = 32);
EXPORTISMRMRD void decompress_acquisition_nhlbi(ISMRMRD::ISMRMRD_Acquisition &acq, std::vector<uint8_t> &buffer);

struct CompressionParameters {
    float tolerance = 0.0;
    unsigned int precision = 0;
};

typedef cereal::UserDataAdapter<CompressionParameters, cereal::PortableBinaryOutputArchive> CompressiblePortableBinaryOutputArchive;
typedef cereal::PortableBinaryInputArchive CompressiblePortableBinaryInputArchive;

class Serialize {
public:
    static ISMRMRD::ISMRMRD_Acquisition &access(ISMRMRD::Acquisition &acq) {
        return acq.acq;
    }

    template <typename T>
    static ISMRMRD::ISMRMRD_Image &access(ISMRMRD::Image<T> &im) {
        return im.im;
    }

    template <typename T>
    static ISMRMRD::ISMRMRD_NDArray &access(ISMRMRD::NDArray<T> &arr) {
        return arr.arr;
    }
};

} // namespace ISMRMRD

// External cereal serialization functions
namespace cereal {

template <class Archive>
void serialize(Archive &ar, ISMRMRD::ISMRMRD_EncodingCounters &counter, const unsigned int version) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");
    ar(
        make_nvp("kspace_encode_step_1", counter.kspace_encode_step_1),
        make_nvp("kspace_encode_step_2", counter.kspace_encode_step_2),
        make_nvp("average", counter.average),
        make_nvp("slice", counter.slice),
        make_nvp("contrast", counter.contrast),
        make_nvp("phase", counter.phase),
        make_nvp("repetition", counter.repetition),
        make_nvp("set", counter.set),
        make_nvp("segment", counter.segment),
        make_nvp("user", counter.user));
}

template <class Archive>
void serialize(Archive &ar, ISMRMRD::ISMRMRD_AcquisitionHeader &header, const unsigned int version) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");
    ar(
        make_nvp("version", header.version),
        make_nvp("flags", header.flags),
        make_nvp("measurement_uid", header.measurement_uid),
        make_nvp("scan_counter", header.scan_counter),
        make_nvp("acquisition_time_stamp", header.acquisition_time_stamp),
        make_nvp("physiology_time_stamp", header.physiology_time_stamp),
        make_nvp("number_of_samples", header.number_of_samples),
        make_nvp("available_channels", header.available_channels),
        make_nvp("active_channels", header.active_channels),
        make_nvp("channel_mask", header.channel_mask),
        make_nvp("discard_pre", header.discard_pre),
        make_nvp("discard_post", header.discard_post),
        make_nvp("center_sample", header.center_sample),
        make_nvp("encoding_space_ref", header.encoding_space_ref),
        make_nvp("trajectory_dimensions", header.trajectory_dimensions),
        make_nvp("sample_time_us", header.sample_time_us),
        make_nvp("position", header.position),
        make_nvp("read_dir", header.read_dir),
        make_nvp("phase_dir", header.phase_dir),
        make_nvp("slice_dir", header.slice_dir),
        make_nvp("patient_table_position", header.patient_table_position),
        make_nvp("idx", header.idx),
        make_nvp("user_int", header.user_int),
        make_nvp("user_float", header.user_float));
}

template <class Archive>
void serialize(Archive &ar, ISMRMRD::ISMRMRD_ImageHeader &header, const unsigned int version) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");
    ar(
        make_nvp("version", header.version),
        make_nvp("data_type", header.data_type),
        make_nvp("flags", header.flags),
        make_nvp("measurement_uid", header.measurement_uid),
        make_nvp("matrix_size", header.matrix_size),
        make_nvp("field_of_view", header.field_of_view),
        make_nvp("channels", header.channels),
        make_nvp("position", header.position),
        make_nvp("read_dir", header.read_dir),
        make_nvp("phase_dir", header.phase_dir),
        make_nvp("slice_dir", header.slice_dir),
        make_nvp("patient_table_position", header.patient_table_position),
        make_nvp("average", header.average),
        make_nvp("slice", header.slice),
        make_nvp("contrast", header.contrast),
        make_nvp("phase", header.phase),
        make_nvp("repetition", header.repetition),
        make_nvp("set", header.set),
        make_nvp("acquisition_time_stamp", header.acquisition_time_stamp),
        make_nvp("physiology_time_stamp", header.physiology_time_stamp),
        make_nvp("image_type", header.image_type),
        make_nvp("image_index", header.image_index),
        make_nvp("image_series_index", header.image_series_index),
        make_nvp("user_int", header.user_int),
        make_nvp("user_float", header.user_float),
        make_nvp("attribute_string_len", header.attribute_string_len));
}

template <class Archive>
void serialize(Archive &ar, ISMRMRD::ISMRMRD_WaveformHeader &header, const unsigned int version) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");
    ar(
        make_nvp("version", header.version),
        make_nvp("flags", header.flags),
        make_nvp("measurement_uid", header.measurement_uid),
        make_nvp("scan_counter", header.scan_counter),
        make_nvp("time_stamp", header.time_stamp),
        make_nvp("number_of_samples", header.number_of_samples),
        make_nvp("channels", header.channels),
        make_nvp("sample_time_us", header.sample_time_us),
        make_nvp("waveform_id", header.waveform_id));
}

template <class Archive>
void serialize(Archive &ar, ISMRMRD::AcquisitionHeader &header, const unsigned int version) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");
    // All data is in the base class
    ar(cereal::base_class<ISMRMRD::ISMRMRD_AcquisitionHeader>(&header));
}

template <class Archive>
void serialize(Archive &ar, ISMRMRD::ImageHeader &header, const unsigned int version) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");
    // All data is in the base class
    ar(cereal::base_class<ISMRMRD::ISMRMRD_ImageHeader>(&header));
}

template <class Archive>
void serialize(Archive &ar, ISMRMRD::WaveformHeader &header, const unsigned int version) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");
    // All data is in the base class
    ar(cereal::base_class<ISMRMRD::ISMRMRD_WaveformHeader>(&header));
}

template <class Archive>
void archive_data(Archive &ar, uint16_t data_type, void *data, size_t datasize) {
    switch (data_type) {
    case ISMRMRD::ISMRMRD_USHORT:
        ar(make_nvp("data", binary_data(reinterpret_cast<uint16_t *>(data), datasize)));
        break;
    case ISMRMRD::ISMRMRD_SHORT:
        ar(make_nvp("data", binary_data(reinterpret_cast<int16_t *>(data), datasize)));
        break;
    case ISMRMRD::ISMRMRD_UINT:
        ar(make_nvp("data", binary_data(reinterpret_cast<uint32_t *>(data), datasize)));
        break;
    case ISMRMRD::ISMRMRD_INT:
        ar(make_nvp("data", binary_data(reinterpret_cast<int32_t *>(data), datasize)));
        break;
    case ISMRMRD::ISMRMRD_FLOAT:
        ar(make_nvp("data", binary_data(reinterpret_cast<float *>(data), datasize)));
        break;
    case ISMRMRD::ISMRMRD_DOUBLE:
        ar(make_nvp("data", binary_data(reinterpret_cast<double *>(data), datasize)));
        break;
    case ISMRMRD::ISMRMRD_CXFLOAT:
        ar(make_nvp("data", binary_data(reinterpret_cast<complex_float_t *>(data), datasize)));
        break;
    case ISMRMRD::ISMRMRD_CXDOUBLE:
        ar(make_nvp("data", binary_data(reinterpret_cast<complex_double_t *>(data), datasize)));
        break;
    default:
        throw std::runtime_error("Invalid image data type");
    }
}

template <class Archive>
void save(Archive &ar, ISMRMRD::ISMRMRD_Image const &image, const unsigned int version, unsigned int precision, float tolerance) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");
    ar(make_nvp("head", image.head));

    if (image.head.attribute_string_len > 0)
        ar(make_nvp("attribute_string", binary_data(image.attribute_string, ISMRMRD::ismrmrd_size_of_image_attribute_string(&image))));

    if (ISMRMRD::ismrmrd_is_flag_set(image.head.flags, ISMRMRD::ISMRMRD_IMAGE_COMPRESSION1)) {
        std::vector<uint8_t> data;

        ISMRMRD::compress_image(image, data, precision, tolerance);
        ar(make_nvp("data", data));
    } else {
        archive_data(ar, image.head.data_type, image.data, ISMRMRD::ismrmrd_size_of_image_data(&image));
    }
}

template <class Archive>
void save(Archive &ar, ISMRMRD::ISMRMRD_Image const &image, const unsigned int version) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");
    save(ar, image, version, 0, 0);
}

template <>
inline void save(ISMRMRD::CompressiblePortableBinaryOutputArchive &ar, ISMRMRD::ISMRMRD_Image const &image, const unsigned int version) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");
    auto &parameters = cereal::get_user_data<ISMRMRD::CompressionParameters>(ar);
    save(ar, image, version, parameters.precision, parameters.tolerance);
}

template <class Archive>
void load(Archive &ar, ISMRMRD::ISMRMRD_Image &image, const unsigned int version) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");
    ar(make_nvp("head", image.head));

    ISMRMRD::ismrmrd_make_consistent_image(&image);

    if (image.head.attribute_string_len > 0)
        ar(make_nvp("attribute_string", binary_data(image.attribute_string, ISMRMRD::ismrmrd_size_of_image_attribute_string(&image))));

    if (ISMRMRD::ismrmrd_is_flag_set(image.head.flags, ISMRMRD::ISMRMRD_IMAGE_COMPRESSION1)) {
        std::vector<uint8_t> data;

        // Compress data
        ar(data);
        ISMRMRD::decompress_image(image, data);

        // Clear the compression flag
        ISMRMRD::ismrmrd_clear_flag(&(image.head.flags), ISMRMRD::ISMRMRD_IMAGE_COMPRESSION1);
    } else {
        archive_data(ar, image.head.data_type, image.data, ISMRMRD::ismrmrd_size_of_image_data(&image));
    }
}

template <class Archive>
void save(Archive &ar, ISMRMRD::ISMRMRD_Acquisition const &acq, const unsigned int version, unsigned int precision, float tolerance) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");
    ar(make_nvp("head", acq.head));
    if (acq.head.trajectory_dimensions) {
        ar(make_nvp("traj", binary_data(acq.traj, ISMRMRD::ismrmrd_size_of_acquisition_traj(&acq))));
    }

    if (ISMRMRD::ismrmrd_is_flag_set(acq.head.flags, ISMRMRD::ISMRMRD_ACQ_COMPRESSION1)) {
        std::vector<uint8_t> data;
        ISMRMRD::compress_acquisition(acq, data, precision, tolerance);
        ar(make_nvp("data", data));
    } else if (ISMRMRD::ismrmrd_is_flag_set(acq.head.flags, ISMRMRD::ISMRMRD_ACQ_COMPRESSION2)) {
        std::vector<uint8_t> data;
        ISMRMRD::compress_acquisition_nhlbi(acq, data, tolerance, precision);
        ar(make_nvp("data", data));
    } else {
        ar(make_nvp("data", binary_data(acq.data, ISMRMRD::ismrmrd_size_of_acquisition_data(&acq))));
    }
}

template <class Archive>
void save(Archive &ar, ISMRMRD::ISMRMRD_Acquisition const &acq, const unsigned int version) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");
    save(ar, acq, version, 0, 0.0);
}

template <>
inline void save(ISMRMRD::CompressiblePortableBinaryOutputArchive &ar, ISMRMRD::ISMRMRD_Acquisition const &acq, const unsigned int version) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");
    auto &parameters = cereal::get_user_data<ISMRMRD::CompressionParameters>(ar);
    save(ar, acq, version, parameters.precision, parameters.tolerance);
}

template <class Archive>
void load(Archive &ar, ISMRMRD::ISMRMRD_Acquisition &acq, const unsigned int version) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");
    ar(make_nvp("head", acq.head));

    ISMRMRD::ismrmrd_make_consistent_acquisition(&acq);

    if (acq.head.trajectory_dimensions) {
        ar(make_nvp("traj", binary_data(acq.traj, ISMRMRD::ismrmrd_size_of_acquisition_traj(&acq))));
    }

    if (ISMRMRD::ismrmrd_is_flag_set(acq.head.flags, ISMRMRD::ISMRMRD_ACQ_COMPRESSION1)) {
        std::vector<uint8_t> data;

        // Compress data
        ar(data);
        ISMRMRD::decompress_acquisition(acq, data);

        // Clear the compression flag
        ISMRMRD::ismrmrd_clear_flag(&(acq.head.flags), ISMRMRD::ISMRMRD_ACQ_COMPRESSION1);
    } else if (ISMRMRD::ismrmrd_is_flag_set(acq.head.flags, ISMRMRD::ISMRMRD_ACQ_COMPRESSION2)) {
        std::vector<uint8_t> data;

        ar(data);
        ISMRMRD::decompress_acquisition_nhlbi(acq, data);

        // Clear the compression flag
        ISMRMRD::ismrmrd_clear_flag(&(acq.head.flags), ISMRMRD::ISMRMRD_ACQ_COMPRESSION2);
    } else {
        ar(binary_data(acq.data, ISMRMRD::ismrmrd_size_of_acquisition_data(&acq)));
    }
}

template <class Archive>
void serialize(Archive &ar, ISMRMRD::ISMRMRD_NDArray &ndArray, const unsigned int version) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");
    ar(
        make_nvp("version", ndArray.version),
        make_nvp("data_type", ndArray.data_type),
        make_nvp("ndim", ndArray.ndim),
        make_nvp("dims", ndArray.dims));

    if (Archive::is_loading::value) {
        ISMRMRD::ismrmrd_make_consistent_ndarray(&ndArray);
    }

    archive_data(ar, ndArray.data_type, ndArray.data, ISMRMRD::ismrmrd_size_of_ndarray_data(&ndArray));
}

template <class Archive>
void serialize(Archive &ar, ISMRMRD::ISMRMRD_Waveform &waveform, const unsigned int version) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");
    ar(make_nvp("head", waveform.head));

    if (Archive::is_loading::value) {
        ISMRMRD::ismrmrd_make_consistent_waveform(&waveform);
    }

    ar(make_nvp("data", binary_data(waveform.data, ISMRMRD::ismrmrd_size_of_waveform_data(&waveform))));
}

template <class Archive, typename T>
void serialize(Archive &ar, ISMRMRD::NDArray<T> &ndArray, const unsigned int version) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");
    ar(ISMRMRD::Serialize::access(ndArray));
}

template <class Archive, typename T>
void serialize(Archive &ar, ISMRMRD::Image<T> &image, const unsigned int version) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");
    ar(ISMRMRD::Serialize::access(image));
}

template <class Archive>
void serialize(Archive &ar, ISMRMRD::Acquisition &acq, const unsigned int version) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");
    ar(ISMRMRD::Serialize::access(acq));
}

template <class Archive>
void serialize(Archive &ar, ISMRMRD::IsmrmrdHeader &header, const unsigned int version) {
    if (ISMRMRD_SERIALIZE_VERSION != version)
        throw std::runtime_error("cereal version mismatch");

    if (Archive::is_loading::value) {
        std::string xml;
        ar(make_nvp("xml", xml));
        deserialize(xml.c_str(), header);
    }
    else {
        std::stringstream xml;
        serialize(header, xml);
        ar(make_nvp("xml", xml.str()));
    }
}

} // namespace cereal

CEREAL_CLASS_VERSION(ISMRMRD::ISMRMRD_EncodingCounters, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::ISMRMRD_AcquisitionHeader, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::ISMRMRD_ImageHeader, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::ISMRMRD_Acquisition, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::ISMRMRD_Image, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::ISMRMRD_NDArray, ISMRMRD_SERIALIZE_VERSION);

CEREAL_CLASS_VERSION(ISMRMRD::AcquisitionHeader, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::ImageHeader, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::IsmrmrdHeader, ISMRMRD_SERIALIZE_VERSION);

CEREAL_CLASS_VERSION(ISMRMRD::Acquisition, ISMRMRD_SERIALIZE_VERSION);

CEREAL_CLASS_VERSION(ISMRMRD::Image<uint16_t>, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::Image<int16_t>, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::Image<uint32_t>, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::Image<int32_t>, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::Image<float>, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::Image<double>, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::Image<complex_float_t>, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::Image<complex_double_t>, ISMRMRD_SERIALIZE_VERSION);

CEREAL_CLASS_VERSION(ISMRMRD::NDArray<uint16_t>, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::NDArray<int16_t>, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::NDArray<uint32_t>, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::NDArray<int32_t>, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::NDArray<float>, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::NDArray<double>, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::NDArray<complex_float_t>, ISMRMRD_SERIALIZE_VERSION);
CEREAL_CLASS_VERSION(ISMRMRD::NDArray<complex_double_t>, ISMRMRD_SERIALIZE_VERSION);

#endif

#endif /*ISMRMRD_SERIALIZE_H*/
