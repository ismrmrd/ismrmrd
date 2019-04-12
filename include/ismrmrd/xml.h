/**
 * @file xml.h
 * @defgroup xml XML API
 * @{
 */

#ifndef ISMRMRDXML_H
#define ISMRMRDXML_H

#include "ismrmrd/export.h"

#include <cstddef>
#include <new> //For std::badalloc
#include <stdexcept> //For std::length_error
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>

/**
  TODO LIST

  - Add a date time class
  - Add enumeration class

 */


namespace ISMRMRD
{

  template <typename T> class Optional
  {
  public:
    Optional()
      : present_(false)
    {

    }

    Optional(const T&v) {
      present_ = true;
      value_ = v;      
    }

	Optional& operator=(const Optional& o) {
		present_ = o.present_;
		if (present_)
			value_ = o.value_;	
		return *this;
	}

    Optional& operator=(const T& v) {
      present_ = true;
      value_ = v;
      return *this;
    }

     T* operator->() {
      return &value_;
    }

    T& operator*() {
      return value_;
    }

    const T* operator->() const {
      return &value_;
    }

    const T& operator*() const {
      return value_;
    }

    operator bool() const {
      return present_;
    }

    bool is_present() const {
      return present_;
    }

    T& get() {
      if (!present_) {
	throw std::runtime_error("Access optional value, which has not been set");
      }
      return value_;
    }


    const T& get() const  {
      if (!present_) {
	throw std::runtime_error("Access optional value, which has not been set");
      }
      return value_;
    }

    
    T& operator()() {
      return get();
    }

    void set(const T& v) {
      present_ = true;
      value_ = v;
    }

  protected:
    bool present_;
    T value_;

  }; 

  struct SubjectInformation 
  {
    Optional<std::string> patientName;
    Optional<float> patientWeight_kg;
    Optional<std::string> patientID;
    Optional<std::string> patientBirthdate;
    Optional<std::string> patientGender;
  };

  struct StudyInformation
  {
    Optional<std::string> studyDate;
    Optional<std::string> studyTime;
    Optional<std::string> studyID;
    Optional<long> accessionNumber;
    Optional<std::string> referringPhysicianName;
    Optional<std::string> studyDescription;
    Optional<std::string> studyInstanceUID;
  };

  struct MeasurementDependency
  {
    std::string dependencyType;
    std::string measurementID;
  };

  struct ReferencedImageSequence
  {
    std::string referencedSOPInstanceUID;
  };
  
  struct MeasurementInformation
  {
    Optional<std::string> measurementID;
    Optional<std::string> seriesDate;
    Optional<std::string> seriesTime;
    std::string patientPosition;
    Optional<long int> initialSeriesNumber;
    Optional<std::string> protocolName;
    Optional<std::string> seriesDescription;
    std::vector<MeasurementDependency> measurementDependency;
    Optional<std::string> seriesInstanceUIDRoot;
    Optional<std::string> frameOfReferenceUID;
    std::vector<ReferencedImageSequence> referencedImageSequence;
  };

  struct CoilLabel
  {
    unsigned short coilNumber;
    std::string coilName;
  };
  
  struct AcquisitionSystemInformation
  {
    Optional<std::string> systemVendor;
    Optional<std::string> systemModel;
    Optional<float> systemFieldStrength_T;
    Optional<float> relativeReceiverNoiseBandwidth;
    Optional<unsigned short> receiverChannels;
    std::vector<CoilLabel> coilLabel;
    Optional<std::string> institutionName;
    Optional<std::string> stationName;
  };


  struct ExperimentalConditions
  {
    long int H1resonanceFrequency_Hz;
  };

  struct MatrixSize
  {
    MatrixSize()
    : x(1)
    , y(1)
    , z(1)
    {

    }
    
    MatrixSize(unsigned short x, unsigned short y)
    : x(x)
    , y(y)
    , z(1)
    {

    }
    
    MatrixSize(unsigned short x, unsigned short y, unsigned short z)
    : x(x)
    , y(y)
    , z(z)
    {

    }
    
