/* ISMRMRD MR Raw Data Strutures                           */
/* DRAFT                                                   */
/* Authors:                                                */
/*    Michael S. Hansen (michael.hansen@nih.gov)           */
/*    Brian Hargreaves  (bah@stanford.edu)                 */
/*    Sebastian Kozerke (kozerke@biomed.ee.ethz.ch)        */
/*    Kaveh Vahedipour  (k.vahedipour@fz-juelich.de)       */
/*    Hui Xue           (hui.xue@nih.gov)                  */
/*    Souheil Inati     (souheil.inati@nih.gov)            */
/*    Joseph Naegele    (joseph.naegele@nih.gov)           */

// clang-format off

/**
 * @file ismrmrd.h
 * @defgroup capi C API
 * @defgroup cxxapi C++ API
 */

#pragma once
#ifndef ISMRMRD_H
#define ISMRMRD_H

/* Language and cross platform section for defining types */
/* integers */
#include <stdint.h>
#include <stddef.h>     /* for size_t */

#if __cplusplus > 199711L
#include <type_traits>
#endif

/* Unaligned */
#ifdef _MSC_VER
#define ISMRMRD_UNALIGNED __unaligned
#else
#define ISMRMRD_UNALIGNED
#endif

/* Complex numbers */
#ifdef __cplusplus
#include <complex>
typedef std::complex<float> complex_float_t;
typedef std::complex<double> complex_double_t;
#else
#ifdef _MSC_VER /* MS C compiler */
typedef struct complex_float_t{
    float real;
    float imag;
}complex_float_t;
typedef struct complex_double_t{
    double real;
    double imag;
}complex_double_t;
#else /* C99 compiler */
#include <complex.h>
typedef float complex complex_float_t;
typedef double complex complex_double_t;
#endif /* _MSC_VER */
#endif /* __cplusplus */

/* Booleans - part of C++ */
#ifndef __cplusplus
#if defined (_MSC_VER) && (_MSC_VER < 1800) /* old MS C compiler */
typedef int bool;
#define false 0
#define true 1
#else /* C99 compiler */
#include <stdbool.h>
#endif /* _MSC_VER */
#endif /* __cplusplus */

/* Vectors */
#ifdef __cplusplus
#include <vector>
#endif /* __cplusplus */

/* Exports needed for MS C++ */
#include "ismrmrd/export.h"

#pragma pack(push, 2) /* Use 2 byte alignment */

