/* ISMRMRD MR Raw Data Strutures                           */
/* DRAFT                                                   */
/* Authors:                                                */
/*    Michael S. Hansen (michael.hansen@nih.gov)           */
/*    Brian Hargreaves  (bah@stanford.edu)                 */
/*    Sebastian Kozerke (kozerke@biomed.ee.ethz.ch)        */
/*    Kaveh Vahedipour  (k.vahedipour@fz-juelich.de)       */
/*    Hui Xue           (hui.xue@nih.gov)                  */
/*    Souheil Inati     (souheil.inati@nih.gov)            */
/*    Joeseph Naegele   (joseph.naegele@gmail.com)         */

#pragma once
#ifndef ISMRMRD_H
#define ISMRMRD_H

/* Cross platform section for defining integer types */
#ifndef ISMRMRD_HAS_BASIC_TYPES
#ifdef _MSC_VER
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#include <math.h>
#endif
#endif

#ifdef __cplusplus
#include <stdio.h>
#include <string.h>
#include <exception>
#include <iostream>
#include <vector>
#include <valarray>
#include <complex>
#include <assert.h>
#endif

#pragma pack(push, 2) //Use 2 byte alignment

#define ISMRMRD_VERSION           1
#define ISMRMRD_POSITION_LENGTH   3
#define ISMRMRD_DIRECTION_LENGTH  3
#define ISMRMRD_USER_INTS         8
#define ISMRMRD_USER_FLOATS       8
#define ISMRMRD_PHYS_STAMPS       8  //TODO: This should be changed to 3 (Major impact)
#define ISMRMRD_CHANNEL_MASKS     16

#ifdef __cplusplus
namespace ISMRMRD
{

class FlagBit
{
public:
 FlagBit(unsigned short b)
   : bitmask_(0)
    {
      if (b > 0) {
    bitmask_ = 1;
    bitmask_ = (bitmask_ << (b-1));
      }
    }
  
  bool isSet(const uint64_t& m) const {
    return ((m & bitmask_)>0);
  }
  
  uint64_t bitmask_;
  
};
#endif //__cplusplus

/** ACQUISITION FLAGS */
enum AcquisitionFlags {
    /* Looping indicators */
    ACQ_FIRST_IN_ENCODE_STEP1                   = 1,
    ACQ_LAST_IN_ENCODE_STEP1                    = 2,
    ACQ_FIRST_IN_ENCODE_STEP2                   = 3,
    ACQ_LAST_IN_ENCODE_STEP2                    = 4,
    ACQ_FIRST_IN_AVERAGE                        = 5,
    ACQ_LAST_IN_AVERAGE                         = 6,
    ACQ_FIRST_IN_SLICE                          = 7,
    ACQ_LAST_IN_SLICE                           = 8,
    ACQ_FIRST_IN_CONTRAST                       = 9,
    ACQ_LAST_IN_CONTRAST                        = 10,
    ACQ_FIRST_IN_PHASE                          = 11,
    ACQ_LAST_IN_PHASE                           = 12,
    ACQ_FIRST_IN_REPETITION                     = 13,
    ACQ_LAST_IN_REPETITION                      = 14,
    ACQ_FIRST_IN_SET                            = 15,
    ACQ_LAST_IN_SET                             = 16,
    ACQ_FIRST_IN_SEGMENT                        = 17,
    ACQ_LAST_IN_SEGMENT                         = 18,
    ACQ_IS_NOISE_MEASUREMENT                    = 19,
    ACQ_IS_PARALLEL_CALIBRATION                 = 20,
    ACQ_IS_PARALLEL_CALIBRATION_AND_IMAGING     = 21,
    ACQ_IS_REVERSE                              = 22,
    ACQ_IS_NAVIGATION_DATA                      = 23,
    ACQ_IS_PHASECORR_DATA                       = 24,
    ACQ_LAST_IN_MEASUREMENT                     = 25,
    ACQ_IS_HPFEEDBACK_DATA                      = 26,
    ACQ_IS_DUMMYSCAN_DATA                       = 27,
    ACQ_IS_RTFEEDBACK_DATA                      = 28,
    ACQ_IS_SURFACECOILCORRECTIONSCAN_DATA       = 29,

