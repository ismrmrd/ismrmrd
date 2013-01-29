/* ISMRMRD MR Raw Data Strutures                           */
/* DRAFT - July 2012                                       */

/* Authors:                                                */
/*    Michael S. Hansen (michael.hansen@nih.gov)           */
/*    Brian Hargreaves  (bah@stanford.edu)                 */
/*    Sebastian Kozerke (kozerke@biomed.ee.ethz.ch)        */
/*    Kaveh Vahedipour  (k.vahedipour@fz-juelich.de)       */

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
#endif
#endif

#ifdef __cplusplus
#include <stdio.h>
#include <string.h>
#include <exception>
#include <iostream>
#include <vector>
#include <valarray>
#include <assert.h>
#endif

#pragma pack(push, 2) //Use 2 byte alignment

#define ISMRMRD_VERSION 1

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
	ACQ_FIRST_IN_ENCODE_STEP1                	= 1,
	ACQ_LAST_IN_ENCODE_STEP1    				= 2,
	ACQ_FIRST_IN_ENCODE_STEP2   				= 3,
	ACQ_LAST_IN_ENCODE_STEP2    				= 4,
	ACQ_FIRST_IN_AVERAGE        				= 5,
	ACQ_LAST_IN_AVERAGE         				= 6,
	ACQ_FIRST_IN_SLICE          				= 7,
	ACQ_LAST_IN_SLICE           				= 8,
	ACQ_FIRST_IN_CONTRAST       				= 9,
	ACQ_LAST_IN_CONTRAST        				= 10,
	ACQ_FIRST_IN_PHASE          				= 11,
	ACQ_LAST_IN_PHASE           				= 12,
	ACQ_FIRST_IN_REPETITION     				= 13,
	ACQ_LAST_IN_REPETITION      				= 14,
	ACQ_FIRST_IN_SET            				= 15,
	ACQ_LAST_IN_SET             				= 16,
	ACQ_FIRST_IN_SEGMENT        				= 17,
	ACQ_LAST_IN_SEGMENT         				= 18,

	ACQ_IS_NOISE_MEASUREMENT                	= 19,
	ACQ_IS_PARALLEL_CALIBRATION             	= 20,
	ACQ_IS_PARALLEL_CALIBRATION_AND_IMAGING 	= 21,
	ACQ_IS_REVERSE              				= 22,
	ACQ_IS_NAVIGATION_DATA      				= 23,

	ACQ_USER1                   				= 57,
	ACQ_USER2                   				= 58,
	ACQ_USER3                   				= 59,
	ACQ_USER4                   				= 60,
	ACQ_USER5                   				= 61,
	ACQ_USER6                   				= 62,
	ACQ_USER7                   				= 63,
	ACQ_USER8                   				= 64
};


/**
     Struct used for keeping track of typical loop counters in MR experiment.

 */
struct EncodingCounters {
	uint16_t kspace_encode_step_1; /**< e.g. phase encoding line number */
	uint16_t kspace_encode_step_2; /**< e.g. partition encodning number */
	uint16_t average;              /**< e.g. signal average number */
	uint16_t slice;                /**< e.g. imaging slice number */
	uint16_t contrast;             /**< e.g. echo number in multi-echo */
	uint16_t phase;                /**< e.g. cardiac phase number */
	uint16_t repetition;           /**< e.g. dynamic number for dynamic scanning */
	uint16_t set;                  /**< e.g. flow encodning set */
	uint16_t segment;              /**< e.g. segment number for segmented acquisition */
	uint16_t user[8];              /**< Free user parameters */
};

/**
     Header for each MR acquisition. 
 */
struct AcquisitionHeader
{
	uint16_t           version;                        /**< First unsigned int indicates the version */
	uint64_t           flags;                          /**< bit field with flags */
	uint32_t           measurement_uid;                /**< Unique ID for the measurement */
	uint32_t           scan_counter;                   /**< Current acquisition number in the measurement */
	uint32_t           acquisition_time_stamp;         /**< Acquisition clock */
	uint32_t           physiology_time_stamp[3];       /**< Physiology time stamps, e.g. ecg, breating, etc. */
	uint16_t           number_of_samples;              /**< Number of samples acquired */
	uint16_t           available_channels;             /**< Available coils */
	uint16_t           active_channels;                /**< Active coils on current acquisiton */
	uint64_t           channel_mask[16];               /**< Mask to indicate which channels are active. Support for 1024 channels */
	uint16_t           discard_pre;                    /**< Samples to be discarded at the beginning of acquisition */
	uint16_t           discard_post;                   /**< Samples to be discarded at the end of acquisition */
	uint16_t           center_sample;                  /**< Sample at the center of k-space */
	uint16_t           encoding_space_ref;             /**< Reference to an encoding space, typically only one per acquisition */
	uint16_t           trajectory_dimensions;          /**< Indicates the dimensionality of the trajectory vector (0 means no trajectory) */
	float              sample_time_us;                 /**< Time between samples in micro seconds, sampling BW */
	float              position[3];                    /**< Three-dimensional spatial offsets from isocenter */
	float              quaternion[4];                  /**< Angulation of acquisition */
	float              patient_table_position[3];      /**< Patient table off-center */
	EncodingCounters   idx;                            /**< Encoding loop counters, see above */
	int32_t            user_int[8];                    /**< Free user parameters */
	float              user_float[8];                  /**< Free user parameters */
};

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
	IMAGE_IS_NAVIGATION_DATA      				= 23,

	IMAGE_USER1                   				= 57,
	IMAGE_USER2                   				= 58,
	IMAGE_USER3                   				= 59,
	IMAGE_USER4                   				= 60,
	IMAGE_USER5                   				= 61,
	IMAGE_USER6                   				= 62,
	IMAGE_USER7                   				= 63,
	IMAGE_USER8                   				= 64
};