#ifdef __cplusplus
namespace ISMRMRD {
extern "C" {
#endif

/**
 * Constants
 */
enum ISMRMRD_Constants {
    ISMRMRD_USER_INTS = 8,
    ISMRMRD_USER_FLOATS = 8,
    ISMRMRD_PHYS_STAMPS = 3,
    ISMRMRD_CHANNEL_MASKS = 16,
    ISMRMRD_NDARRAY_MAXDIM = 7,
    ISMRMRD_POSITION_LENGTH = 3,
    ISMRMRD_DIRECTION_LENGTH = 3
};


/**
 * Constants
 */
enum ISMRMRD_ErrorCodes {
    ISMRMRD_BEGINERROR=-1,
    ISMRMRD_NOERROR,
    ISMRMRD_MEMORYERROR,
    ISMRMRD_FILEERROR,
    ISMRMRD_TYPEERROR,
    ISMRMRD_RUNTIMEERROR,
    ISMRMRD_HDF5ERROR,
    ISMRMRD_ENDERROR
};

/**
 * Data Types
 */
enum ISMRMRD_DataTypes {
    ISMRMRD_USHORT   = 1, /**< corresponds to uint16_t */
    ISMRMRD_SHORT    = 2, /**< corresponds to int16_t */
    ISMRMRD_UINT     = 3, /**< corresponds to uint32_t */
    ISMRMRD_INT      = 4, /**< corresponds to int32_t */
    ISMRMRD_FLOAT    = 5, /**< corresponds to float */
    ISMRMRD_DOUBLE   = 6, /**< corresponds to double */
    ISMRMRD_CXFLOAT  = 7, /**< corresponds to complex float */
    ISMRMRD_CXDOUBLE = 8  /**< corresponds to complex double */
};

/** Returns the size in bytes of an ISMRMRD_DataType */
EXPORTISMRMRD size_t ismrmrd_sizeof_data_type(int data_type);

/**
 * Acquisition Flags
 */
enum ISMRMRD_AcquisitionFlags {
    ISMRMRD_ACQ_FIRST_IN_ENCODE_STEP1               =  1,
    ISMRMRD_ACQ_LAST_IN_ENCODE_STEP1                =  2,
    ISMRMRD_ACQ_FIRST_IN_ENCODE_STEP2               =  3,
    ISMRMRD_ACQ_LAST_IN_ENCODE_STEP2                =  4,
    ISMRMRD_ACQ_FIRST_IN_AVERAGE                    =  5,
    ISMRMRD_ACQ_LAST_IN_AVERAGE                     =  6,
    ISMRMRD_ACQ_FIRST_IN_SLICE                      =  7,
    ISMRMRD_ACQ_LAST_IN_SLICE                       =  8,
    ISMRMRD_ACQ_FIRST_IN_CONTRAST                   =  9,
    ISMRMRD_ACQ_LAST_IN_CONTRAST                    = 10,
    ISMRMRD_ACQ_FIRST_IN_PHASE                      = 11,
    ISMRMRD_ACQ_LAST_IN_PHASE                       = 12,
    ISMRMRD_ACQ_FIRST_IN_REPETITION                 = 13,
    ISMRMRD_ACQ_LAST_IN_REPETITION                  = 14,
    ISMRMRD_ACQ_FIRST_IN_SET                        = 15,
    ISMRMRD_ACQ_LAST_IN_SET                         = 16,
    ISMRMRD_ACQ_FIRST_IN_SEGMENT                    = 17,
    ISMRMRD_ACQ_LAST_IN_SEGMENT                     = 18,
    ISMRMRD_ACQ_IS_NOISE_MEASUREMENT                = 19,
    ISMRMRD_ACQ_IS_PARALLEL_CALIBRATION             = 20,
    ISMRMRD_ACQ_IS_PARALLEL_CALIBRATION_AND_IMAGING = 21,
    ISMRMRD_ACQ_IS_REVERSE                          = 22,
    ISMRMRD_ACQ_IS_NAVIGATION_DATA                  = 23,
    ISMRMRD_ACQ_IS_PHASECORR_DATA                   = 24,
    ISMRMRD_ACQ_LAST_IN_MEASUREMENT                 = 25,
    ISMRMRD_ACQ_IS_HPFEEDBACK_DATA                  = 26,
    ISMRMRD_ACQ_IS_DUMMYSCAN_DATA                   = 27,
    ISMRMRD_ACQ_IS_RTFEEDBACK_DATA                  = 28,
    ISMRMRD_ACQ_IS_SURFACECOILCORRECTIONSCAN_DATA   = 29,
    ISMRMRD_ACQ_IS_PHASE_STABILIZATION_REFERENCE    = 30,
    ISMRMRD_ACQ_IS_PHASE_STABILIZATION              = 31,


    ISMRMRD_ACQ_COMPRESSION1                        = 53,
    ISMRMRD_ACQ_COMPRESSION2                        = 54,
    ISMRMRD_ACQ_COMPRESSION3                        = 55,
    ISMRMRD_ACQ_COMPRESSION4                        = 56,
    ISMRMRD_ACQ_USER1                               = 57,
    ISMRMRD_ACQ_USER2                               = 58,
    ISMRMRD_ACQ_USER3                               = 59,
    ISMRMRD_ACQ_USER4                               = 60,
    ISMRMRD_ACQ_USER5                               = 61,
    ISMRMRD_ACQ_USER6                               = 62,
    ISMRMRD_ACQ_USER7                               = 63,
    ISMRMRD_ACQ_USER8                               = 64
};

/**
 * Image Types
 */
enum ISMRMRD_ImageTypes {
    ISMRMRD_IMTYPE_MAGNITUDE = 1,
    ISMRMRD_IMTYPE_PHASE     = 2,
    ISMRMRD_IMTYPE_REAL      = 3,
    ISMRMRD_IMTYPE_IMAG      = 4,
    ISMRMRD_IMTYPE_COMPLEX   = 5
};

/**
 * Image Flags
 */
enum ISMRMRD_ImageFlags {
    ISMRMRD_IMAGE_IS_NAVIGATION_DATA  =  1,
    ISMRMRD_IMAGE_FIRST_IN_AVERAGE    =  5,
    ISMRMRD_IMAGE_LAST_IN_AVERAGE     =  6,
    ISMRMRD_IMAGE_FIRST_IN_SLICE      =  7,
    ISMRMRD_IMAGE_LAST_IN_SLICE       =  8,
    ISMRMRD_IMAGE_FIRST_IN_CONTRAST   =  9,
    ISMRMRD_IMAGE_LAST_IN_CONTRAST    = 10,
    ISMRMRD_IMAGE_FIRST_IN_PHASE      = 11,
    ISMRMRD_IMAGE_LAST_IN_PHASE       = 12,
    ISMRMRD_IMAGE_FIRST_IN_REPETITION = 13,
    ISMRMRD_IMAGE_LAST_IN_REPETITION  = 14,
    ISMRMRD_IMAGE_FIRST_IN_SET        = 15,
    ISMRMRD_IMAGE_LAST_IN_SET         = 16,
    ISMRMRD_IMAGE_USER1               = 57,
    ISMRMRD_IMAGE_USER2               = 58,
    ISMRMRD_IMAGE_USER3               = 59,
    ISMRMRD_IMAGE_USER4               = 60,
    ISMRMRD_IMAGE_USER5               = 61,
    ISMRMRD_IMAGE_USER6               = 62,
    ISMRMRD_IMAGE_USER7               = 63,
    ISMRMRD_IMAGE_USER8               = 64
};

/**
 * Struct used for keeping track of typical loop counters in MR experiment.
 */
typedef struct ISMRMRD_EncodingCounters {
    uint16_t kspace_encode_step_1;    /**< e.g. phase encoding line number */
    uint16_t kspace_encode_step_2;    /**< e.g. partition encoding number */
    uint16_t average;                 /**< e.g. signal average number */
    uint16_t slice;                   /**< e.g. imaging slice number */
    uint16_t contrast;                /**< e.g. echo number in multi-echo */
    uint16_t phase;                   /**< e.g. cardiac phase number */
    uint16_t repetition;              /**< e.g. dynamic number for dynamic scanning */
    uint16_t set;                     /**< e.g. flow encoding set */
    uint16_t segment;                 /**< e.g. segment number for segmented acquisition */
    uint16_t user[ISMRMRD_USER_INTS]; /**< Free user parameters */
} ISMRMRD_EncodingCounters;

#if __cplusplus > 199711L // Static assert requires C++11
// Check standard layout
static_assert(std::is_standard_layout<ISMRMRD_EncodingCounters>::value, "ISMRMRD_EncodingCounters is not a standard layout type");
// Check for size and offset of EncodingCounters struct members
static_assert(sizeof(ISMRMRD_EncodingCounters) == 34, "ISMRMRD_EncodingCounters is not the expected size");
static_assert(offsetof(ISMRMRD_EncodingCounters, kspace_encode_step_1) == 0, "kspace_encode_step_1 is not at the expected offset");
static_assert(offsetof(ISMRMRD_EncodingCounters, kspace_encode_step_2) == 2, "kspace_encode_step_2 is not at the expected offset");
static_assert(offsetof(ISMRMRD_EncodingCounters, average) == 4, "average is not at the expected offset");
static_assert(offsetof(ISMRMRD_EncodingCounters, slice) == 6, "slice is not at the expected offset");
static_assert(offsetof(ISMRMRD_EncodingCounters, contrast) == 8, "contrast is not at the expected offset");
static_assert(offsetof(ISMRMRD_EncodingCounters, phase) == 10, "phase is not at the expected offset");
static_assert(offsetof(ISMRMRD_EncodingCounters, repetition) == 12, "repetition is not at the expected offset");
static_assert(offsetof(ISMRMRD_EncodingCounters, set) == 14, "set is not at the expected offset");
static_assert(offsetof(ISMRMRD_EncodingCounters, segment) == 16, "segment is not at the expected offset");
static_assert(offsetof(ISMRMRD_EncodingCounters, user) == 18, "user is not at the expected offset");
#endif

/**
 * Header for each MR acquisition.
 */
typedef struct ISMRMRD_AcquisitionHeader {
    uint16_t version;                                    /**< First unsigned int indicates the version */
    uint64_t flags;                                      /**< bit field with flags */
    uint32_t measurement_uid;                            /**< Unique ID for the measurement */
    uint32_t scan_counter;                               /**< Current acquisition number in the measurement */
    uint32_t acquisition_time_stamp;                     /**< Acquisition clock */
    uint32_t physiology_time_stamp[ISMRMRD_PHYS_STAMPS]; /**< Physiology time stamps, e.g. ecg, breathing, etc. */
    uint16_t number_of_samples;                          /**< Number of samples acquired */
    uint16_t available_channels;                         /**< Available coils */
    uint16_t active_channels;                            /**< Active coils on current acquisition */
    uint64_t channel_mask[ISMRMRD_CHANNEL_MASKS];        /**< Mask to indicate which channels are active. Support for 1024 channels */
    uint16_t discard_pre;                                /**< Samples to be discarded at the beginning of  acquisition */
    uint16_t discard_post;                               /**< Samples to be discarded at the end of acquisition */
    uint16_t center_sample;                              /**< Sample at the center of k-space */
    uint16_t encoding_space_ref;                         /**< Reference to an encoding space, typically only one per acquisition */
    uint16_t trajectory_dimensions;                      /**< Indicates the dimensionality of the trajectory vector (0 means no trajectory) */
    float sample_time_us;                                /**< Time between samples in micro seconds, sampling BW */
    float position[3];                                   /**< Three-dimensional spatial offsets from isocenter */
    float read_dir[3];                                   /**< Directional cosines of the readout/frequency encoding */
    float phase_dir[3];                                  /**< Directional cosines of the phase */
    float slice_dir[3];                                  /**< Directional cosines of the slice direction */
    float patient_table_position[3];                     /**< Patient table off-center */
    ISMRMRD_EncodingCounters idx;                        /**< Encoding loop counters, see above */
    int32_t user_int[ISMRMRD_USER_INTS];                 /**< Free user parameters */
    float user_float[ISMRMRD_USER_FLOATS];               /**< Free user parameters */
} ISMRMRD_AcquisitionHeader;

#if __cplusplus > 199711L // Static assert requires C++11
// Check standard layout
static_assert(std::is_standard_layout<ISMRMRD_AcquisitionHeader>::value, "ISMRMRD_AcquisitionHeader is not a standard layout type");
// Check for size and offset of AcquisitionHeader struct members
static_assert(sizeof(ISMRMRD_AcquisitionHeader) == 340, "ISMRMRD_AcquisitionHeader is not the expected size");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, version) == 0, "version is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, flags) == 2, "flags is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, measurement_uid) == 10, "measurement_uid is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, scan_counter) == 14, "scan_counter is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, acquisition_time_stamp) == 18, "acquisition_time_stamp is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, physiology_time_stamp) == 22, "physiology_time_stamp is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, number_of_samples) == 34, "number_of_samples is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, available_channels) == 36, "available_channels is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, active_channels) == 38, "active_channels is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, channel_mask) == 40, "channel_mask is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, discard_pre) == 168, "discard_pre is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, discard_post) == 170, "discard_post is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, center_sample) == 172, "center_sample is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, encoding_space_ref) == 174, "encoding_space_ref is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, trajectory_dimensions) == 176, "trajectory_dimensions is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, sample_time_us) == 178, "sample_time_us is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, position) == 182, "position is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, read_dir) == 194, "read_dir is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, phase_dir) == 206, "phase_dir is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, slice_dir) == 218, "slice_dir is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, patient_table_position) == 230, "patient_table_position is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, idx) == 242, "idx is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, user_int) == 276, "user_int is not at the expected offset");
static_assert(offsetof(ISMRMRD_AcquisitionHeader, user_float) == 308, "user_float is not at the expected offset");
#endif