    unsigned short x;
    unsigned short y;
    unsigned short z;
  };

  struct FieldOfView_mm
  {
    float x;
    float y;
    float z;
  };

  struct EncodingSpace
  {
    MatrixSize matrixSize;
    FieldOfView_mm fieldOfView_mm;
  };


  struct Limit
  {
    Limit() 
    : minimum(0)
    , maximum(0)
    , center(0)
    {

    }
    
    Limit(unsigned short minimum, unsigned short maximum, unsigned short center) 
    : minimum(minimum)
    , maximum(maximum)
    , center(center)
    {

    }
    
    unsigned short minimum;
    unsigned short maximum;
    unsigned short center;
  };

  struct EncodingLimits
  {
    Optional<Limit> kspace_encoding_step_0;
    Optional<Limit> kspace_encoding_step_1;
    Optional<Limit> kspace_encoding_step_2;
    Optional<Limit> average;
    Optional<Limit> slice;
    Optional<Limit> contrast;
    Optional<Limit> phase;
    Optional<Limit> repetition;
    Optional<Limit> set;
    Optional<Limit> segment;
  };


  struct UserParameterLong
  {
    std::string name;
    long value;
  };

  struct UserParameterDouble
  {
    std::string name;
    double value;
  };
  
  struct UserParameterString

  {
    std::string name;
    std::string value;
  };

  struct UserParameters
  {
    std::vector<UserParameterLong> userParameterLong;
    std::vector<UserParameterDouble> userParameterDouble;
    std::vector<UserParameterString> userParameterString;
    std::vector<UserParameterString> userParameterBase64;
  };

  struct TrajectoryDescription
  {
    std::string identifier;
    std::vector<UserParameterLong> userParameterLong;
    std::vector<UserParameterDouble> userParameterDouble;
    Optional<std::string> comment; 
  };

  struct AccelerationFactor
  {
    unsigned short kspace_encoding_step_1;
    unsigned short kspace_encoding_step_2;
  };

  struct ParallelImaging
  {
    AccelerationFactor accelerationFactor;
    Optional<std::string> calibrationMode;
    Optional<std::string> interleavingDimension;
  };

  enum class TrajectoryType {
      CARTESIAN,
      EPI,
      RADIAL,
      GOLDENANGLE,
      SPIRAL,
      OTHER
  };

  struct Encoding
  {
    EncodingSpace encodedSpace;
    EncodingSpace reconSpace;
    EncodingLimits encodingLimits;
    TrajectoryType trajectory;
    Optional<TrajectoryDescription> trajectoryDescription;
    Optional<ParallelImaging> parallelImaging;
    Optional<long> echoTrainLength;
  };

  struct SequenceParameters
  {
    Optional<std::vector<float> > TR;
    Optional<std::vector<float> > TE;
    Optional<std::vector<float> > TI;
    Optional<std::vector<float> > flipAngle_deg;
    Optional<std::string> sequence_type;
    Optional<std::vector<float> > echo_spacing;
  };

  enum class WaveformType {
      ECG,
      PULSE,
      RESPIRATORY,
      TRIGGER,
      GRADIENTWAVEFORM,
      OTHER
  };


  struct WaveformInformation{
      std::string waveformName;
      WaveformType waveformType;
      Optional<UserParameters> userParameters;
  };

  struct IsmrmrdHeader
  {
    Optional<long> version;
    Optional<SubjectInformation> subjectInformation;
    Optional<StudyInformation> studyInformation;
    Optional<MeasurementInformation> measurementInformation;
    Optional<AcquisitionSystemInformation> acquisitionSystemInformation;
    ExperimentalConditions experimentalConditions;
    std::vector<Encoding> encoding;
    Optional<SequenceParameters> sequenceParameters;
    Optional<UserParameters> userParameters;
    std::vector<WaveformInformation> waveformInformation;
  };



  EXPORTISMRMRD void deserialize(const char* xml, IsmrmrdHeader& h);
  EXPORTISMRMRD void serialize(const IsmrmrdHeader& h, std::ostream& o);
}

/** @} */
#endif //ISMRMRDXML_H