/**
 *  Definition of ISMRM Raw Data Image structure
 */
struct ImageHeader
{
	uint16_t           	version;                        /**< First unsigned int indicates the version */
	uint64_t           	flags;                          /**< bit field with flags */
	uint32_t           	measurement_uid;                /**< Unique ID for the measurement  */
	uint16_t           	matrix_size[3];                 /**< Pixels in the 3 spatial dimensions */
	float              	field_of_view[3];               /**< Size (in mm) of the 3 spatial dimensions */
	uint16_t           	channels;                       /**< Number of receive channels */
	float              	position[3];                    /**< Three-dimensional spatial offsets from isocenter */
	float              	quaternion[4];                  /**< Angulation of acquisition */
	float              	patient_table_position[3];      /**< Patient table off-center */
	uint16_t           	average;                        /**< e.g. signal average number */
	uint16_t           	slice;                          /**< e.g. imaging slice number */
	uint16_t           	contrast;                       /**< e.g. echo number in multi-echo */
	uint16_t           	phase;                          /**< e.g. cardiac phase number */
	uint16_t           	repetition;                     /**< e.g. dynamic number for dynamic scanning */
	uint16_t           	set;                            /**< e.g. flow encodning set */
	uint32_t           	acquisition_time_stamp;         /**< Acquisition clock */
	uint32_t           	physiology_time_stamp[3];       /**< Physiology time stamps, e.g. ecg, breating, etc. */
	uint16_t           	image_data_type;                /**< e.g. unsigned short, float, complex float, etc. */
	uint16_t           	image_type;                     /**< e.g. magnitude, phase, complex, real, imag, etc. */
	uint16_t           	image_index;					/**< e.g. image number in series of images  */
	uint16_t			image_series_index;             /**< e.g. series number */
	int32_t            	user_int[8];                    /**< Free user parameters */
	float              	user_float[8];                  /**< Free user parameters */
};

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
	NDArrayContainer(const std::vector<unsigned int>& dimensions, const T& t) {
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
class Image : public NDArrayContainer<T> {
    
public:
	Image() :
        NDArrayContainer<T>() {
		memset(&head_,0,sizeof(ImageHeader));
		head_.version = ISMRMRD_VERSION;
	}
    
	Image (const std::vector<unsigned int>& dimensions, T* d) :
        NDArrayContainer<T> (dimensions, d) {
        head_.matrix_size[0] = dimensions[0];
        head_.matrix_size[1] = dimensions[1];
        head_.matrix_size[2] = dimensions[2];
        head_.channels       = dimensions[3];
	}
    
	Image (const std::vector<unsigned int>& dimensions, T t = T(0)) :
        NDArrayContainer<T> (dimensions, t) {
        head_.matrix_size[0] = dimensions[0];
        head_.matrix_size[1] = dimensions[1];
        head_.matrix_size[2] = dimensions[2];
        head_.channels       = dimensions[3];
	}
    
    Image(const Image& a) {   // copy constructor
		if (this != &a) {
            head_ = a.head_;
            this->data_ = a.data_;
		}
	}

	Image& operator=(const Image& a) {
		if (this != &a) {
            head_ = a.head_;
            this->data_ = a.data_;
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

	ImageHeader head_;     /**< ImageHeader as defined above */
    static const unsigned short MAX_IMAGE_DIMS;

};

/**
 * @brief Single acquisition
 */
class Acquisition : public NDArrayContainer<float> {
    
public:
    
	Acquisition()
        : traj_(), data_() {
		memset(&head_,0,sizeof(AcquisitionHeader));
		head_.version = ISMRMRD_VERSION;
	}
    
	~Acquisition() {}
    
	bool isFlagSet(const FlagBit& f) const {
		return f.isSet(head_.flags);
	}
    
	void setFlag(const FlagBit& f) {
		head_.flags |= f.bitmask_;
	}
    
	Acquisition (const Acquisition& a) {   // copy constructor
		if (this != &a) {
			head_ = a.head_;
            traj_ = a.traj_;
            data_ = a.data_;
            /* TODO: Consistency checks*/
		}
	}
    
	Acquisition& operator=(const Acquisition& a) {
		if (this != &a) {
            head_ = a.head_;
            traj_ = a.traj_;
            data_ = a.data_;
		}
		return *this;
	}
	
	AcquisitionHeader head_; /**< Header, see above */
    
    std::valarray<float> traj_;
    std::valarray<float> data_;
    
};
#endif //__cplusplus


#ifdef __cplusplus
} //End of ISMRMRD namespace
#endif //__cplusplus

#pragma pack(pop) //Restore old alignment

#endif //ISMRMRD_H