/**
 * Initialize an Acquisition Header
 * @ingroup capi
 *
 */
EXPORTISMRMRD int ismrmrd_init_acquisition_header(ISMRMRD_AcquisitionHeader *hdr);

/** Individual MR acquisition. */
typedef struct ISMRMRD_Acquisition {
    ISMRMRD_AcquisitionHeader head; /**< Header, see above */
    float *traj;
    complex_float_t *data;
} ISMRMRD_Acquisition;

/** @addtogroup capi
 *  @{
 */
EXPORTISMRMRD ISMRMRD_Acquisition * ismrmrd_create_acquisition();
EXPORTISMRMRD int ismrmrd_free_acquisition(ISMRMRD_Acquisition *acq);
EXPORTISMRMRD int ismrmrd_init_acquisition(ISMRMRD_Acquisition *acq);
EXPORTISMRMRD int ismrmrd_cleanup_acquisition(ISMRMRD_Acquisition *acq);
EXPORTISMRMRD int ismrmrd_copy_acquisition(ISMRMRD_Acquisition *acqdest, const ISMRMRD_Acquisition *acqsource);
EXPORTISMRMRD int ismrmrd_make_consistent_acquisition(ISMRMRD_Acquisition *acq);
EXPORTISMRMRD size_t ismrmrd_size_of_acquisition_traj(const ISMRMRD_Acquisition *acq);
EXPORTISMRMRD size_t ismrmrd_size_of_acquisition_data(const ISMRMRD_Acquisition *acq);
/** @} */

