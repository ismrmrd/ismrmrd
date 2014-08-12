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

  String parse_string(pugi::xml_node& n, const char* child) 
  {
    String r(n.child_value(child));
    if (r.size() == 0) throw std::runtime_error("Null length string");
    return r;
  }

  Optional<String> parse_optional_string(pugi::xml_node& n, const char* child)
  {
    String s(n.child_value(child));
    Optional<String> r;
    if (s.size()) r = s;
    return r;
  }

  Optional<float> parse_optional_float(pugi::xml_node& n, const char* child)
  {
    Optional<float> r;
    pugi::xml_node nc = n.child(child);
    if (nc) {
      r = std::atof(nc.child_value());
    }
    return r;
  }

  Optional<long> parse_optional_long(pugi::xml_node& n, const char* child) {
    Optional<long> r;
    pugi::xml_node nc = n.child(child);
    if (nc) {
      r = std::atoi(nc.child_value());
    }
    return r;
  }

  Optional<unsigned short> parse_optional_ushort(pugi::xml_node& n, const char* child) {
    Optional<unsigned short> r;
    pugi::xml_node nc = n.child(child);
    if (nc) {
      r = static_cast<unsigned short>(std::atoi(nc.child_value()));
    }
    return r;
  }

  Vector<float> parse_vector_float(pugi::xml_node& n, const char* child) 
  {
    Vector<float> r;
    
    pugi::xml_node nc = n.child(child);

    while (nc) {
      float f = std::atof(nc.child_value());
      r.push_back(f);
      nc = nc.next_sibling(child);
    }
    
    return r;
  }

  Vector<String> parse_vector_string(pugi::xml_node& n, const char* child)
  {
    Vector<String> r;
    pugi::xml_node nc = n.child(child);
    while (nc) {
      String s = nc.child_value();
      r.push_back(s);
      nc = nc.next_sibling(child);
    }
    return r;
  }

  Vector<UserParameterLong> parse_user_parameter_long(pugi::xml_node& n, const char* child) 
  {
    Vector<UserParameterLong> r;
    pugi::xml_node nc = n.child(child);
    while (nc) {
      UserParameterLong v;
      pugi::xml_node name = nc.child("name");
      pugi::xml_node value = nc.child("value");

      if (!name || !value) {
	throw std::runtime_error("Malformed user parameter (long)");
      }

      v.name = String(name.child_value());
      v.value = std::atoi(value.child_value());

      r.push_back(v);

      nc = nc.next_sibling(child);
    }
    return r;
  }

  Vector<UserParameterDouble> parse_user_parameter_double(pugi::xml_node& n, const char* child) 
  {
    Vector<UserParameterDouble> r;
    pugi::xml_node nc = n.child(child);
    while (nc) {
      UserParameterDouble v;
      pugi::xml_node name = nc.child("name");
      pugi::xml_node value = nc.child("value");

      if (!name || !value) {
	throw std::runtime_error("Malformed user parameter (double)");
      }

      char buffer[10000];
      memcpy(buffer,name.child_value(),strlen(name.child_value())+1);
      String tmp("BLAH");
      v.name = "BLAKKK";//tmp;//String("BLAH");//name.child_value());
      v.value = std::atof(value.child_value());

      r.push_back(v);

      nc = nc.next_sibling(child);
    }
   
    return r;
  }

  Vector<UserParameterString> parse_user_parameter_string(pugi::xml_node& n, const char* child) 
  {
    Vector<UserParameterString> r;
    pugi::xml_node nc = n.child(child);
    while (nc) {
      UserParameterString v;
      pugi::xml_node name = nc.child("name");
      pugi::xml_node value = nc.child("value");

      if (!name || !value) {
	throw std::runtime_error("Malformed user parameter (string)");
      }

      v.name = String(name.child_value());
      v.value = String(value.child_value());

      r.push_back(v);

      nc = nc.next_sibling(child);
    }
   
    return r;
  }

  //End of utility functions for deserializing header

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
      pugi::xml_node dicomParameters = root.child("dicomParameters");
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
	    e.trajectory = String(encoding.child_value("trajectory"));
	  }

	  pugi::xml_node trajectoryDescription = encoding.child("trajectoryDescription");
	  
	  if (trajectoryDescription) {
	    TrajectoryDescription traj;
	    try {
	      traj.identifier = parse_string(trajectoryDescription,"identifier");
	      traj.userParameterLong = 
		parse_user_parameter_long(trajectoryDescription, "userParameterLong");
	      traj.userParameterDouble = 
		parse_user_parameter_double(trajectoryDescription, "userParameterDouble");
	      traj.comment = parse_optional_string(trajectoryDescription, "comment");
	      e.trajectoryDescription = traj;
	    } catch (std::runtime_error& e) {
	      std::cout << "Error parsing trajectory description" << std::endl;
	      throw;
	    }
	    
	  }

	  h_.encoding.push_back(e);
	  encoding = encoding.next_sibling("encoding");
	}

      }

      if (subjectInformation) {
	SubjectInformation info;
	info.patientName = parse_optional_string(subjectInformation, "patientName");
	info.patientWeight_kg = parse_optional_float(subjectInformation, "patientWeight_kg");
	info.patientID = parse_optional_string(subjectInformation, "patientID");
	info.patientBirthdate = parse_optional_string(subjectInformation, "patientBirthdate");
	info.patientGender = parse_optional_string(subjectInformation, "patientGender");
	h_.subjectInformation = info;
      }

      if (studyInformation) {
	StudyInformation info;
	info.studyDate = parse_optional_string(studyInformation,"studyDate");
	info.studyTime = parse_optional_string(studyInformation,"studyTime");
	info.studyID = parse_optional_string(studyInformation,"studyID");
	info.accessionNumber = parse_optional_long(studyInformation,"accessionNumber");
	info.referringPhysicianName = parse_optional_string(studyInformation,"referringPhysicianName");
	info.studyDescription = parse_optional_string(studyInformation,"studyDescription");
	h_.studyInformation = info;
      }

      if (measurementInformation) {
	MeasurementInformation info;
	info.measurementID = parse_optional_string(measurementInformation,"measurementID");
	info.seriesDate = parse_optional_string(measurementInformation, "seriesDate");
	info.seriesTime = parse_optional_string(measurementInformation, "seriesTime");
	info.patientPosition = parse_string(measurementInformation, "patientPosition");
	info.initialSeriesNumber = parse_optional_long(measurementInformation, "initialSeriesNumber");
	info.protocolName = parse_optional_string(measurementInformation, "protocolName");
	info.seriesDescription = parse_optional_string(measurementInformation, "seriesDescription");
	
	pugi::xml_node measurementDependency = measurementInformation.child("measurementDependency");
	while (measurementDependency) {
	  try {
	    MeasurementDependency d;
	    d.dependencyID = parse_string(measurementDependency,"dependencyID");
	    d.dependencyType = parse_string(measurementDependency,"dependencyType");
	    info.measurementDepencency.push_back(d);
	  } catch (std::runtime_error& e) {
	    std::cout << "Error parsing measurement dependency: " << e.what() << std::endl;
	    throw;
	  } 
	  measurementDependency = measurementDependency.next_sibling("measurementDependency");
	}
	h_.measurementInformation = info;
      }

      if (acquisitionSystemInformation) {
	AcquisitionSystemInformation info;
	info.systemVendor = parse_optional_string(acquisitionSystemInformation, "systemVendor");
	info.systemModel = parse_optional_string(acquisitionSystemInformation, "systemModel");
	info.systemFieldStrength_T = parse_optional_float(acquisitionSystemInformation, "systemFieldStrength_T");
	info.relativeReceiverNoiseBandwidth = parse_optional_float(acquisitionSystemInformation, "relativeReceiverNoiseBandwidth");
	info.receiverChannels = parse_optional_ushort(acquisitionSystemInformation, "receiverChannels");
	info.institutionName = parse_optional_string(acquisitionSystemInformation, "institutionName");
	info.stationName = parse_optional_string(acquisitionSystemInformation, "stationName");

	h_.acquisitionSystemInformation = info;
      }

      if (parallelImaging) {
	ParallelImaging info;
	
	pugi::xml_node accelerationFactor = parallelImaging.child("accelerationFactor");
	if (!accelerationFactor) {
	  throw std::runtime_error("Unable to accelerationFactor section in parallelImaging");
	} else {
	  info.accelerationFactor.kspace_encoding_step_1 = static_cast<unsigned short>(std::atoi(accelerationFactor.child_value("kspace_encoding_step_1")));
	  info.accelerationFactor.kspace_encoding_step_2 = static_cast<unsigned short>(std::atoi(accelerationFactor.child_value("kspace_encoding_step_2")));
	}

	info.calibrationMode = parse_optional_string(parallelImaging,"calibrationMode");
	info.interleavingDimension = parse_optional_string(parallelImaging,"interleavingDimension");
	h_.parallelImaging = info;
      }

      if (sequenceParameters) {
	SequenceParameters p;
	p.TR = parse_vector_float(sequenceParameters,"TR");
	p.TE = parse_vector_float(sequenceParameters,"TE");
	p.TI = parse_vector_float(sequenceParameters,"TI");
	h_.sequenceParameters = p;
      }

      if (dicomParameters) { 
	DicomParameters p;
	p.studyInstanceUID = parse_string(dicomParameters,"studyInstanceUID");
	p.seriesInstanceUIDRoot = parse_optional_string(dicomParameters,"seriesInstanceUIDRoot");
	p.frameOfReference = parse_optional_string(dicomParameters,"frameOfReferenceUID");

	//This part of the schema is totally messed up and needs to be fixed, but for now we will just read it. 
	pugi::xml_node ri = dicomParameters.child("referencedImageSequemce");
	if (ri) {
	  pugi::xml_node ric = ri.child("referencedSOPInstanceUID");
	  while (ric) {
	    ReferencedImageSequence r;
	    r.referencedSOPInstanceUID = ric.child_value();
	    p.referencedImageSequence.push_back(r);
	    ric = ric.next_sibling("referenceSOPInstanceIUID");
	  }
	}
	
	pugi::xml_node mrimageModule = dicomParameters.child("MRImageModule");
	if (mrimageModule) {
	  MRImageModule m;
	  m.imageType = parse_optional_string(mrimageModule,"imageType");
	  m.scanningSequence = parse_optional_string(mrimageModule, "scanningSequence");
	  m.sequenceVartiant = parse_optional_string(mrimageModule, "sequenceVariant");
	  m.scanOptions = parse_optional_string(mrimageModule, "scanOptions");
	  m.mrAcquisitionType = parse_optional_string(mrimageModule, "mrAcquisitionType");
	  m.echoTrainLength = parse_optional_long(mrimageModule, "echoTrainLength");
	  m.triggerTime = parse_optional_float(mrimageModule, "triggerTime");
	  m.flipAngle_deg = parse_optional_float(mrimageModule, "flipAngle_deg");
	  m.freqEncodingDirection = parse_optional_string(mrimageModule, "freqEncodingDirection");
	  p.mrImageModule = m;
	}
	h_.dicomParameters = p;
      }


      if (userParameters) {
	UserParameters p;
 	//p.userParameterLong = parse_user_parameter_long(userParameters,"userParameterLong");
 	p.userParameterDouble = parse_user_parameter_double(userParameters,"userParameterDouble");
 	p.userParameterString = parse_user_parameter_string(userParameters,"userParameterString");
 	p.userParameterBase64 = parse_user_parameter_string(userParameters,"userParameterBase64");
	h_.userParameters = p;
      }

    } else {
      throw std::runtime_error("Root node 'ismrmrdHeader' not found");
    }

  }


  void serialize(std::ostream& o)
  {

  }


}