    ACQ_USER1                                   = 57,
    ACQ_USER2                                   = 58,
    ACQ_USER3                                   = 59,
    ACQ_USER4                                   = 60,
    ACQ_USER5                                   = 61,
    ACQ_USER6                                   = 62,
    ACQ_USER7                                   = 63,
    ACQ_USER8                                   = 64
};

/**
     Struct used for keeping track of typical loop counters in MR experiment.

 */
typedef struct EncodingCounters {
    uint16_t kspace_encode_step_1;     /**< e.g. phase encoding line number */
    uint16_t kspace_encode_step_2;     /**< e.g. partition encodning number */
    uint16_t average;                  /**< e.g. signal average number */
    uint16_t slice;                    /**< e.g. imaging slice number */
    uint16_t contrast;                 /**< e.g. echo number in multi-echo */
    uint16_t phase;                    /**< e.g. cardiac phase number */
    uint16_t repetition;               /**< e.g. dynamic number for dynamic scanning */
    uint16_t set;                      /**< e.g. flow encodning set */
    uint16_t segment;                  /**< e.g. segment number for segmented acquisition */
    uint16_t user[ISMRMRD_USER_INTS];  /**< Free user parameters */
} EncodingCounters;

/**
     Header for each MR acquisition.
 */
typedef struct AcquisitionHeader
{
    uint16_t           version;                                          /**< First unsigned int indicates the version */
    uint64_t           flags;                                            /**< bit field with flags */
    uint32_t           measurement_uid;                                  /**< Unique ID for the measurement */
    uint32_t           scan_counter;                                     /**< Current acquisition number in the measurement */
    uint32_t           acquisition_time_stamp;                           /**< Acquisition clock */
    uint32_t           physiology_time_stamp[ISMRMRD_PHYS_STAMPS];       /**< Physiology time stamps, e.g. ecg, breating, etc. */
    uint16_t           number_of_samples;                                /**< Number of samples acquired */
    uint16_t           available_channels;                               /**< Available coils */
    uint16_t           active_channels;                                  /**< Active coils on current acquisiton */
    uint64_t           channel_mask[ISMRMRD_CHANNEL_MASKS];              /**< Mask to indicate which channels are active. Support for 1024 channels */
    uint16_t           discard_pre;                                      /**< Samples to be discarded at the beginning of acquisition */
    uint16_t           discard_post;                                     /**< Samples to be discarded at the end of acquisition */
    uint16_t           center_sample;                                    /**< Sample at the center of k-space */
    uint16_t           encoding_space_ref;                               /**< Reference to an encoding space, typically only one per acquisition */
    uint16_t           trajectory_dimensions;                            /**< Indicates the dimensionality of the trajectory vector (0 means no trajectory) */
    float              sample_time_us;                                   /**< Time between samples in micro seconds, sampling BW */
    float              position[ISMRMRD_POSITION_LENGTH];                /**< Three-dimensional spatial offsets from isocenter */
    float              read_dir[ISMRMRD_DIRECTION_LENGTH];               /**< Directional cosines of the readout/frequency encoding */
    float              phase_dir[ISMRMRD_DIRECTION_LENGTH];              /**< Directional cosines of the phase */
    float              slice_dir[ISMRMRD_DIRECTION_LENGTH];              /**< Directional cosines of the slice direction */
    float              patient_table_position[ISMRMRD_POSITION_LENGTH];  /**< Patient table off-center */
    EncodingCounters   idx;                                              /**< Encoding loop counters, see above */
    int32_t            user_int[ISMRMRD_USER_INTS];                      /**< Free user parameters */
    float              user_float[ISMRMRD_USER_FLOATS];                  /**< Free user parameters */
} AcquisitionHeader;

enum ImageDataType
{
    DATA_FLOAT = 1,
    DATA_DOUBLE,
    DATA_COMPLEX_FLOAT,
    DATA_COMPLEX_DOUBLE,
    DATA_UNSIGNED_SHORT
};

enum ImageType
{
    TYPE_MAGNITUDE = 1,
    TYPE_PHASE,
    TYPE_REAL,
    TYPE_IMAG,
    TYPE_COMPLEX
};

/** IMAGE FLAGS */
enum ImageFlags {
    IMAGE_IS_NAVIGATION_DATA                      = 23,

