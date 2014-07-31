#include "ismrmrd_xml_export.h"


#include <cstddef>
#include <new> //For std::badalloc
#include <stdexcept> //For std::length_error
#include <stdio.h>
#include <string.h>
#include <iostream>

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
      copy(v_,v.v_,len_);      
    }

    ///Asignment operator
    Vector& operator=(const Vector& v) {
      allocate(v.len_);
      len_ = v.len_;
      siz_ = v.siz_;
      copy(v_,v.v_,len_);
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
  class EXPORTISMRMRDXML String : public Vector<char>
  {
    typedef Vector<char> inherited;

  public:
    ///Default constructor
    String();

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


  struct EXPORTISMRMRDXML SubjectInformation 
  {
    String patientName;
    float patientWeight_kg;
    String patientID;
    String patientBirthDate;
    String patientGender;
  };

  struct StudyInformation
  {
    String studyDate;
    String studyTime;
    String studyID;
    long int accessionNumber;
    String referringPhysicianName;
    String studyDescription;
  };

  struct MeasurementDependency
  {
    String dependencyType;
    String dependencyID;
  };

  struct MeasurementInformation
  {
    String measurementID;
    String seriesDate;
    String seriesTime;
    String patientPosition;
    long int initialSeriesNumber;
    String protocolName;
    String seriesDescription;
    Vector<MeasurementDependency> measurementDepencency;
  };

  


  /*
  //Should this actually be the name. 
  //Xml in the name is bad but I wanted to avoid confusion with the AcquisitionHeader
  class IsmrmrdXmlHeader 
  {

  public:

    //Constructors
    IsmrmrdXmlHeader(const char* xml);
    IsmrmrdXmlHeader(const std::string& xml);
    IsmrmrdXmlHeader(); //Minimal Header

    //Copy constructor
    IsmrmrdXmlHeader(const IsmrmrdXmlHeader& h);
    
    //Assignment operator
    IsmrmrdXmlHeader& operator=(const IsmrmrdXmlHeader& h);

    

  protected:
    
    
  };
  */


}
