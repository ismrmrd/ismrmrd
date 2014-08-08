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
  
  //Utility Functions for deserializing Header
  EncodingSpace parse_encoding_space(pugi::xml_node& n, const char* child) 
  {
    EncodingSpace e;
    pugi::xml_node encodingSpace = n.child(child);
    pugi::xml_node matrixSize = encodingSpace.child("matrixSize");
    pugi::xml_node fieldOfView_mm = encodingSpace.child("fieldOfView_mm");
    
    if (!matrixSize) {
      throw std::runtime_error("matrixSize not found in encodingSpace");
    } else {
      e.matrixSize.x = std::atoi(matrixSize.child_value("x"));
      e.matrixSize.y = std::atoi(matrixSize.child_value("y"));
      e.matrixSize.z = std::atoi(matrixSize.child_value("z"));
    }

    if (!fieldOfView_mm) {
      throw std::runtime_error("fieldOfView_mm not found in encodingSpace");
    } else {
      e.fieldOfView_mm.x = std::atoi(fieldOfView_mm.child_value("x"));
      e.fieldOfView_mm.y = std::atoi(fieldOfView_mm.child_value("y"));
      e.fieldOfView_mm.z = std::atoi(fieldOfView_mm.child_value("z"));
    }

    return e;
  }
  
  Optional<Limit> parse_encoding_limit(pugi::xml_node& n, const char* child) 
  {
    Optional<Limit> o;
    pugi::xml_node nc = n.child(child);
    
    if (nc) {
      Limit l;
      l.minimum = std::atoi(nc.child_value("minimum"));
      l.maximum = std::atoi(nc.child_value("maximum"));
      l.center = std::atoi(nc.child_value("center"));
      o = l;
    }

    return o;
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
	  Encoding e;
	  
	  try {
	    e.encodedSpace = parse_encoding_space(encoding,"encodedSpace");
	    e.reconSpace = parse_encoding_space(encoding,"reconSpace");
	  } catch (std::runtime_error& e) {
	    std::cout << "Unable to parse encoding section: " << e.what() << std::endl;
	    throw;
	  }

	  pugi::xml_node encodingLimits = encoding.child("encodingLimits");
	  
	  if (!encodingLimits) {
	    throw std::runtime_error("encodingLimits not found in encoding section");
	  } else {
	    e.encodingLimits.kspace_encoding_step_0 = parse_encoding_limit(encodingLimits,"kspace_encoding_step_0");
	    e.encodingLimits.kspace_encoding_step_1 = parse_encoding_limit(encodingLimits,"kspace_encoding_step_1");
	    e.encodingLimits.kspace_encoding_step_2 = parse_encoding_limit(encodingLimits,"kspace_encoding_step_2");
	    e.encodingLimits.average                = parse_encoding_limit(encodingLimits,"average");
	    e.encodingLimits.slice                  = parse_encoding_limit(encodingLimits,"slice");
	    e.encodingLimits.contrast               = parse_encoding_limit(encodingLimits,"contrast");
	    e.encodingLimits.phase                  = parse_encoding_limit(encodingLimits,"phase");
	    e.encodingLimits.repetition             = parse_encoding_limit(encodingLimits,"repetition");
	    e.encodingLimits.set                    = parse_encoding_limit(encodingLimits,"set");
	    e.encodingLimits.segment                = parse_encoding_limit(encodingLimits,"segment");
	  }
	  
	  pugi::xml_node trajectory = encoding.child("trajectory");
	  if (!trajectory) {
	    throw std::runtime_error("trajectory not found in encoding section");
	  } else {
	    e.trajectory = String(trajectory.value());
	  }

	  pugi::xml_node trajectoryDescription = encoding.child("trajectoryDescription");
	  
	  if (trajectoryDescription) {
	    
	  }

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
