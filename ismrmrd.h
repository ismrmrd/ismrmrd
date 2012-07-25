/* ISMRMRD MR Raw Data Strutures                           */
/* DRAFT - July 2012                                       */

/* Authors:                                                */
/*    Michael S. Hansen (michael.hansen@nih.gov)           */
/*    Brian Hargreaves  (bah@stanford.edu)                 */
/*    Sebastian Kozerke (kozerke@biomed.ee.ethz.ch)        */

#prama once
#ifndef ISMRMRD_H
#define ISMRMRD_H

/* Cross platform section for defining integer types */
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

#pragma pack(4) //Use 4 byte alignment

namespace ISMRMRD
{

/**
   Struct used for keeping track of typical loop counters in MR experiment.

 */
struct EncodingCounters {
  uint16_t kspace_encode_step_1; //e.g. phase encoding line number
  uint16_t kspace_encode_step_2; //e.g. partition encodning number
  uint16_t average;              //e.g. signal average number
  uint16_t slice;                //e.g. imaging slice number
  uint16_t contrast;             //e.g. echo number in multi-echo
  uint16_t phase;                //e.g. cardiac phase number
  uint16_t repetition;           //e.g. dynamic number for dynamic scanning
  uint16_t set;                  //e.g. flow encodning set
  uint16_t segment;              //e.g. segment number for segmented acquisition
  uint16_t user[8];              //Free user parameters
};

/**
   Header for each MR acquisition. 
 */
struct Acquisition
{
  uint64_t           flags;                          //bit field with flags
  uint32_t           measurement_uid;                //Unique ID for the measurement
  uint32_t           scan_counter;                   //Current acquisition number in the measurement
  uint32_t           acquisition_time_stamp;         //Acquisition clock
  uint32_t           physiology_time_stamp[3];       //Physiology time stamps, e.g. ecg, breating, etc. 
  uint16_t           number_of_samples;              //Number of samples acquired
  uint16_t           available_channels;             //Available coils
  uint16_t           active_channels;                //Active coils on current acquisiton
  uint64_t           channel_mask[4];                //Mask to indicate which channels are active. Support for 256 channels
  uint16_t           discard_pre;                    //Samples to be discarded at the beginning of acquisition
  uint16_t           discard_post;                   //Samples to be discarded at the end of acquisition
  uint16_t           center_sample;                  //Sample at the center of k-space
  uint16_t           encoding_space_ref;             //Reference to an encoding space, typically only one per acquisition
  float              sample_time_us;                 //Time between samples in micro seconds, sampling BW
  float              position[3];                    //Three-dimensional spatial offsets from isocenter
  float              quarternion[4];                 //Angulation of acquisition
  float	             patient_table_position[3];      //Patient table off-center
  EncodingCounters   idx;                            //Encoding loop counters, see above
  uint32_t           user_int[8];                    //Free user parameters
  float              user_float[8];                  //Free user parameters
}; 

} //End of ISMRMRD namespace

#endif //ISMRMRD_H
