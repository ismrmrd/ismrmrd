#include "ismrmrd_xml_export.h"

#include <cstddef>

namespace ISMRMRD
{

  /**

     Simple string container class. The class is used for returning string values to avoid using std::string 
     which can cause problems when exported from a DLL on Windows. 

   */
  class EXPORTISMRMRDXML IsmrmrdXmlString
  {
  public:
    ///Default constructor
    IsmrmrdXmlString();

    ///Construct taking null terminated string
    IsmrmrdXmlString(const char* s);

    ///Construct taking length of string as input
    IsmrmrdXmlString(const char* s, size_t length);

    ///Destructor
    ~IsmrmrdXmlString();

    ///Copy constructor
    IsmrmrdXmlString(const IsmrmrdXmlString& s);

    ///Asignment operator
    IsmrmrdXmlString& operator=(const IsmrmrdXmlString& s);

    ///Get a const pointer to the string
    const char* c_str();

    ///Set the string
    void  set(const char* s);
      
    ///Set the string with specified length
    void  set(const char* s, size_t len);

    ///Get the length of the string
    size_t size();


  protected:
    char* string_;
    size_t strlen_;

    void allocate(size_t len);
    void copy_string(const char* s, size_t len);

  };


  struct EXPORTISMRMRDXML SubjectInformation 
  {
    IsmrmrdXmlString patientName;
    float patientWeight_kg;
    IsmrmrdXmlString patientID;
    IsmrmrdXmlString patientBirthDate;
    IsmrmrdXmlString patientGender;
  };

  struct StudyInformation
  {
    IsmrmrdXmlString studyDate;
    IsmrmrdXmlString studyTime;
    IsmrmrdXmlString studyID;
    long int accessionNumber;
    IsmrmrdXmlString referringPhysicianName;
    IsmrmrdXmlString studyDescription;
  };

  struct MeasurementInformation
  {
    IsmrmrdXmlString measurementID;
    IsmrmrdXmlString seriesDate;
    IsmrmrdXmlString seriesTime;
    IsmrmrdXmlString patientPosition;
    long int initialSeriesNumber;
    IsmrmrdXmlString protocolName;
    IsmrmrdXmlString seriesDescription;
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
