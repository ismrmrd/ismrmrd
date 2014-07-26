//
//  ISMRMRD C++ Interface
//

// TODO:
// - exports for all the classes

#include "ismrmrd.h"

#pragma once
#ifndef ISMRMRD_HPP
#define ISMRMRD_HPP

namespace ISMRMRD {

class AcquisitionHeader {
public:
    AcquisitionHeader();

    // Accessors and mutators
    const uint16_t &version();
    const uint64_t &flags();
    uint32_t &measurement_uid();
    uint32_t &scan_counter();
    uint32_t &acquisition_time_stamp();
    uint32_t (&physiology_time_stamp())[ISMRMRD_PHYS_STAMPS];
    uint16_t &number_of_samples();
    uint16_t &available_channels();
    uint16_t &active_channels();
    const uint64_t (&channel_mask())[ISMRMRD_CHANNEL_MASKS];
    uint16_t &discard_pre();
    uint16_t &discard_post();
    uint16_t &center_sample();
    uint16_t &encoding_space_ref();
    uint16_t &trajectory_dimensions();
    float &sample_time_us();
    float (&position())[3];
    float (&read_dir())[3];
    float (&phase_dir())[3];
    float (&slice_dir())[3];
    float (&patient_table_position())[3];
    ISMRMRD_EncodingCounters &idx();
    int32_t (&user_int())[ISMRMRD_USER_INTS];
    float (&user_float())[ISMRMRD_USER_FLOATS];

    // Flag methods
    bool isFlagSet(const uint64_t val);
    void setFlag(const uint64_t val);
    void clearFlag(const uint64_t val);
    void clearAllFlags();

    // Channel mask methods
    // TODO: need to add the functionality for these
    //bool isChannelActive(uint16_t channel_id);
    //void setChannelActive(uint16_t channel_id);
    //void setChannelNotActive(uint16_t channel_id);
    //void setAllChannelsNotActive();

protected:
    ISMRMRD_AcquisitionHeader head_;
};

class Acquisition {
public:
    Acquisition();

    // Accessors and mutators
    const uint16_t &version();
    const uint64_t &flags();
    uint32_t &measurement_uid();
    uint32_t &scan_counter();
    uint32_t &acquisition_time_stamp();
    uint32_t (&physiology_time_stamp())[ISMRMRD_PHYS_STAMPS];
    const uint16_t &number_of_samples();
    void number_of_samples(uint16_t num_samples);
    uint16_t &available_channels();
    const uint16_t &active_channels();
    void active_channels(uint16_t num_active_channels);
    const uint64_t (&channel_mask())[ISMRMRD_CHANNEL_MASKS];
    uint16_t &discard_pre();
    uint16_t &discard_post();
    uint16_t &center_sample();
    uint16_t &encoding_space_ref();
    uint16_t &trajectory_dimensions();
    float &sample_time_us();
    float (&position())[3];
    float (&read_dir())[3];
    float (&phase_dir())[3];
    float (&slice_dir())[3];
    float (&patient_table_position())[3];
    ISMRMRD_EncodingCounters &idx();
    int32_t (&user_int())[ISMRMRD_USER_INTS];
    float (&user_float())[ISMRMRD_USER_FLOATS];

    // Flag methods
    bool isFlagSet(const uint64_t val);
    void setFlag(const uint64_t val);
    void clearFlag(const uint64_t val);
    void clearAllFlags();

    // Channel mask methods
    // TODO: need to add the functionality for these
    //bool isChannelActive(uint16_t channel_id);
    //void setChannelActive(uint16_t channel_id);
    //void setChannelNotActive(uint16_t channel_id);
    //void setAllChannelsNotActive();

protected:
    ISMRMRD_Acquisition acq_;
};

class ImageHeader {
public:
    // Constructors
    ImageHeader();
    ImageHeader(ImageHeader &hdr);
    ImageHeader(ISMRMRD_ImageHeader *hdr);

    // Accessors and mutators
    const uint16_t &version();
    const uint16_t &data_type();
    void data_type(uint16_t dtype);
    const uint64_t &flags();
    uint32_t &measurement_uid();
    const uint16_t (&matrix_size())[3];
    void matrix_size(const uint16_t msize[3]);
    float (&field_of_view())[3];
    const uint16_t &channels();
    void channels(const uint16_t num_channels);
    float (&position())[3];
    float (&read_dir())[3];
    float (&phase_dir())[3];
    float (&slice_dir())[3];
    float (&patient_table_position())[3];
    uint16_t &average();
    uint16_t &slice();
    uint16_t &contrast();
    uint16_t &phase();
    uint16_t &repetition();
    uint16_t &set();
    uint32_t &acquisition_time_stamp();
    uint32_t (&physiology_time_stamp())[ISMRMRD_PHYS_STAMPS];
    uint16_t &image_type();
    uint16_t &image_index();
    uint16_t &image_series_index();
    int32_t (&user_int())[ISMRMRD_USER_INTS];
    float (&user_float())[ISMRMRD_USER_FLOATS];
    const uint32_t &attribute_string_len();

    // Flag methods
    bool isFlagSet(const uint64_t val);
    void setFlag(const uint64_t val);
    void clearFlag(const uint64_t val);
    void clearAllFlags();

protected:
    ISMRMRD_ImageHeader head_;
};

class Image {
public:
    // Constructors
    Image();
    Image(Image &im);
    Image(ISMRMRD_Image *im);

    // Accessors and mutators
    const uint16_t &version();
    const uint16_t &data_type();
    void data_type(uint16_t dtype);
    const uint64_t &flags();
    uint32_t &measurement_uid();
    const uint16_t (&matrix_size())[3];
    void matrix_size(const uint16_t msize[3]);
    float (&field_of_view())[3];
    const uint16_t &channels();
    void channels(const uint16_t num_channels);
    float (&position())[3];
    float (&read_dir())[3];
    float (&phase_dir())[3];
    float (&slice_dir())[3];
    float (&patient_table_position())[3];
    uint16_t &average();
    uint16_t &slice();
    uint16_t &contrast();
    uint16_t &phase();
    uint16_t &repetition();
    uint16_t &set();
    uint32_t &acquisition_time_stamp();
    uint32_t (&physiology_time_stamp())[ISMRMRD_PHYS_STAMPS];
    uint16_t &image_type();
    uint16_t &image_index();
    uint16_t &image_series_index();
    int32_t (&user_int())[ISMRMRD_USER_INTS];
    float (&user_float())[ISMRMRD_USER_FLOATS];
    const uint32_t &attribute_string_len();

    // Flag methods
    bool isFlagSet(const uint64_t val);
    void setFlag(const uint64_t val);
    void clearFlag(const uint64_t val);
    void clearAllFlags();

protected:
    ISMRMRD_Image image_;
};

class NDArray : protected ISMRMRD_NDArray {
};

} // namespace ISMRMRD

#endif // ISMRMRD_HPP
