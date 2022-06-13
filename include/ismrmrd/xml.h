/**
 * @file xml.h
 * @defgroup xml XML API
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

/**
 * @addtogroup xml
 * @{
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

    bool has_value() const noexcept {
        return present_;
    }


  T &value() &{
      if (!present_) {
          throw std::runtime_error("Access optional value, which has not been set");
      }
      return value_;
  }


  const T &value() const &{
      if (!present_) {
          throw std::runtime_error("Access optional value, which has not been set");
      }
      return value_;
  }

  T &&value() &&{
      if (!present_) {
          throw std::runtime_error("Access optional value, which has not been set");
      }
      return std::move(value_);
  }

  const T &&value() const &&{
      if (!present_) {
          throw std::runtime_error("Access optional value, which has not been set");
      }
      return std::move(value_);
  }

  T &get() & {
      return  this->value();
    }

    T&& get()&&{
        return this->value();
    }
     const T &get() const &  {
          return  this->value();
      }

     const  T&& get() const &&{
          return this->value();
      }
  template<class U>
  T value_or(U &&default_value) const &{
      return bool(*this) ? **this : static_cast<T>(std::forward<U>(default_value));
  }

  template<class U>
  T value_or(U &&default_value) &&{
      return bool(*this) ? std::move(**this) : static_cast<T>(std::forward<U>(default_value));
  }

    bool operator==(const Optional<T>& other) const {
      if (this->present_ && other.present_) return this->get() == *other;
      if ((!this->present_) && (!other.present_)) return true;
      return false;
    }

    bool operator==(const T& val) const {
      if (this->present_) return this->get() == val;
      return false; 
    }


  T& operator()() {
  return value();
}

    void set(const T& v) {
      present_ = true;
      value_ = v;
    }

  protected:
    bool present_;
    T value_;

  }; 

  struct threeDimensionalFloat
  {
    float x;
    float y;
    float z;
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
    Optional<threeDimensionalFloat> relativeTablePosition;
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
    Optional<std::string> deviceID;
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
      friend bool operator==(const EncodingLimits &lhs, const EncodingLimits &rhs);
      friend bool operator!=(const EncodingLimits &lhs, const EncodingLimits &rhs);
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

  struct Multiband{
      std::vector<float> spacing;
    float factor;
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
    Optional<Multiband> multiband;
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

    EXPORTISMRMRD std::ostream& operator<<(std::ostream & os, const IsmrmrdHeader&);

EXPORTISMRMRD bool operator==(const IsmrmrdHeader&, const IsmrmrdHeader&);
 EXPORTISMRMRD bool operator!=(const IsmrmrdHeader &lhs, const IsmrmrdHeader &rhs);
 EXPORTISMRMRD bool operator==(const SubjectInformation &lhs, const SubjectInformation &rhs);
 EXPORTISMRMRD bool operator!=(const SubjectInformation &lhs, const SubjectInformation &rhs);
 EXPORTISMRMRD bool operator==(const StudyInformation &lhs, const StudyInformation &rhs);
 EXPORTISMRMRD bool operator!=(const StudyInformation &lhs, const StudyInformation &rhs);
 EXPORTISMRMRD bool operator==(const ReferencedImageSequence &lhs, const ReferencedImageSequence &rhs);
 EXPORTISMRMRD bool operator!=(const ReferencedImageSequence &lhs, const ReferencedImageSequence &rhs);
 EXPORTISMRMRD bool operator==(const MeasurementInformation &lhs, const MeasurementInformation &rhs);
 EXPORTISMRMRD bool operator!=(const MeasurementInformation &lhs, const MeasurementInformation &rhs);
 EXPORTISMRMRD bool operator==(const CoilLabel &lhs, const CoilLabel &rhs);
 EXPORTISMRMRD bool operator!=(const CoilLabel &lhs, const CoilLabel &rhs);
 EXPORTISMRMRD bool operator==(const AcquisitionSystemInformation &lhs, const AcquisitionSystemInformation &rhs);
 EXPORTISMRMRD bool operator!=(const AcquisitionSystemInformation &lhs, const AcquisitionSystemInformation &rhs);
 EXPORTISMRMRD bool operator==(const ExperimentalConditions &lhs, const ExperimentalConditions &rhs);
 EXPORTISMRMRD bool operator!=(const ExperimentalConditions &lhs, const ExperimentalConditions &rhs);
 EXPORTISMRMRD bool operator==(const MatrixSize &lhs, const MatrixSize &rhs);
 EXPORTISMRMRD bool operator!=(const MatrixSize &lhs, const MatrixSize &rhs);
 EXPORTISMRMRD bool operator==(const FieldOfView_mm &lhs, const FieldOfView_mm &rhs);
 EXPORTISMRMRD bool operator!=(const FieldOfView_mm &lhs, const FieldOfView_mm &rhs);
 EXPORTISMRMRD bool operator==(const EncodingSpace &lhs, const EncodingSpace &rhs);
 EXPORTISMRMRD bool operator!=(const EncodingSpace &lhs, const EncodingSpace &rhs);
 EXPORTISMRMRD bool operator==(const Limit &lhs, const Limit &rhs);
 EXPORTISMRMRD bool operator!=(const Limit &lhs, const Limit &rhs);
 EXPORTISMRMRD bool operator==(const EncodingLimits &lhs, const EncodingLimits &rhs);
 EXPORTISMRMRD bool operator!=(const EncodingLimits &lhs, const EncodingLimits &rhs);
 EXPORTISMRMRD bool operator==(const UserParameterLong &lhs, const UserParameterLong &rhs);
 EXPORTISMRMRD bool operator!=(const UserParameterLong &lhs, const UserParameterLong &rhs);
 EXPORTISMRMRD bool operator==(const UserParameterDouble &lhs, const UserParameterDouble &rhs);
 EXPORTISMRMRD bool operator!=(const UserParameterDouble &lhs, const UserParameterDouble &rhs);
 EXPORTISMRMRD bool operator==(const UserParameterString &lhs, const UserParameterString &rhs);
 EXPORTISMRMRD bool operator!=(const UserParameterString &lhs, const UserParameterString &rhs);
 EXPORTISMRMRD bool operator==(const UserParameters &lhs, const UserParameters &rhs);
 EXPORTISMRMRD bool operator!=(const UserParameters &lhs, const UserParameters &rhs);
 EXPORTISMRMRD bool operator==(const TrajectoryDescription &lhs, const TrajectoryDescription &rhs);
 EXPORTISMRMRD bool operator!=(const TrajectoryDescription &lhs, const TrajectoryDescription &rhs);
 EXPORTISMRMRD bool operator==(const AccelerationFactor &lhs, const AccelerationFactor &rhs);
 EXPORTISMRMRD bool operator!=(const AccelerationFactor &lhs, const AccelerationFactor &rhs);
 EXPORTISMRMRD bool operator==(const ParallelImaging &lhs, const ParallelImaging &rhs);
 EXPORTISMRMRD bool operator!=(const ParallelImaging &lhs, const ParallelImaging &rhs);
 EXPORTISMRMRD bool operator==(const Multiband &lhs, const Multiband &rhs);
 EXPORTISMRMRD bool operator!=(const Multiband &lhs, const Multiband &rhs);
 EXPORTISMRMRD bool operator==(const Encoding &lhs, const Encoding &rhs);
 EXPORTISMRMRD bool operator!=(const Encoding &lhs, const Encoding &rhs);
 EXPORTISMRMRD bool operator==(const SequenceParameters &lhs, const SequenceParameters &rhs);
 EXPORTISMRMRD bool operator!=(const SequenceParameters &lhs, const SequenceParameters &rhs);
 EXPORTISMRMRD bool operator==(const WaveformInformation &lhs, const WaveformInformation &rhs);
 EXPORTISMRMRD bool operator!=(const WaveformInformation &lhs, const WaveformInformation &rhs);
 EXPORTISMRMRD bool operator==(const threeDimensionalFloat &lhs, const threeDimensionalFloat &rhs);
 EXPORTISMRMRD bool operator!=(const threeDimensionalFloat &lhs, const threeDimensionalFloat &rhs);
 EXPORTISMRMRD bool operator==(const MeasurementDependency &lhs, const MeasurementDependency &rhs);
 EXPORTISMRMRD bool operator!=(const MeasurementDependency &lhs, const MeasurementDependency &rhs);



/** @} */

}

#endif //ISMRMRDXML_H