    IMAGE_USER1                                   = 57,
    IMAGE_USER2                                   = 58,
    IMAGE_USER3                                   = 59,
    IMAGE_USER4                                   = 60,
    IMAGE_USER5                                   = 61,
    IMAGE_USER6                                   = 62,
    IMAGE_USER7                                   = 63,
    IMAGE_USER8                                   = 64
};

/**
 *  Definition of ISMRM Raw Data Image structure
 */
typedef struct ImageHeader
{
    uint16_t            version;                                         /**< First unsigned int indicates the version */
    uint64_t            flags;                                           /**< bit field with flags */
    uint32_t            measurement_uid;                                 /**< Unique ID for the measurement  */
    uint16_t            matrix_size[3];                                  /**< Pixels in the 3 spatial dimensions */
    float               field_of_view[3];                                /**< Size (in mm) of the 3 spatial dimensions */
    uint16_t            channels;                                        /**< Number of receive channels */
    float               position[ISMRMRD_POSITION_LENGTH];               /**< Three-dimensional spatial offsets from isocenter */
    float               read_dir[ISMRMRD_DIRECTION_LENGTH];              /**< Directional cosines of the readout/frequency encoding */
    float               phase_dir[ISMRMRD_DIRECTION_LENGTH];             /**< Directional cosines of the phase */
    float               slice_dir[ISMRMRD_DIRECTION_LENGTH];             /**< Directional cosines of the slice direction */
    float               patient_table_position[ISMRMRD_POSITION_LENGTH]; /**< Patient table off-center */
    uint16_t            average;                                         /**< e.g. signal average number */
    uint16_t            slice;                                           /**< e.g. imaging slice number */
    uint16_t            contrast;                                        /**< e.g. echo number in multi-echo */
    uint16_t            phase;                                           /**< e.g. cardiac phase number */
    uint16_t            repetition;                                      /**< e.g. dynamic number for dynamic scanning */
    uint16_t            set;                                             /**< e.g. flow encodning set */
    uint32_t            acquisition_time_stamp;                          /**< Acquisition clock */
    uint32_t            physiology_time_stamp[ISMRMRD_PHYS_STAMPS];      /**< Physiology time stamps, e.g. ecg, breating, etc. */
    uint16_t            image_data_type;                                 /**< e.g. unsigned short, float, complex float, etc. */
    uint16_t            image_type;                                      /**< e.g. magnitude, phase, complex, real, imag, etc. */
    uint16_t            image_index;                                     /**< e.g. image number in series of images  */
    uint16_t            image_series_index;                              /**< e.g. series number */
    int32_t             user_int[ISMRMRD_USER_INTS];                     /**< Free user parameters */
    float               user_float[ISMRMRD_USER_FLOATS];                 /**< Free user parameters */
} ImageHeader;

#ifdef __cplusplus
/**
 *  Container for generic array. This structure is used through the HDF5 file interaction.
 */
template <typename T> class NDArrayContainer {

public:
    NDArrayContainer() {}

    /**
     * @brief Construct with dimensions and data
     */
    NDArrayContainer(const std::vector<unsigned int>& dimensions) {
        dimensions_ = dimensions;
        data_.resize(elements());
    }

    /**
     * @brief Construct with dimensions and data
     */
    NDArrayContainer(const std::vector<unsigned int>& dimensions, const T* d) {
        dimensions_ = dimensions;
        data_.resize(elements());
        memcpy(&data_[0],d,sizeof(T)*elements());
    }

    /**
     * @brief Construct with dimensions and preset value
     */
    NDArrayContainer(const std::vector<unsigned int>& dimensions, const std::valarray<T>& t) {
        dimensions_ = dimensions;
        data_.resize(elements());
        data_ = t;
    }

    virtual ~NDArrayContainer() {}

    std::vector<unsigned int> dimensions_; /**< Array with dimensions of the array. First dimension is fastest moving in the array */
    std::valarray<T> data_;               /**< The data itself. A vector is used here for easy memory management                  */

    size_t elements() const {
        if (dimensions_.size() == 0) {
            return 0;
        }
        size_t elements = 1;
        std::vector<unsigned int>::const_iterator it = dimensions_.begin();
        while (it != dimensions_.end()) {
            elements *= *(it++);
        }
        return elements;
    }

    T& operator[] (const size_t& p) {
        return data_[p];
    }

    T operator[] (const size_t& p) const {
        return data_[p];
    }

    void resize (const size_t& s) {
        data_.resize(s);
    }

    void resize (const size_t& s, const T& t) {
        data_.resize(s,t);
    }

    bool is_consistent() const {
        return (elements() == data_.size());
    }

    size_t ndims() const {
        size_t i = 1, n_dimensions = 1;
        for (; i < dimensions_.size(); i++)
            n_dimensions += (size_t) (dimensions_[i] > 1);
        return n_dimensions;
    }

};

/**
 *   Container for an image (header and data)
 */
template <typename T>
class Image {

public:
    Image(unsigned int matrix_size_x = 0,
          unsigned int matrix_size_y = 1,
          unsigned int matrix_size_z = 1,
          unsigned int channels      = 1)
    {
        memset(&head_,0,sizeof(ImageHeader));
        head_.version = ISMRMRD_VERSION;
        head_.matrix_size[0] = matrix_size_x;
        head_.matrix_size[1] = matrix_size_y;
        head_.matrix_size[2] = matrix_size_z;
        head_.channels       = channels;
        setSpecificDataType();
        makeConsistent();
    }


