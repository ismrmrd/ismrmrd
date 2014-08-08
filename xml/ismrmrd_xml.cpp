#include "ismrmrd_xml.h"
#include "pugixml.hpp"
#include <cstdlib>

namespace ISMRMRD
{
  IsmrmrdHeaderProxy::IsmrmrdHeaderProxy(const char* xml)
  {
    deserialize(xml);
  }

  IsmrmrdHeaderProxy::IsmrmrdHeaderProxy(const std::string& xml)
  {
    deserialize(xml.c_str());
  }

  IsmrmrdHeaderProxy::IsmrmrdHeaderProxy()
  {

  }
  
  IsmrmrdHeaderProxy::IsmrmrdHeaderProxy(const IsmrmrdHeader& h)
  {
    h_ = h;
  }
  
  IsmrmrdHeaderProxy& IsmrmrdHeaderProxy::operator=(const IsmrmrdHeader& h)
  {
    h_ = h;
    return *this;
  }
  
  void IsmrmrdHeaderProxy::deserialize(const char* xml) 
  {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load(xml);

    pugi::xml_node root = doc.child("ismrmrdHeader");

    if (root) {
      pugi::xml_node subjectInformation = root.child("subjectInformation");
      pugi::xml_node studyInformation = root.child("studyInformation");
      pugi::xml_node measurementInformation = root.child("measurementInformation");
      pugi::xml_node acquisitionSystemInformation = root.child("acquisitionSystemInformation");
      pugi::xml_node experimentalConditions = root.child("experimentalConditions");
      pugi::xml_node encoding = root.child("encoding");
      pugi::xml_node parallelImaging = root.child("parallelImaging");
      pugi::xml_node sequenceParameters = root.child("sequenceParameters");
      pugi::xml_node dicomeParameters = root.child("dicomParameters");
      pugi::xml_node userParameters = root.child("userParameters");
      
      //Parsing experimentalConditions
      if (!experimentalConditions) {
	throw std::runtime_error("experimentalConditions not defined in ismrmrdHeader");
      } else {
	ExperimentalConditions e;
	e.H1resonanceFrequency_Hz = std::atoi(experimentalConditions.child_value("H1resonanceFrequency_Hz"));
	h_.experimentalConditions = e;
      }

      
      //Parsing encoding section
      if (!encoding) {
	throw std::runtime_error("encoding section not found in ismrmrdHeader");
      } else {
	while (encoding) {
	  
	  encoding = encoding.next_sibling("encoding");
	}

      }

      if (subjectInformation) {

      }

      if (studyInformation) {

      }

      if (measurementInformation) {

      }

      if (acquisitionSystemInformation) {

      }


    } else {
      throw std::runtime_error("Root node 'ismrmrdHeader' not found");
    }

  }


  void serialize(std::ostream& o)
  {

  }


}