/**********/
/* Images */
/**********/

/**
 *  Header for each Image
 */
typedef struct ISMRMRD_ImageHeader {
    uint16_t version;                                    /**< First unsigned int indicates the version */
    uint16_t data_type;                                  /**< e.g. unsigned short, float, complex float, etc. */
    uint64_t flags;                                      /**< bit field with flags */
    uint32_t measurement_uid;                            /**< Unique ID for the measurement  */
    uint16_t matrix_size[3];                             /**< Pixels in the 3 spatial dimensions */
    float field_of_view[3];                              /**< Size (in mm) of the 3 spatial dimensions */
    uint16_t channels;                                   /**< Number of receive channels */
    float position[3];                                   /**< Three-dimensional spatial offsets from isocenter */
    float read_dir[3];                                   /**< Directional cosines of the readout/frequency encoding */
    float phase_dir[3];                                  /**< Directional cosines of the phase */
    float slice_dir[3];                                  /**< Directional cosines of the slice direction */
    float patient_table_position[3];                     /**< Patient table off-center */
    uint16_t average;                                    /**< e.g. signal average number */
    uint16_t slice;                                      /**< e.g. imaging slice number */
    uint16_t contrast;                                   /**< e.g. echo number in multi-echo */
    uint16_t phase;                                      /**< e.g. cardiac phase number */
    uint16_t repetition;                                 /**< e.g. dynamic number for dynamic scanning */
    uint16_t set;                                        /**< e.g. flow encoding set */
    uint32_t acquisition_time_stamp;                     /**< Acquisition clock */
    uint32_t physiology_time_stamp[ISMRMRD_PHYS_STAMPS]; /**< Physiology time stamps, e.g. ecg, breathing, etc. */
    uint16_t image_type;                                 /**< e.g. magnitude, phase, complex, real, imag, etc. */
    uint16_t image_index;                                /**< e.g. image number in series of images  */
    uint16_t image_series_index;                         /**< e.g. series number */
    int32_t user_int[ISMRMRD_USER_INTS];                 /**< Free user parameters */
    float user_float[ISMRMRD_USER_FLOATS];               /**< Free user parameters */
    uint32_t attribute_string_len;                       /**< Length of attributes string */
} ISMRMRD_ImageHeader;