    Image(const Image& a) {   // copy constructor
        if (this != &a) {
            head_ = a.head_;
            this->data_ = a.data_;
            makeConsistent();
        }
    }

    Image& operator=(const Image& a) {
        if (this != &a) {
            head_ = a.head_;
            this->data_ = a.data_;
            makeConsistent();
        }
        return *this;
    }

    ~Image() {}

    bool isFlagSet(FlagBit& f) const {
        return f.isSet(head_.flags);
    }

    void setFlag(FlagBit& f) {
        head_.flags |= f.bitmask_;
    }

    size_t getNumberOfElements() const {
        return head_.matrix_size[0]*
            head_.matrix_size[1]*
            head_.matrix_size[2]*
            head_.channels;
    }


    uint32_t getAcquisitionTimeStamp() const {
        return head_.acquisition_time_stamp;
    }

    void setAcquisitionTimeStamp(uint32_t acquisitionTimeStamp) {
        head_.acquisition_time_stamp = acquisitionTimeStamp;
    }

    uint16_t getAverage() const {
        return head_.average;
    }

    void setAverage(uint16_t average) {
        this->head_.average = average;
    }

    uint16_t getChannels() const {
        return head_.channels;
    }

    void setChannels(uint16_t channels) {
        this->head_.channels = channels;
    }

    uint16_t getContrast() const {
        return head_.contrast;
    }

    void setContrast(uint16_t contrast) {
        this->head_.contrast = contrast;
    }

    float getFieldOfViewX() const {
        return head_.field_of_view[0];
    }

    void setFieldOfViewX(float f) {
        head_.field_of_view[0] = f;
    }

    float getFieldOfViewY() const {
        return head_.field_of_view[1];
    }

    void setFieldOfViewY(float f) {
        head_.field_of_view[1] = f;
    }

    float getFieldOfViewZ() const {
        return head_.field_of_view[2];
    }

    void setFieldOfViewZ(float f) {
        head_.field_of_view[2] = f;
    }

    uint64_t getFlags() const {
        return head_.flags;
    }

    void setFlags(uint64_t flags) {
        this->head_.flags = flags;
    }

    uint16_t getImageDataType() const {
        return head_.image_data_type;
    }

    void setImageDataType(uint16_t imageDataType) {
        head_.image_data_type = imageDataType;
    }

    uint16_t getImageIndex() const {
        return head_.image_index;
    }

    void setImageIndex(uint16_t imageIndex) {
        head_.image_index = imageIndex;
    }

    uint16_t getImageSeriesIndex() const {
        return head_.image_series_index;
    }

    void setImageSeriesIndex(uint16_t imageSeriesIndex) {
        head_.image_series_index = imageSeriesIndex;
    }

    uint16_t getImageType() const {
        return head_.image_type;
    }

    void setImageType(uint16_t imageType) {
        head_.image_type = imageType;
    }

    uint16_t getMatrixSizeX() const {
        return head_.matrix_size[0];
    }

    void setMatrixSizeX(uint16_t s) {
        head_.matrix_size[0] = s;
        makeConsistent();
    }

    uint16_t getMatrixSizeY() const {
        return head_.matrix_size[1];
    }

    void setMatrixSizeY(uint16_t s) {
        head_.matrix_size[1] = s;
        makeConsistent();
    }

    uint16_t getMatrixSizeZ() const {
        return head_.matrix_size[2];
    }

    void setMatrixSizeZ(uint16_t s) {
        head_.matrix_size[2] = s;
        makeConsistent();
    }


    uint32_t getMeasurementUid() const {
        return head_.measurement_uid;
    }

    void setMeasurementUid(uint32_t measurementUid) {
        head_.measurement_uid = measurementUid;
    }

    const float getPatientTablePosition(unsigned int axis) const {
        if (axis < ISMRMRD_POSITION_LENGTH) {
            return head_.patient_table_position[axis];
        }
        return 0;
    }

    void setPatientTablePosition(unsigned int axis, float value) {
        if (axis < ISMRMRD_POSITION_LENGTH) {
            head_.patient_table_position[axis] = value;
        }
    }

