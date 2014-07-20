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

/* Language and Cross platform section for defining types */
#ifdef __cplusplus

#ifdef _MSC_VER
// MS C++ Compiler
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;

#else
// non MS C++ compiler
#include <cstdint>
#endif

#include <cstdlib>
#include <cstring>
#include <complex>
#include <cmath>
typedef std::complex<float> complex_float_t;
typedef std::complex<double> complex_double_t;

#else
// C99 compiler
#include <stdint.h>
#include <complex.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
typedef float complex complex_float_t;
typedef double complex complex_double_t;

#endif  // __cplusplus

#pragma pack(push, 2)  // Use 2 byte alignment

#define ISMRMRD_VERSION 1
#define ISMRMRD_POSITION_LENGTH 3
#define ISMRMRD_DIRECTION_LENGTH 3
#define ISMRMRD_USER_INTS 8
#define ISMRMRD_USER_FLOATS 8
#define ISMRMRD_PHYS_STAMPS 3
#define ISMRMRD_CHANNEL_MASKS 16
#define ISMRMRD_NDARRAY_MAXDIM 7

#ifdef __cplusplus
namespace ISMRMRD {
#endif  //__cplusplus

/**************/
/* Data Types */
/**************/
enum DataTypes {
  DATA_UNSIGNED_SHORT = 1,  // corresponds to uint16_t
  DATA_SHORT,               // corresponds to int16_t
  DATA_UNSIGNED_INT,        // corresponds to uint32_t
  DATA_INT,                 // corresponds to int32_t
  DATA_FLOAT,               // corresponds to float
  DATA_DOUBLE,              // corresponds to double
  DATA_COMPLEX_FLOAT,       // corresponds to complex float
  DATA_COMPLEX_DOUBLE,      // corresponds to complex double
};

/****************/
/* Acquisitions */
/****************/

/** ACQUISITION FLAGS */
enum AcquisitionFlags {
  /* Looping indicators */
  ACQ_FIRST_IN_ENCODE_STEP1 = 1,
  ACQ_LAST_IN_ENCODE_STEP1 = 2,
  ACQ_FIRST_IN_ENCODE_STEP2 = 3,
  ACQ_LAST_IN_ENCODE_STEP2 = 4,
  ACQ_FIRST_IN_AVERAGE = 5,
  ACQ_LAST_IN_AVERAGE = 6,
  ACQ_FIRST_IN_SLICE = 7,
  ACQ_LAST_IN_SLICE = 8,
  ACQ_FIRST_IN_CONTRAST = 9,
  ACQ_LAST_IN_CONTRAST = 10,
  ACQ_FIRST_IN_PHASE = 11,
  ACQ_LAST_IN_PHASE = 12,
  ACQ_FIRST_IN_REPETITION = 13,
  ACQ_LAST_IN_REPETITION = 14,
  ACQ_FIRST_IN_SET = 15,
  ACQ_LAST_IN_SET = 16,
  ACQ_FIRST_IN_SEGMENT = 17,
  ACQ_LAST_IN_SEGMENT = 18,
  ACQ_IS_NOISE_MEASUREMENT = 19,
  ACQ_IS_PARALLEL_CALIBRATION = 20,
  ACQ_IS_PARALLEL_CALIBRATION_AND_IMAGING = 21,
  ACQ_IS_REVERSE = 22,
  ACQ_IS_NAVIGATION_DATA = 23,
  ACQ_IS_PHASECORR_DATA = 24,
  ACQ_LAST_IN_MEASUREMENT = 25,
  ACQ_IS_HPFEEDBACK_DATA = 26,
  ACQ_IS_DUMMYSCAN_DATA = 27,
  ACQ_IS_RTFEEDBACK_DATA = 28,
  ACQ_IS_SURFACECOILCORRECTIONSCAN_DATA = 29,
  ACQ_USER1 = 57,
  ACQ_USER2 = 58,
  ACQ_USER3 = 59,
  ACQ_USER4 = 60,
  ACQ_USER5 = 61,
  ACQ_USER6 = 62,
  ACQ_USER7 = 63,
  ACQ_USER8 = 64
};

/**
   Struct used for keeping track of typical loop counters in MR experiment.
*/
typedef struct EncodingCounters {
  uint16_t kspace_encode_step_1; /**< e.g. phase encoding line number */
  uint16_t kspace_encode_step_2; /**< e.g. partition encodning number */
  uint16_t average;              /**< e.g. signal average number */
  uint16_t slice;                /**< e.g. imaging slice number */
  uint16_t contrast;             /**< e.g. echo number in multi-echo */
  uint16_t phase;                /**< e.g. cardiac phase number */
  uint16_t repetition; /**< e.g. dynamic number for dynamic scanning */
  uint16_t set;        /**< e.g. flow encodning set */
  uint16_t segment;    /**< e.g. segment number for segmented acquisition */
  uint16_t user[ISMRMRD_USER_INTS]; /**< Free user parameters */
} EncodingCounters;

/**
   Header for each MR acquisition.
*/
typedef struct AcquisitionHeader {
  uint16_t version;         /**< First unsigned int indicates the version */
  uint64_t flags;           /**< bit field with flags */
  uint32_t measurement_uid; /**< Unique ID for the measurement */
  uint32_t scan_counter; /**< Current acquisition number in the measurement */
  uint32_t acquisition_time_stamp;                     /**< Acquisition clock */
  uint32_t physiology_time_stamp[ISMRMRD_PHYS_STAMPS]; /**< Physiology time
                                                          stamps, e.g. ecg,
                                                          breating, etc. */
  uint16_t number_of_samples;  /**< Number of samples acquired */
  uint16_t available_channels; /**< Available coils */
  uint16_t active_channels;    /**< Active coils on current acquisiton */
  uint64_t channel_mask[ISMRMRD_CHANNEL_MASKS]; /**< Mask to indicate which
                                                   channels are
                                                   active. Support for 1024
                                                   channels */
  uint16_t discard_pre; /**< Samples to be discarded at the beginning of
                           acquisition */
  uint16_t
      discard_post; /**< Samples to be discarded at the end of acquisition */
  uint16_t center_sample;      /**< Sample at the center of k-space */
  uint16_t encoding_space_ref; /**< Reference to an encoding space, typically
                                  only one per acquisition */
  uint16_t trajectory_dimensions; /**< Indicates the dimensionality of the
                                     trajectory vector (0 means no trajectory)
                                     */
  float
      sample_time_us; /**< Time between samples in micro seconds, sampling BW */
  float position[ISMRMRD_POSITION_LENGTH]; /**< Three-dimensional spatial
                                              offsets from isocenter */
  float read_dir[ISMRMRD_DIRECTION_LENGTH]; /**< Directional cosines of the
                                               readout/frequency encoding */
  float phase_dir[ISMRMRD_DIRECTION_LENGTH]; /**< Directional cosines of the
                                                phase */
  float slice_dir[ISMRMRD_DIRECTION_LENGTH]; /**< Directional cosines of the
                                                slice direction */
  float patient_table_position[ISMRMRD_POSITION_LENGTH]; /**< Patient table
                                                            off-center */
  EncodingCounters idx;                /**< Encoding loop counters, see above */
  int32_t user_int[ISMRMRD_USER_INTS]; /**< Free user parameters */
  float user_float[ISMRMRD_USER_FLOATS]; /**< Free user parameters */
} AcquisitionHeader;

/* Initialize an Acquisition Header */
void InitAcquisitionHeader(AcquisitionHeader* hdr);

/**
   Individual MR acquisition.
*/
typedef struct Acquisition {
  AcquisitionHeader head; /**< Header, see above */
  float* traj;
  complex_float_t* data;
} Acquisition;

void InitAcquisition(Acquisition* acq);
void CopyAcquisition(Acquisition* acqdest, const Acquisition* acqsource);
void FreeAcquisition(Acquisition* acq);
void MakeConsistentAcquisition(Acquisition* acq);

/**********/
/* Images */
/**********/

enum ImageType {
  TYPE_MAGNITUDE = 1,
  TYPE_PHASE,
  TYPE_REAL,
  TYPE_IMAG,
  TYPE_COMPLEX
};

/** IMAGE FLAGS */
enum ImageFlags {
  IMAGE_IS_NAVIGATION_DATA = 23,
  IMAGE_USER1 = 57,
  IMAGE_USER2 = 58,
  IMAGE_USER3 = 59,
  IMAGE_USER4 = 60,
  IMAGE_USER5 = 61,
  IMAGE_USER6 = 62,
  IMAGE_USER7 = 63,
  IMAGE_USER8 = 64
};

/**
 *  Header for each Image
 */
typedef struct ImageHeader {
  uint16_t version;   /**< First unsigned int indicates the version */
  uint16_t data_type; /**< e.g. unsigned short, float, complex float, etc. */
  uint64_t flags;     /**< bit field with flags */
  uint32_t measurement_uid; /**< Unique ID for the measurement  */
  uint16_t matrix_size[3];  /**< Pixels in the 3 spatial dimensions */
  float field_of_view[3];   /**< Size (in mm) of the 3 spatial dimensions */
  uint16_t channels;        /**< Number of receive channels */
  float position[ISMRMRD_POSITION_LENGTH]; /**< Three-dimensional spatial
                                              offsets from isocenter */
  float read_dir[ISMRMRD_DIRECTION_LENGTH]; /**< Directional cosines of the
                                               readout/frequency encoding */
  float phase_dir[ISMRMRD_DIRECTION_LENGTH]; /**< Directional cosines of the
                                                phase */
  float slice_dir[ISMRMRD_DIRECTION_LENGTH]; /**< Directional cosines of the
                                                slice direction */
  float patient_table_position[ISMRMRD_POSITION_LENGTH]; /**< Patient table
                                                            off-center */
  uint16_t average;    /**< e.g. signal average number */
  uint16_t slice;      /**< e.g. imaging slice number */
  uint16_t contrast;   /**< e.g. echo number in multi-echo */
  uint16_t phase;      /**< e.g. cardiac phase number */
  uint16_t repetition; /**< e.g. dynamic number for dynamic scanning */
  uint16_t set;        /**< e.g. flow encodning set */
  uint32_t acquisition_time_stamp;                     /**< Acquisition clock */
  uint32_t physiology_time_stamp[ISMRMRD_PHYS_STAMPS]; /**< Physiology time
                                                          stamps, e.g. ecg,
                                                          breating, etc. */
  uint16_t image_type;  /**< e.g. magnitude, phase, complex, real, imag, etc. */
  uint16_t image_index; /**< e.g. image number in series of images  */
  uint16_t image_series_index;           /**< e.g. series number */
  int32_t user_int[ISMRMRD_USER_INTS];   /**< Free user parameters */
  float user_float[ISMRMRD_USER_FLOATS]; /**< Free user parameters */
  uint32_t attribute_string_len;         /**< Length of attributes string */
} ImageHeader;

void InitImageHeader(ImageHeader* hdr);

/**
 *  An individual Image
 */
typedef struct Image {
  ImageHeader head;
  void* data;
  char* attribute_string;
} Image;

void InitImage(Image* im);
void FreeImage(Image* im);
void MakeConsistentImage(Image* im);

/************/
/* NDArrays */
/************/

typedef struct NDArray {
  uint16_t version;   /**< First unsigned int indicates the version */
  uint16_t data_type; /**< e.g. unsigned short, float, complex float, etc. */
  uint16_t ndim;      /**< Number of dimensions */
  uint16_t dims[ISMRMRD_NDARRAY_MAXDIM]; /**< Dimensions */
  void* data;                            /**< pointer to data array */
} NDArray;

void InitNDArray(NDArray* arr);
void FreeNDArray(NDArray* arr);
void MakeConsistentNDArray(NDArray* arr);

/*********/
/* Flags */
/*********/
bool flags_is_set(const uint64_t flags, const uint64_t val);
void flags_set(uint64_t* flags, const uint64_t val);
void flags_clear(uint64_t* flags, const uint64_t val);
void flags_clear_all(uint64_t* flags);

/*****************************/
/* Rotations and Quaternions */
/*****************************/
/* Calculates the determinant of the matrix and return the sign */
int sign_of_directions(float read_dir[3], float phase_dir[3],
                       float slice_dir[3]);

/* Creates a normalized quaternion from a 3x3 rotation matrix */
void directions_to_quaternion(float read_dir[3], float phase_dir[3],
                              float slice_dir[3], float quat[4]);

/* Converts a quaternion of the form | a b c d | to a 3x3 rotation matrix */
void quaternion_to_directions(float quat[4], float read_dir[3],
                              float phase_dir[3], float slice_dir[3]);

#ifdef __cplusplus
}  // End of ISMRMRD namespace
#endif  //__cplusplus

#pragma pack(pop)  // Restore old alignment

#endif  // ISMRMRD_H