#if __cplusplus > 199711L // Static assert requires C++11
// Check standard layout
static_assert(std::is_standard_layout<ISMRMRD_ImageHeader>::value, "ISMRMRD_ImageHeader is not a standard layout type");
// Check size and offsets of ISMRMRD_ImageHeader
static_assert(sizeof(ISMRMRD_ImageHeader) == 198, "ISMRMRD_ImageHeader is not the expected size");
static_assert(offsetof(ISMRMRD_ImageHeader, version) == 0, "version is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, data_type) == 2, "data_type is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, flags) == 4, "flags is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, measurement_uid) == 12, "measurement_uid is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, matrix_size) == 16, "matrix_size is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, field_of_view) == 22, "field_of_view is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, channels) == 34, "channels is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, position) == 36, "position is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, read_dir) == 48, "read_dir is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, phase_dir) == 60, "phase_dir is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, slice_dir) == 72, "slice_dir is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, patient_table_position) == 84, "patient_table_position is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, average) == 96, "average is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, slice) == 98, "slice is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, contrast) == 100, "contrast is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, phase) == 102, "phase is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, repetition) == 104, "repetition is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, set) == 106, "set is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, acquisition_time_stamp) == 108, "acquisition_time_stamp is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, physiology_time_stamp) == 112, "physiology_time_stamp is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, image_type) == 124, "image_type is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, image_index) == 126, "image_index is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, image_series_index) == 128, "image_series_index is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, user_int) == 130, "user_int is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, user_float) == 162, "user_float is not at the expected offset");
static_assert(offsetof(ISMRMRD_ImageHeader, attribute_string_len) == 194, "attribute_string_len is not at the expected offset");
#endif

/** @ingroup capi */
EXPORTISMRMRD int ismrmrd_init_image_header(ISMRMRD_ImageHeader *hdr);

/**
 *  An individual Image
 *  @ingroup capi
 */
typedef struct ISMRMRD_Image {
    ISMRMRD_ImageHeader head;
    char *attribute_string;
    void *data;
} ISMRMRD_Image;


/** @addtogroup capi
 *  @{
 */
EXPORTISMRMRD ISMRMRD_Image * ismrmrd_create_image();
EXPORTISMRMRD int ismrmrd_free_image(ISMRMRD_Image *im);
EXPORTISMRMRD int ismrmrd_init_image(ISMRMRD_Image *im);
EXPORTISMRMRD int ismrmrd_cleanup_image(ISMRMRD_Image *im);
EXPORTISMRMRD int ismrmrd_copy_image(ISMRMRD_Image *imdest, const ISMRMRD_Image *imsource);
EXPORTISMRMRD int ismrmrd_make_consistent_image(ISMRMRD_Image *im);
EXPORTISMRMRD size_t ismrmrd_size_of_image_attribute_string(const ISMRMRD_Image *im);
EXPORTISMRMRD size_t ismrmrd_size_of_image_data(const ISMRMRD_Image *im);
/** @} */

/************/
/* NDArrays */
/************/

/**
 *  A simple N dimensional array
 */
typedef struct ISMRMRD_NDArray {
    uint16_t version;                      /**< First unsigned int indicates the version */
    uint16_t data_type;                    /**< e.g. unsigned short, float, complex float, etc. */
    uint16_t ndim;                         /**< Number of dimensions */
    size_t   dims[ISMRMRD_NDARRAY_MAXDIM]; /**< Dimensions */
    void     *data;                        /**< Pointer to data */
} ISMRMRD_NDArray;

/** @addtogroup capi
 *  @{
 */
EXPORTISMRMRD ISMRMRD_NDArray * ismrmrd_create_ndarray();
EXPORTISMRMRD int ismrmrd_free_ndarray(ISMRMRD_NDArray *arr);
EXPORTISMRMRD int ismrmrd_init_ndarray(ISMRMRD_NDArray *arr);
EXPORTISMRMRD int ismrmrd_cleanup_ndarray(ISMRMRD_NDArray *arr);
EXPORTISMRMRD int ismrmrd_copy_ndarray(ISMRMRD_NDArray *arrdest, const ISMRMRD_NDArray *arrsource);
EXPORTISMRMRD int ismrmrd_make_consistent_ndarray(ISMRMRD_NDArray *arr);
EXPORTISMRMRD size_t ismrmrd_size_of_ndarray_data(const ISMRMRD_NDArray *arr);
/** @} */

/*********/
/* Flags */
/*********/
/** @addtogroup capi
 *  @{
 */
EXPORTISMRMRD bool ismrmrd_is_flag_set(const uint64_t flags, const uint64_t val);
EXPORTISMRMRD int ismrmrd_set_flag(ISMRMRD_UNALIGNED uint64_t *flags, const uint64_t val);
EXPORTISMRMRD int ismrmrd_set_flags(ISMRMRD_UNALIGNED uint64_t *flags, const uint64_t val);
EXPORTISMRMRD int ismrmrd_clear_flag(ISMRMRD_UNALIGNED uint64_t *flags, const uint64_t val);
EXPORTISMRMRD int ismrmrd_clear_all_flags(ISMRMRD_UNALIGNED uint64_t *flags);
/** @} */

/*****************/
/* Channel Masks */
/*****************/
/** @addtogroup capi
 *  @{
 */