    const uint32_t getPhysiologyTimeStamp(unsigned int stamp_id) const {
        if (stamp_id < ISMRMRD_PHYS_STAMPS) {
            return head_.physiology_time_stamp[stamp_id];
        }
        return 0;
    }

    void setPhysiologyTimeStamp(unsigned int stamp_id, uint32_t value){
        if (stamp_id < ISMRMRD_PHYS_STAMPS) {
            head_.physiology_time_stamp[stamp_id] = value;
        }
    }

    const float getPosition(unsigned int axis) const {
        if (axis < ISMRMRD_POSITION_LENGTH) {
            return head_.position[axis];
        }
        return 0;
    }

    void setPosition(unsigned int axis, float value){
        if (axis < ISMRMRD_POSITION_LENGTH) {
            head_.position[axis] = value;
        }
    }

        const float getReadDirection(unsigned int index) const {
                if (index < ISMRMRD_DIRECTION_LENGTH) {
                        return head_.read_dir[index];
                }
                return 0;
        }

        void setReadDirection(unsigned int index, float value) {
                if (index < ISMRMRD_DIRECTION_LENGTH) {
                        head_.read_dir[index] = value;
                }
        }

        const float getPhaseDirection(unsigned int index) const {
                if (index < ISMRMRD_DIRECTION_LENGTH) {
                        return head_.phase_dir[index];
                }
                return 0;
        }

        void setPhaseDirection(unsigned int index, float value) {
                if (index < ISMRMRD_DIRECTION_LENGTH) {
                        head_.phase_dir[index] = value;
                }
        }

        const float getSliceDirection(unsigned int index) const {
                if (index < ISMRMRD_DIRECTION_LENGTH) {
                        return head_.slice_dir[index];
                }
                return 0;
        }

        void setSliceDirection(unsigned int index, float value) {
                if (index < ISMRMRD_DIRECTION_LENGTH) {
                        head_.slice_dir[index] = value;
                }
        }

    uint16_t getPhase() const {
        return head_.phase;
    }

    void setPhase(uint16_t phase) {
        this->head_.phase = phase;
    }


    uint16_t getRepetition() const {
        return head_.repetition;
    }

    void setRepetition(uint16_t repetition) {
        this->head_.repetition = repetition;
    }

    uint16_t getSet() const {
        return head_.set;
    }

    void setSet(uint16_t set) {
        this->head_.set = set;
    }

    uint16_t getSlice() const {
        return head_.slice;
    }

    void setSlice(uint16_t slice) {
        this->head_.slice = slice;
    }

    const float getUserFloat(unsigned int index) const {
        if (index < ISMRMRD_USER_FLOATS) {
            return head_.user_float[index];
        }
        return 0.0f;
    }

    void setUserFloat(unsigned int index, float value) {
        if (index < ISMRMRD_USER_FLOATS) {
            head_.user_float[index] = value;
        }
    }

    const int32_t getUserInt(unsigned int index) const {
        if (index < ISMRMRD_USER_INTS) {
            return head_.user_int[index];
        }
        return 0;
    }

    void setUserInt(unsigned int index, int32_t value) {
        if (index < ISMRMRD_USER_INTS) {
            head_.user_int[index] = value;
        }
    }

    uint16_t getVersion() const {
        return head_.version;
    }

    void setVersion(uint16_t version) {
        this->head_.version = version;
    }

    const std::valarray<T>& getData() const {
        return data_;
    }

    void setData(const std::valarray<T>& data) {
        data_ = data;
    }

    const ImageHeader& getHead() const {
        return head_;
    }

    void setHead(const ImageHeader& head) {
        head_ = head;
        makeConsistent();
    }

    T& operator[] (const size_t& p) {
        return data_[p];
    }

    T operator[] (const size_t& p) const {
        return data_[p];
    }


protected:
    ImageHeader head_;     /**< ImageHeader as defined above */
    std::valarray<T> data_;
    void makeConsistent() {
        if (getNumberOfElements() != data_.size()) {
            data_.resize(getNumberOfElements());
        }
    }

