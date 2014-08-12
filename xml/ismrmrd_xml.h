#include "ismrmrd_xml_export.h"


#include <cstddef>
#include <new> //For std::badalloc
#include <stdexcept> //For std::length_error
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>

/*
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

    const Optional& operator=(const T& v) {
      present_ = true;
      value_ = v;
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

  struct EXPORTISMRMRDXML SubjectInformation 
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
  };

  struct MeasurementDependency
  {
    std::string dependencyType;
    std::string dependencyID;
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
    std::vector<MeasurementDependency> measurementDepencency;
  };

  
  struct AcquisitionSystemInformation
  {
    Optional<std::string> systemVendor;
    Optional<std::string> systemModel;
    Optional<float> systemFieldStrength_T;
    Optional<float> relativeReceiverNoiseBandwidth;
    Optional<unsigned short> receiverChannels;
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

  struct Encoding
  {
    EncodingSpace encodedSpace;
    EncodingSpace reconSpace;
    EncodingLimits encodingLimits;
    std::string trajectory;
    Optional<TrajectoryDescription> trajectoryDescription;
  };

  struct SequenceParameters
  {
    std::vector<float> TR;
    std::vector<float> TE;
    std::vector<float> TI;
  };

  struct ReferencedImageSequence
  {
    std::string referencedSOPInstanceUID;
  };
  

  struct MRImageModule
  {
    Optional<std::string> imageType;
    Optional<std::string> scanningSequence;
    Optional<std::string> sequenceVartiant;
    Optional<std::string> scanOptions;
    Optional<std::string> mrAcquisitionType;
    Optional<long> echoTrainLength;
    Optional<float> triggerTime;
    Optional<float> flipAngle_deg;
    Optional<std::string> freqEncodingDirection;
  };

  struct DicomParameters
  {
    std::string studyInstanceUID;
    Optional<std::string> seriesInstanceUIDRoot;
    Optional<std::string> frameOfReference;
    std::vector<ReferencedImageSequence> referencedImageSequence;
    Optional<MRImageModule> mrImageModule;
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


  struct IsmrmrdHeader
  {
    Optional<SubjectInformation> subjectInformation;
    Optional<StudyInformation> studyInformation;
    Optional<MeasurementInformation> measurementInformation;
    Optional<AcquisitionSystemInformation> acquisitionSystemInformation;
    Optional<ExperimentalConditions> experimentalConditions;
    std::vector<Encoding> encoding;
    Optional<ParallelImaging> parallelImaging;
    Optional<SequenceParameters> sequenceParameters;
    Optional<DicomParameters> dicomParameters;
    Optional<UserParameters> userParameters;    
  };



  void deserialize(const char* xml, IsmrmrdHeader& h);
  void serialize(const IsmrmrdHeader& h, std::ostream& o);

  /*
  class IsmrmrdHeaderProxy 
  {

  public:

    //Constructors
    IsmrmrdHeaderProxy(const char* xml);
    IsmrmrdHeaderProxy(const std::string& xml);
    IsmrmrdHeaderProxy(); //Minimal Header

    //Copy constructor
    IsmrmrdHeaderProxy(const IsmrmrdHeader& h);
    
    //Assignment operator
    IsmrmrdHeaderProxy& operator=(const IsmrmrdHeader& h);

    void deserialize(const char* xml);

    void serialize(std::ostream& o);

    IsmrmrdHeader& h() {
      return h_;
    }

  protected:
    IsmrmrdHeader h_;
  };
  */


}