EXPORTISMRMRD bool ismrmrd_is_channel_on(const uint64_t channel_mask[ISMRMRD_CHANNEL_MASKS], const uint16_t chan);
EXPORTISMRMRD int ismrmrd_set_channel_on(uint64_t channel_mask[ISMRMRD_CHANNEL_MASKS], const uint16_t chan);
EXPORTISMRMRD int ismrmrd_set_channel_off(uint64_t channel_mask[ISMRMRD_CHANNEL_MASKS], const uint16_t chan);
EXPORTISMRMRD int ismrmrd_set_all_channels_off(uint64_t channel_mask[ISMRMRD_CHANNEL_MASKS]);
/** @} */

/******************/
/* Error Handling */
/******************/
/** @addtogroup capi
 *  @{
 */
typedef void (*ismrmrd_error_handler_t)(const char *file, int line,
        const char *function, int code, const char *msg);
#define ISMRMRD_PUSH_ERR(code, msg) ismrmrd_push_error(__FILE__, __LINE__, \
        __func__, (code), (msg))
int ismrmrd_push_error(const char *file, const int line, const char *func,
        const int code, const char *msg);
/** Sets a custom error handler */
EXPORTISMRMRD void ismrmrd_set_error_handler(ismrmrd_error_handler_t);
/** Returns message for corresponding error code */
EXPORTISMRMRD const char *ismrmrd_strerror(int code);
/** @} */

/** Populates parameters (if non-NULL) with error information
 * @returns true if there was error information to return, false otherwise */
bool ismrmrd_pop_error(char **file, int *line, char **func,
        int *code, char **msg);

/*****************************/
/* Rotations and Quaternions */
/*****************************/
/** @addtogroup capi
 *  @{
 */
/** Calculates the determinant of the matrix and return the sign */
EXPORTISMRMRD int ismrmrd_sign_of_directions(float const read_dir[3], float const phase_dir[3], float const slice_dir[3]);

/** Creates a normalized quaternion from a 3x3 rotation matrix */
EXPORTISMRMRD void ismrmrd_directions_to_quaternion(float const read_dir[3], float const phase_dir[3], float const slice_dir[3], float quat[4]);

/** Converts a quaternion of the form | a b c d | to a 3x3 rotation matrix */
EXPORTISMRMRD void ismrmrd_quaternion_to_directions(float const quat[4], float read_dir[3], float phase_dir[3], float slice_dir[3]);
/** @} */

#pragma pack(pop) /* Restore old alignment */

#ifdef __cplusplus
} // extern "C"

///  ISMRMRD C++ Interface

/// Construct exception message from ISMRMRD error stack
std::string build_exception_string(void);

/// Some typedefs to beautify the namespace
typedef  ISMRMRD_EncodingCounters EncodingCounters;


/** @addtogroup cxxapi
 *  @{
 */


bool operator==(const EncodingCounters& ec1, const EncodingCounters& ec2);

/// Allowed data types for Images and NDArrays
template <typename T> EXPORTISMRMRD ISMRMRD_DataTypes get_data_type();

/// Convenience class for flags
class EXPORTISMRMRD FlagBit
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

/// Header for MR Acquisition type
class EXPORTISMRMRD AcquisitionHeader: public ISMRMRD_AcquisitionHeader {
public:
    // Constructors
    AcquisitionHeader();


    bool operator==(const AcquisitionHeader& acq) const;

    // Flag methods
    bool isFlagSet(const ISMRMRD_AcquisitionFlags val) const;
    void setFlag(const ISMRMRD_AcquisitionFlags val);
    void clearFlag(const ISMRMRD_AcquisitionFlags val);
    void clearAllFlags();

    // Channel mask methods
    bool isChannelActive(uint16_t channel_id) const;
    void setChannelActive(uint16_t channel_id);
    void setChannelNotActive(uint16_t channel_id);
    void setAllChannelsNotActive();

};

#if __cplusplus > 199711L // Static assert and is_standard_layout requires C++11
static_assert(sizeof(AcquisitionHeader) == sizeof(ISMRMRD_AcquisitionHeader), "AcquisitionHeader size mismatch");
static_assert(std::is_standard_layout<AcquisitionHeader>::value, "AcquisitionHeader is not a standard layout type");
#endif

/// MR Acquisition type
class EXPORTISMRMRD Acquisition {
    friend class Dataset;
public:
    // Constructors, assignment, destructor
    Acquisition();
    Acquisition(uint16_t num_samples, uint16_t active_channels=1, uint16_t trajectory_dimensions=0);
    Acquisition(const Acquisition &other);
    Acquisition & operator= (const Acquisition &other);
    ~Acquisition();