    void setSpecificDataType();

};

template <> inline void Image<float>::setSpecificDataType() {
    head_.image_data_type = DATA_FLOAT;
}

template <> inline void Image<double>::setSpecificDataType() {
    head_.image_data_type = DATA_DOUBLE;
}

template <> inline void Image< std::complex<float> >::setSpecificDataType() {
    head_.image_data_type = DATA_COMPLEX_FLOAT;
}

template <> inline void Image< std::complex<double> >::setSpecificDataType() {
    head_.image_data_type = DATA_COMPLEX_DOUBLE;
}

template <> inline void Image< unsigned short >::setSpecificDataType() {
    head_.image_data_type = DATA_UNSIGNED_SHORT;
}



/**
 * @brief Single acquisition
 */

/* TODO:
 *    - Move data and traj to protected
 *    - Check data and traj sizes when assigning number of samples, active channels, traj dimensions
 *    - Check active channels when assigning channel bits
 *    - Assign channel bits when assiging number of channels
 *    - Change Image to have encapsulation
 *    - Make Image Data Type Consistent with field.
 *    - Test, test, test.
 */
class Acquisition {

public:
    Acquisition()
        : traj_(), data_() {
        memset(&head_,0,sizeof(AcquisitionHeader));
        head_.version = ISMRMRD_VERSION;
    }

    ~Acquisition() {}

    Acquisition (const Acquisition& a) {   // copy constructor
        if (this != &a) {
            head_ = a.head_;
            data_.resize(a.data_.size());
            memcpy(&data_[0],&a.data_[0],sizeof(float)*a.data_.size());
            traj_.resize(a.traj_.size());
            memcpy(&traj_[0],&a.traj_[0],sizeof(float)*a.traj_.size());
        }
    }

    Acquisition& operator=(const Acquisition& a) {
        if (this != &a) {
            head_ = a.head_;
            data_.resize(a.data_.size());
            memcpy(&data_[0],&a.data_[0],sizeof(float)*a.data_.size());
            traj_.resize(a.traj_.size());
            memcpy(&traj_[0],&a.traj_[0],sizeof(float)*a.traj_.size());
        }
        return *this;
    }

    const AcquisitionHeader& getHead() {
        return head_;
    }

    void setHead(const AcquisitionHeader& h) {
        head_ = h;
        makeConsistent();
    }

    bool isFlagSet(const FlagBit& f) const {
        return f.isSet(head_.flags);
    }

    void setFlag(const FlagBit& f) {
        head_.flags |= f.bitmask_;
    }

    uint32_t getAcquisitionTimeStamp() const {
        return head_.acquisition_time_stamp;
    }

    void setAcquisitionTimeStamp(uint32_t acquisitionTimeStamp) {
        head_.acquisition_time_stamp = acquisitionTimeStamp;
    }

    uint16_t getActiveChannels() const {
        return head_.active_channels;
    }

    void setActiveChannels(uint16_t activeChannels) {
        head_.active_channels = activeChannels;
        makeConsistent();
    }

    uint16_t getAvailableChannels() const {
        return head_.available_channels;
    }

    void setAvailableChannels(uint16_t availableChannels) {
        head_.available_channels = availableChannels;
    }

    uint16_t getCenterSample() const {
        return head_.center_sample;
    }

    void setCenterSample(uint16_t centerSample) {
        head_.center_sample = centerSample;
    }

    bool isChannelActive(unsigned int channel_id) {
        if (channel_id < 64*ISMRMRD_CHANNEL_MASKS) {
            unsigned int mask_idx = channel_id>>6;
            unsigned int mask_bit = channel_id-mask_idx*64;
            return ((head_.channel_mask[mask_idx] & ( (uint64_t)1 << mask_bit)) > 0);
        }
        return false;
    }

    void setChannelActive(unsigned int channel_id) {
        if (channel_id < 64*ISMRMRD_CHANNEL_MASKS) {
            unsigned int mask_idx = channel_id>>6;
            unsigned int mask_bit = channel_id-mask_idx*64;
            head_.channel_mask[mask_idx] |= ( (uint64_t)1 << mask_bit);
        }
    }

    unsigned int getActiveChannelBits() {
        unsigned int bits = 0;
        for (unsigned int i = 0; i < ISMRMRD_CHANNEL_MASKS*64; i++) {
            if (isChannelActive(i)) {
                bits++;
            }
        }
        return bits;
    }

    uint16_t getDiscardPost() const {
        return head_.discard_post;
    }

    void setDiscardPost(uint16_t discardPost) {
        head_.discard_post = discardPost;
    }

    uint16_t getDiscardPre() const {
        return head_.discard_pre;
    }

    void setDiscardPre(uint16_t discardPre) {
        head_.discard_pre = discardPre;
    }

    uint16_t getEncodingSpaceRef() const {
        return head_.encoding_space_ref;
    }

    void setEncodingSpaceRef(uint16_t encodingSpaceRef) {
        head_.encoding_space_ref = encodingSpaceRef;
    }

