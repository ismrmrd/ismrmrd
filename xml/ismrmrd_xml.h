#include "ismrmrd_xml_export.h"


#include <cstddef>
#include <new> //For std::badalloc
#include <stdexcept> //For std::length_error
#include <stdio.h>
#include <string.h>
#include <iostream>


/*
  TODO LIST

  - Add a date time class
  - Add enumeration class

 */


namespace ISMRMRD
{

  template <typename T> class Vector
  {

  public:

    Vector()
      : v_(0)
      , len_(0)
      , siz_(0)
      {} 

    Vector(const T* v, size_t len) {
      allocate(len);
      copy(v,len);
      siz_ = len;
    }

    Vector(size_t len)
      : v_(0)
      , len_(0)
      , siz_(0)
    {
      allocate(len);
      siz_ = len;
    }

    Vector(size_t len, const T& val) 
      : Vector()
    { 
      allocate(len);
      siz_ = len;
      for (size_t i = 0; i < siz_; i++) v_[i] = val;
    }

    //Copy constructor
    Vector(const Vector& v) {
      allocate(v.len_);
      len_ = v.len_;
      siz_ = v.siz_;
      copy(v.v_,len_);      
    }

    ///Asignment operator
    Vector& operator=(const Vector& v) {
      allocate(v.len_);
      len_ = v.len_;
      siz_ = v.siz_;
      copy(v.v_,len_);
    }


    virtual ~Vector() {
      if (v_) delete [] v_;
    }

    void push_back(const T& item) {
      if ((siz_+1) > len_) {
	allocate(len_ + 5); //We will grow by 5 items at a time
      }
      v_[siz_++] = item; //Copy item to array
    }

    T& operator[](size_t index) {
      return v_[index];
    }

    size_t size() {
      return siz_;
    }

  protected:
    T* v_;
    size_t len_; ///Allocated length
    size_t siz_; ///Size of vector

    void allocate(size_t len) {
      T* tmp_v = v_;
      if (len != len_) {
	//Allocate new array
	try {
	  v_ = new T[len];
	} catch (std::bad_alloc&) {
	  std::cout << "Error allocating memory in IsmrmrdXmlString" << std::endl;
	  throw;
	}

	//Copy old content if applicable
	if (tmp_v) {
	  memcpy(v_, tmp_v, (len < len_ ? len : len_)*sizeof(T));
	  delete [] tmp_v;
	}

	len_ = len;
      }
    }

    void copy(const T* v, size_t len) {
      if (len > len_) {
	throw std::length_error("Invalid length array specified for copy in IsmrmrdXmlString");
      }
      memcpy(v_,v,len*sizeof(T));
    } 

  };

  /**

     Simple string container class. The class is used for returning string values to avoid using std::string 
     which can cause problems when exported from a DLL on Windows. 

   */
  class String : public Vector<char>
  {
    typedef Vector<char> inherited;

  public:
    ///Default constructor
    String() 
    {
      inherited();
    }
    

    ///Construct taking null terminated string
    String(const char* s) 
    {
      inherited(s,strlen(s)+1);
    }
    
    ///Construct taking length of string as input
    String(const char* s, size_t length) 
    {
      inherited(length+1);
      copy(s,length);
      v_[length-1] = '\0';
    }

    ///Get a const pointer to the string
    const char* c_str() {
      return &v_[0];
    }

    ///Set the string
    void  set(const char* s) {
      size_t l = strlen(s);
      allocate(l+1);
      copy(s,l);
      v_[len_-1] = '\0';
      siz_ = len_;
    }
      
    ///Set the string with specified length
    void  set(const char* s, size_t len)
    {
      allocate(len+1);
      copy(s,len);
      v_[len_-1] = '\0';
      siz_ = len_;
    }

    ///Get the length of the string
    size_t size() {
      return inherited::size()-1;
    }
  };


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

    bool is_present() {
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
    Optional<String> patientName;
    Optional<float> patientWeight_kg;
    Optional<String> patientID;
    Optional<String> patientBirthDate;
    Optional<String> patientGender;
  };

  struct StudyInformation
  {
    Optional<String> studyDate;
    Optional<String> studyTime;
    Optional<String> studyID;
    Optional<long int> accessionNumber;
    Optional<String> referringPhysicianName;
    Optional<String> studyDescription;
  };

  struct MeasurementDependency
  {
    String dependencyType;
    String dependencyID;
  };

  struct MeasurementInformation
  {
    Optional<String> measurementID;
    Optional<String> seriesDate;
    Optional<String> seriesTime;
    String patientPosition;
    Optional<long int> initialSeriesNumber;
    Optional<String> protocolName;
    Optional<String> seriesDescription;
    Vector<MeasurementDependency> measurementDepencency;
  };

  
  struct AcquisitionSystemInformation
  {
    Optional<String> systemVendor;
    Optional<String> systemModel;
    Optional<float> systemFieldStrength_T;
    Optional<float> relativeReceiverNoiseBandwidth;
    Optional<unsigned short> receiverChannels;
    Optional<String> institutionName;
    Optional<String> stationName;
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
    String name;
    long value;
  };

  struct UserParameterDouble
  {
    String name;
    double value;
  };
  
  struct UserParameterString
  {
    String name;
    String value;
  };

  struct UserParameterBase64
  {
    String name;
    String value;
  };

  struct UserParameters
  {
    Vector<UserParameterLong> userParameterLong;
    Vector<UserParameterDouble> userParameterDouble;
    Vector<UserParameterString> userParameterString;
    Vector<UserParameterBase64> userParameterBase64;
  };

  struct TrajectoryDescription
  {
    String identifier;
    Vector<UserParameterLong> userParameterLong;
    Vector<UserParameterDouble> userParameterDouble;
    Optional<String> comment; 
  };

  struct Encoding
  {
    EncodingSpace encodedSpace;
    EncodingSpace reconSpace;
    EncodingLimits encodingLimits;
    String trajectory;
    Optional<TrajectoryDescription> trajectoryDescription;
  };

  struct SequenceParameters
  {
    Vector<float> TR;
    Vector<float> TE;
    Vector<float> TI;
  };

  struct ReferencedImageSequence
  {
    String referencedSOPInstanceUID;
  };
  

  struct MRImageModule
  {
    Optional<String> imageType;
    Optional<String> scanningSequence;
    Optional<String> sequenceVartiant;
    Optional<String> scanOptions;
    Optional<String> mrAcquisitionType;
    Optional<long> echoTrainLength;
    Optional<float> triggerTime;
    Optional<float> flipAngle_deg;
    Optional<String> freqEncodingDirection;
  };

  struct DicomParameters
  {
    String studyInstanceUID;
    Optional<String> seriesInstanceUIDRoot;
    Optional<String> frameOfReference;
    Vector<ReferencedImageSequence> referencedImageSequence;
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
    Optional<String> calibrationMode;
    Optional<String> interleavingDimension;
  };


  struct IsmrmrdHeader
  {
    Optional<SubjectInformation> subjectInformation;
    Optional<StudyInformation> studyInformation;
    Optional<MeasurementInformation> measurementInformation;
    Optional<AcquisitionSystemInformation> acquisitionSystemInformation;
    Optional<ExperimentalConditions> experimentalConditions;
    Vector<Encoding> encoding;
    Optional<ParallelImaging> parallelImaging;
    Optional<SequenceParameters> sequenceParameters;
    Optional<DicomParameters> dicomParameters;
    Optional<UserParameters> userParameters;    
  };



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


}