    // Accessors and mutators
    const uint16_t &version();
    const uint64_t &flags();
    uint32_t &measurement_uid();
    uint32_t &scan_counter();
    uint32_t &acquisition_time_stamp();
    uint32_t (&physiology_time_stamp())[ISMRMRD_PHYS_STAMPS];
    const uint16_t &number_of_samples();
    uint16_t &available_channels();
    const uint16_t &active_channels();
    const uint64_t (&channel_mask())[ISMRMRD_CHANNEL_MASKS];
    uint16_t &discard_pre();
    uint16_t &discard_post();
    uint16_t &center_sample();
    uint16_t &encoding_space_ref();
    const uint16_t &trajectory_dimensions();
    float &sample_time_us();
    float (&position())[3];
    float (&read_dir())[3];
    float (&phase_dir())[3];
    float (&slice_dir())[3];
    float (&patient_table_position())[3];
    ISMRMRD_EncodingCounters &idx();
    int32_t (&user_int())[ISMRMRD_USER_INTS];
    float (&user_float())[ISMRMRD_USER_FLOATS];

    // Sizes
    void resize(uint16_t num_samples, uint16_t active_channels=1, uint16_t trajectory_dimensions=0);
    size_t getNumberOfDataElements() const;
    size_t getNumberOfTrajElements() const;
    size_t getDataSize() const;
    size_t getTrajSize() const;

    // Header, data and trajectory accessors
    const AcquisitionHeader &getHead() const;
    void setHead(const AcquisitionHeader &other);

    /**
     * Returns a pointer to the data
     */
    const complex_float_t * getDataPtr() const;
    complex_float_t * getDataPtr();

    /**
     * Returns a reference to the data
     */
    complex_float_t & data(uint16_t sample, uint16_t channel);

    /**
     * Sets the datay.  Must set sizes properly first
     */
    void setData(complex_float_t * data);

    /**
     * Returns an iterator to the beginning of the data
     */
    complex_float_t * data_begin() const;

    /**
     * Returns an iterator of the end of the data
     */
    complex_float_t * data_end() const;

    /**
     * Returns a pointer to the trajectory
     */
    const float * getTrajPtr() const;
    float * getTrajPtr();

    /**
     * Returns a reference to the trajectory
     */
    float & traj(uint16_t dimension, uint16_t sample);

    /**
     * Sets the trajectory.  Must set sizes properly first
     */
    void setTraj(float * traj);

    /**
     * Returns an iterator to the beginning of the trajectories
     */
    float * traj_begin() const;

    /**
     * Returns an iterator to the end of the trajectories
     */
    float * traj_end() const;

    // Flag methods
    bool isFlagSet(const uint64_t val);
    void setFlag(const uint64_t val);
    void clearFlag(const uint64_t val);
    void clearAllFlags();

    bool isFlagSet(const FlagBit &val)  { return isFlagSet(val.bitmask_); }
    void setFlag(const FlagBit &val)    { setFlag(val.bitmask_); }
    void clearFlag(const FlagBit &val)  { clearFlag(val.bitmask_); }

    // Channel mask methods
    bool isChannelActive(uint16_t channel_id);
    void setChannelActive(uint16_t channel_id);
    void setChannelNotActive(uint16_t channel_id);
    void setAllChannelsNotActive();

protected:
    ISMRMRD_Acquisition acq;
};

/// Header for MR Image type
class EXPORTISMRMRD ImageHeader: public ISMRMRD_ImageHeader {
public:
    // Constructor
    ImageHeader();

    // Flag methods
    bool isFlagSet(const uint64_t val);
    void setFlag(const uint64_t val);
    void clearFlag(const uint64_t val);
    void clearAllFlags();

};

#if __cplusplus > 199711L // Static assert and is_standard_layout requires C++11
static_assert(sizeof(ImageHeader) == sizeof(ISMRMRD_ImageHeader), "ImageHeader size mismatch");
static_assert(std::is_standard_layout<ImageHeader>::value, "ImageHeader is not a standard layout type");
#endif