    uint64_t getFlags() const {
        return head_.flags;
    }

    void setFlags(uint64_t flags) {
        this->head_.flags = flags;
    }

    EncodingCounters& getIdx() {
        return head_.idx;
    }

    void setIdx(const EncodingCounters& idx) {
        this->head_.idx = idx;
    }

    uint32_t getMeasurementUid() const {
        return head_.measurement_uid;
    }

    void setMeasurementUid(uint32_t measurementUid) {
        head_.measurement_uid = measurementUid;
    }

    uint16_t getNumberOfSamples() const {
        return head_.number_of_samples;
    }

    void setNumberOfSamples(uint16_t numberOfSamples) {
        head_.number_of_samples = numberOfSamples;
        makeConsistent();
    }

    const float getPatientTablePosition(unsigned int axis) const {
        if (axis < ISMRMRD_POSITION_LENGTH) {
            return head_.patient_table_position[axis];
        }
        return 0;
    }

    void setPatientTablePosition(unsigned int axis, float value) {
        if (axis < ISMRMRD_POSITION_LENGTH) {
            head_.patient_table_position[axis] = value;
        }
    }

    const uint32_t getPhysiologyTimeStamp(unsigned int stamp_id) const {
        if (stamp_id < ISMRMRD_PHYS_STAMPS) {
            return head_.physiology_time_stamp[stamp_id];
        }
        return 0;
    }

    void setPhysiologyTimeStamp(unsigned int stamp_id, uint32_t value){
        if (stamp_id < ISMRMRD_PHYS_STAMPS) {
            head_.physiology_time_stamp[stamp_id] = value;
        }
    }

    const float getPosition(unsigned int axis) const {
        if (axis < ISMRMRD_POSITION_LENGTH) {
            return head_.position[axis];
        }
        return 0;
    }

    void setPosition(unsigned int axis, float value){
        if (axis < ISMRMRD_POSITION_LENGTH) {
            head_.position[axis] = value;
        }
    }

        const float getReadDirection(unsigned int index) const {
                if (index < ISMRMRD_DIRECTION_LENGTH) {
                        return head_.read_dir[index];
                }
                return 0;
        }

        void setReadDirection(unsigned int index, float value) {
                if (index < ISMRMRD_DIRECTION_LENGTH) {
                        head_.read_dir[index] = value;
                }
        }

        const float getPhaseDirection(unsigned int index) const {
                if (index < ISMRMRD_DIRECTION_LENGTH) {
                        return head_.phase_dir[index];
                }
                return 0;
        }

        void setPhaseDirection(unsigned int index, float value) {
                if (index < ISMRMRD_DIRECTION_LENGTH) {
                        head_.phase_dir[index] = value;
                }
        }

        const float getSliceDirection(unsigned int index) const {
                if (index < ISMRMRD_DIRECTION_LENGTH) {
                        return head_.slice_dir[index];
                }
                return 0;
        }

        void setSliceDirection(unsigned int index, float value) {
                if (index < ISMRMRD_DIRECTION_LENGTH) {
                        head_.slice_dir[index] = value;
                }
        }

    float getSampleTimeUs() const {
        return head_.sample_time_us;
    }

    void setSampleTimeUs(float sampleTimeUs) {
        head_.sample_time_us = sampleTimeUs;
    }

    uint32_t getScanCounter() const {
        return head_.scan_counter;
    }

    void setScanCounter(uint32_t scanCounter) {
        head_.scan_counter = scanCounter;
    }

    uint16_t getTrajectoryDimensions() const {
        return head_.trajectory_dimensions;
    }

    void setTrajectoryDimensions(uint16_t trajectoryDimensions) {
        head_.trajectory_dimensions = trajectoryDimensions;
        makeConsistent();
    }

    const float getUserFloat(unsigned int index) const {
        if (index < ISMRMRD_USER_FLOATS) {
            return head_.user_float[index];
        }
        return 0.0f;
    }

    void setUserFloat(unsigned int index, float value) {
        if (index < ISMRMRD_USER_FLOATS) {
            head_.user_float[index] = value;
        }
    }

    const int32_t getUserInt(unsigned int index) const {
        if (index < ISMRMRD_USER_INTS) {
            return head_.user_int[index];
        }
        return 0;
    }

    void setUserInt(unsigned int index, int32_t value) {
        if (index < ISMRMRD_USER_INTS) {
            head_.user_int[index] = value;
        }
    }

    uint16_t getVersion() const {
        return head_.version;
    }

