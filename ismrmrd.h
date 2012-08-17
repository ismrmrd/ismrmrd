/* ISMRMRD MR Raw Data Strutures                           */
/* DRAFT - July 2012                                       */

/* Authors:                                                */
/*    Michael S. Hansen (michael.hansen@nih.gov)           */
/*    Brian Hargreaves  (bah@stanford.edu)                 */
/*    Sebastian Kozerke (kozerke@biomed.ee.ethz.ch)        */

#pragma once
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

#include <stdio.h>
#include <string.h>
#include <exception>
#include <iostream>

#pragma pack(push, 4) //Use 4 byte alignment

#define ISMRMRD_VERSION 1

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

	  bool isSet(uint64_t& m) {
		  return ((m & bitmask_)>0);
	  }

	  uint64_t bitmask_;

  };

  /* FLAGS */
  enum {
	  /* Looping indicators */
	  FIRST_IN_ENCODE_STEP1                	= 1,
	  LAST_IN_ENCODE_STEP1    				= 2,
	  FIRST_IN_ENCODE_STEP2   				= 3,
	  LAST_IN_ENCODE_STEP2    				= 4,
	  FIRST_IN_AVERAGE        				= 5,
	  LAST_IN_AVERAGE         				= 6,
	  FIRST_IN_SLICE          				= 7,
	  LAST_IN_SLICE           				= 8,
	  FIRST_IN_CONTRAST       				= 9,
	  LAST_IN_CONTRAST        				= 10,
	  FIRST_IN_PHASE          				= 11,
	  LAST_IN_PHASE           				= 12,
	  FIRST_IN_REPETITION     				= 13,
	  LAST_IN_REPETITION      				= 14,
	  FIRST_IN_SET            				= 15,
	  LAST_IN_SET             				= 16,
	  FIRST_IN_SEGMENT        				= 17,
	  LAST_IN_SEGMENT         				= 18,

	  IS_NOISE_MEASUREMENT                	= 19,
	  IS_PARALLEL_CALIBRATION             	= 20,
	  IS_PARALLEL_CALIBRATION_AND_IMAGING 	= 21,
	  IS_REVERSE              				= 22,
	  IS_NAVIGATION_DATA      				= 23,

	  USER1                   				= 57,
	  USER2                   				= 58,
	  USER3                   				= 59,
	  USER4                   				= 60,
	  USER5                   				= 61,
	  USER6                   				= 62,
	  USER7                   				= 63,
	  USER8                   				= 64
  };


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
 struct AcquisitionHeader
 {
   uint16_t           version;                        //First unsigned int indicates the version
   uint64_t           flags;                          //bit field with flags
   uint32_t           measurement_uid;                //Unique ID for the measurement
   uint32_t           scan_counter;                   //Current acquisition number in the measurement
   uint32_t           acquisition_time_stamp;         //Acquisition clock
   uint32_t           physiology_time_stamp[3];       //Physiology time stamps, e.g. ecg, breating, etc. 
   uint16_t           number_of_samples;              //Number of samples acquired
   uint16_t           available_channels;             //Available coils
   uint16_t           active_channels;                //Active coils on current acquisiton
   uint64_t           channel_mask[16];               //Mask to indicate which channels are active. Support for 1024 channels
   uint16_t           discard_pre;                    //Samples to be discarded at the beginning of acquisition
   uint16_t           discard_post;                   //Samples to be discarded at the end of acquisition
   uint16_t           center_sample;                  //Sample at the center of k-space
   uint16_t           encoding_space_ref;             //Reference to an encoding space, typically only one per acquisition
   uint16_t           trajectory_dimensions;          //Indicates the dimensionality of the trajectory vector (0 means no trajectory)
   float              sample_time_us;                 //Time between samples in micro seconds, sampling BW
   float              position[3];                    //Three-dimensional spatial offsets from isocenter
   float              quaternion[4];                  //Angulation of acquisition
   float              patient_table_position[3];      //Patient table off-center
   EncodingCounters   idx;                            //Encoding loop counters, see above
   int32_t            user_int[8];                    //Free user parameters
   float              user_float[8];                  //Free user parameters
 }; 

 class Acquisition
 {

 public:
   Acquisition() 
     : traj_(0)
     , data_(0)
     {
       memset(&head_,0,sizeof(AcquisitionHeader));
       head_.version = ISMRMRD_VERSION;
     }

   ~Acquisition() {
	   deleteData();
   }

   bool isFlagSet(FlagBit f) {
	   return f.isSet(head_.flags);
   }

   void setFlag(FlagBit f) {
	   head_.flags |= f.bitmask_;
   }

   Acquisition(const Acquisition& a) {   // copy constructor
	   head_ = a.head_;
	   if (head_.trajectory_dimensions > 0) {
		   size_t trajectory_elements = head_.number_of_samples*head_.trajectory_dimensions;
		   try {
			   traj_ = new float[trajectory_elements];
		   } catch (std::exception& e) {
			   std::cerr << "Unable to allocate trajectory in ISMRMRD::Acquisition: " << e.what() << std::endl;
		   }
		   memcpy(traj_,a.traj_,sizeof(float)*trajectory_elements);
	   } else {
		   traj_ = 0;
	   }

	   size_t data_elements = head_.number_of_samples*head_.active_channels;
	   if (data_elements > 0) {
		   try {
			   data_ = new float[data_elements*2]; //*2 for complex
		   } catch (std::exception& e) {
			   std::cerr << "Unable to allocate trajectory in ISMRMRD::Acquisition: " << e.what() << std::endl;
		   }
		   memcpy(data_,a.data_,sizeof(float)*2*data_elements);
	   } else {
		   data_ = 0;
	   }
   }

   Acquisition& operator=(const Acquisition& a) {
	   head_ = a.head_;
	   deleteData();
	   if (head_.trajectory_dimensions > 0) {
		   size_t trajectory_elements = head_.number_of_samples*head_.trajectory_dimensions;
		   try {
			   traj_ = new float[trajectory_elements];
		   } catch (std::exception& e) {
			   std::cerr << "Unable to allocate trajectory in ISMRMRD::Acquisition: " << e.what() << std::endl;
		   }
		   memcpy(traj_,a.traj_,sizeof(float)*trajectory_elements);
	   }

	   size_t data_elements = head_.number_of_samples*head_.active_channels;
	   if (data_elements > 0) {
		   try {
			   data_ = new float[data_elements*2]; //*2 for complex
		   } catch (std::exception& e) {
			   std::cerr << "Unable to allocate trajectory in ISMRMRD::Acquisition: " << e.what() << std::endl;
		   }
		   memcpy(data_,a.data_,sizeof(float)*2*data_elements);
	   }
	   return *this;
   }

   AcquisitionHeader head_; //Header, see above
   
   float* traj_;            //Trajectory, elements = head_.trajectory_dimensions*head_.number_of_samples
                            //   [kx,ky,kx,ky.....]        (for head_.trajectory_dimensions = 2)
                            //   [kx,ky,kz,kx,ky,kz,.....] (for head_.trajectory_dimensions = 3)

   float* data_;            //Actual data, elements = head_.number_of_samples*head_.active_channels*2 
                            //   [re,im,re,im,.....,re,im,re,im,.....,re,im,re,im,.....]
                            //    ---channel 1-------channel 2---------channel 3-----

 protected:
   void deleteData() {
	 if (traj_) {delete [] traj_; traj_ = 0;}
	 if (data_) {delete [] data_; data_ = 0;}
   }
 };

} //End of ISMRMRD namespace

#pragma pack(pop) //Restore old alignmen

#endif //ISMRMRD_H