/// MR Image type
template <typename T> class EXPORTISMRMRD Image {
    friend class Dataset;
public:
    // Constructors
    Image(uint16_t matrix_size_x = 0, uint16_t matrix_size_y = 1,
          uint16_t matrix_size_z = 1, uint16_t channels = 1);
    Image(const Image &other);
    Image & operator= (const Image &other);
    ~Image();

    // Image dimensions
    void resize(uint16_t matrix_size_x, uint16_t matrix_size_y, uint16_t matrix_size_z, uint16_t channels);
    uint16_t getMatrixSizeX() const;
    void setMatrixSizeX(uint16_t matrix_size_x);
    uint16_t getMatrixSizeY() const;
    void setMatrixSizeY(uint16_t matrix_size_y);
    uint16_t getMatrixSizeZ() const;
    void setMatrixSizeZ(uint16_t matrix_size_z);
    uint16_t getNumberOfChannels() const;
    void setNumberOfChannels(uint16_t channels);

    // Field of view
    void setFieldOfView(float fov_x, float fov_y, float fov_z);
    float getFieldOfViewX() const;
    void setFieldOfViewX(float f);
    float getFieldOfViewY() const;
    void setFieldOfViewY(float f);
    float getFieldOfViewZ() const;
    void setFieldOfViewZ(float f);

    // Positions and orientations
    void setPosition(float x, float y, float z);
    float getPositionX() const;
    void setPositionX(float x);
    float getPositionY() const;
    void setPositionY(float y);
    float getPositionZ() const;
    void setPositionZ(float z);

    void setReadDirection(float x, float y, float z);
    float getReadDirectionX() const;
    void setReadDirectionX(float x);
    float getReadDirectionY() const;
    void setReadDirectionY(float y);
    float getReadDirectionZ() const;
    void setReadDirectionZ(float z);

    void setPhaseDirection(float x, float y, float z);
    float getPhaseDirectionX() const;
    void setPhaseDirectionX(float x);
    float getPhaseDirectionY() const;
    void setPhaseDirectionY(float y);
    float getPhaseDirectionZ() const;
    void setPhaseDirectionZ(float z);

    void setSliceDirection(float x, float y, float z);
    float getSliceDirectionX() const;
    void setSliceDirectionX(float x);
    float getSliceDirectionY() const;
    void setSliceDirectionY(float y);
    float getSliceDirectionZ() const;
    void setSliceDirectionZ(float z);

    void setPatientTablePosition(float x, float y, float z);
    float getPatientTablePositionX() const;
    void setPatientTablePositionX(float x);
    float getPatientTablePositionY() const;
    void setPatientTablePositionY(float y);
    float getPatientTablePositionZ() const;
    void setPatientTablePositionZ(float z);


    // Attributes
    uint16_t getVersion() const;
    ISMRMRD_DataTypes getDataType() const;

    // Counters and labels
    uint32_t getMeasurementUid() const;
    void setMeasurementUid(uint32_t measurement_uid);

    uint16_t getAverage() const;
    void setAverage(uint16_t average);

    uint16_t getSlice() const;
    void setSlice(uint16_t slice);

    uint16_t getContrast() const;
    void setContrast(uint16_t contrast);

    uint16_t getPhase() const;
    void setPhase(uint16_t phase);

    uint16_t getRepetition() const;
    void setRepetition(uint16_t repetition);

    uint16_t getSet() const;
    void setSet(uint16_t set);

    uint32_t getAcquisitionTimeStamp() const;
    void setAcquisitionTimeStamp(uint32_t acquisition_time_stamp);

    uint32_t getPhysiologyTimeStamp(unsigned int stamp_id) const;
    void setPhysiologyTimeStamp(unsigned int stamp_id, uint32_t value);

    uint16_t getImageType() const;
    void setImageType(uint16_t image_type);

    uint16_t getImageIndex() const;
    void setImageIndex(uint16_t image_index);

    uint16_t getImageSeriesIndex() const;
    void setImageSeriesIndex(uint16_t image_series_index);

    // User parameters
    float getUserFloat(unsigned int index) const;
    void setUserFloat(unsigned int index, float value);

    int32_t getUserInt(unsigned int index) const;
    void setUserInt(unsigned int index, int32_t value);

    // Flags
    uint64_t getFlags() const;
    void setFlags(const uint64_t flags);
    bool isFlagSet(const uint64_t val) const;
    void setFlag(const uint64_t val);
    void clearFlag(const uint64_t val);
    void clearAllFlags();

    // Header
    ImageHeader & getHead();
    const ImageHeader & getHead() const;
    void setHead(const ImageHeader& head);

    // Attribute string
    void getAttributeString(std::string &attr) const;
    const char *getAttributeString() const;
    void setAttributeString(const std::string &attr);
    void setAttributeString(const char *attr);
    size_t getAttributeStringLength() const;

    // Data
    T * getDataPtr();
    const T * getDataPtr() const;
    /** Returns the number of elements in the image data **/
    size_t getNumberOfDataElements() const;
    /** Returns the size of the image data in bytes **/
    size_t getDataSize() const;

    /** Returns iterator to the beginning of the image data **/
    T* begin();

    /** Returns iterator to the end of the image data **/
    T* end();

    /** Returns a reference to the image data **/
    T & operator () (uint16_t x, uint16_t y=0, uint16_t z=0 , uint16_t channel =0);

protected:
    ISMRMRD_Image im;
};

/// N-Dimensional array type
template <typename T> class EXPORTISMRMRD NDArray {
    friend class Dataset;
public:
    // Constructors, destructor and copy
    NDArray();
    NDArray(const std::vector<size_t> dimvec);
    NDArray(const NDArray<T> &other);
    ~NDArray();
    NDArray<T> & operator= (const NDArray<T> &other);

    // Accessors and mutators
    uint16_t getVersion() const;
    ISMRMRD_DataTypes getDataType() const;
    uint16_t getNDim() const;
    const size_t (&getDims())[ISMRMRD_NDARRAY_MAXDIM];
    size_t getDataSize() const;
    void resize(const std::vector<size_t> dimvec);
    size_t getNumberOfElements() const;
    T * getDataPtr();
    const T * getDataPtr() const;

    /** Returns iterator to the beginning of the array **/
    T * begin();

    /** Returns iterator to the end of the array **/
    T* end();

    /** Returns a reference to the image data **/
    T & operator () (uint16_t x, uint16_t y=0, uint16_t z=0, uint16_t w=0, uint16_t n=0, uint16_t m=0, uint16_t l=0);

protected:
    ISMRMRD_NDArray arr;
};


/** @} */

} // namespace ISMRMRD

#endif

#endif /* ISMRMRD_H */