    void setVersion(uint16_t version) {
        this->head_.version = version;
    }

    const std::valarray<float>& getData() const {
        return data_;
    }

    void setData(const std::valarray<float>& data) {
        data_ = data;
    }

    const std::valarray<float>& getTraj() const {
        return traj_;
    }

    void setTraj(const std::valarray<float>& traj) {
        traj_ = traj;
    }

    float& operator[] (const size_t& p) {
        return data_[p];
    }

    float operator[] (const size_t& p) const {
        return data_[p];
    }

protected:
    size_t calcTrajLength() {
        return head_.trajectory_dimensions*head_.number_of_samples;
    }

    size_t calcDataLength() {
        return head_.active_channels*head_.number_of_samples*2;
    }


    void makeConsistent() {
        if (head_.active_channels > head_.available_channels) {
            head_.available_channels = head_.active_channels;
        }
        if (calcTrajLength() != traj_.size()) {
            traj_.resize(calcTrajLength());
        }
        if (calcDataLength() != data_.size()) {
            data_.resize(calcDataLength());
        }
    }

    AcquisitionHeader head_; /**< Header, see above */
    std::valarray<float> traj_;
    std::valarray<float> data_;

};

#endif //__cplusplus

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Calculates the determinant of the matrix and return the sign
 */
static int sign_of_directions(float read_dir[3], float phase_dir[3], float slice_dir[3])
{
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

/**
 * Creates a normalized quaternion from a 3x3 rotation matrix
 */
static void directions_to_quaternion(float read_dir[3], float phase_dir[3],
        float slice_dir[3], float quat[4])
{
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
    trace = 1.0l + r11 + r22 + r33;
    if (trace > 0.00001l) {                /* simplest case */
        s = sqrt(trace) * 2;
        a = (r32 - r23) / s;
        b = (r13 - r31) / s;
        c = (r21 - r12) / s;
        d = 0.25l * s;
    } else {
        /* trickier case...
         * determine which major diagonal element has
         * the greatest value... */
        xd = 1.0 + r11 - (r22 + r33);  /* 4**b**b */
        yd = 1.0 + r22 - (r11 + r33);  /* 4**c**c */
        zd = 1.0 + r33 - (r11 + r22);  /* 4**d**d */
        /* if r11 is the greatest */
        if (xd > 1.0) {
            s = 2.0 * sqrt(xd);
            a = 0.25l * s;
            b = (r21 + r12) / s;
            c = (r31 + r13) / s;
            d = (r32 - r23) / s;
        }
        /* else if r22 is the greatest */
        else if (yd > 1.0) {
            s = 2.0 * sqrt(yd);
            a = (r21 + r12) / s;
            b = 0.25l * s;
            c = (r32 + r23) / s;
            d = (r13 - r31) / s;
        }
        /* else, r33 must be the greatest */
        else {
            s = 2.0 * sqrt(zd);
            a = (r13 + r31) / s;
            b = (r23 + r32) / s;
            c = 0.25l * s;
            d = (r21 - r12) / s;
        }

        if (a < 0.0l) {
            b = -b;
            c = -c;
            d = -d;
            a = -a;
        }
    }

    quat[0] = (float)a; quat[1] = (float)b; quat[2] = (float)c; quat[3] = (float)d;
}

/**
 * Converts a quaternion of the form | a b c d | to a
 * 3x3 rotation matrix
 *
 * http://www.cs.princeton.edu/~gewang/projects/darth/stuff/quat_faq.html#Q54
 */
static void quaternion_to_directions(float quat[4], float read_dir[3],
        float phase_dir[3], float slice_dir[3])
{
    float a = quat[0], b = quat[1], c = quat[2], d = quat[3];

    read_dir[0]  = 1.0f - 2.0f * ( b*b + c*c );
    phase_dir[0] = 2.0f * ( a*b - c*d );
    slice_dir[0] = 2.0f * ( a*c + b*d );

    read_dir[1]  = 2.0f * ( a*b + c*d );
    phase_dir[1] = 1.0f - 2.0f * ( a*a + c*c );
    slice_dir[1] = 2.0f * ( b*c - a*d );

    read_dir[2]  = 2.0f * ( a*c - b*d );
    phase_dir[2] = 2.0f * ( b*c + a*d );
    slice_dir[2] = 1.0f - 2.0f * ( a*a + b*b );
}

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
} //End of ISMRMRD namespace
#endif //__cplusplus

#pragma pack(pop) //Restore old alignment

#endif //ISMRMRD_H
