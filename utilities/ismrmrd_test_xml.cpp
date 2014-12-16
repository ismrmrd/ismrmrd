#include <iostream>
#include <fstream>
#include <streambuf>
#include "ismrmrd/xml.h"
#include "pugixml.hpp"

int main(int argc, char** argv)
{

  if (argc < 2) {
    std::cout << "Usage:\n\tismrmrd_test_xml <ismrmrd_xml_header.xml>\n" << std::endl;
    return -1;
  }

  std::string filename(argv[1]);

  std::cout << "ISMRMRD Header: " << filename << std::endl;

  std::ifstream t(filename.c_str());
  std::string xml((std::istreambuf_iterator<char>(t)),
		  std::istreambuf_iterator<char>());

  ISMRMRD::IsmrmrdHeader h;
  deserialize(xml.c_str(),h);

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load(xml.c_str());
  if (!result) {
    std::cout << "Unable to load XML document using pugixml parser" << std::endl;
  }

  std::ofstream raw("raw.xml");
  std::ofstream proc("processed.xml");
  

  doc.save(raw);
  serialize(h,proc);

  /*
  deserialize(xml->c_str(),h);

  std::cout << "Resonance frequency: " << h.experimentalConditions.H1resonanceFrequency_Hz << std::endl;
  std::cout << "TR: " << h.sequenceParameters.get().TR[0] << std::endl;
  if (h.userParameters) {
    std::cout << "User parameters found" << std::endl;
    for (size_t i = 0; i < h.userParameters->userParameterLong.size(); i++) {
      std::cout << "UserLong: " << h.userParameters->userParameterLong[i].name.c_str() << ", " 
		<< h.userParameters->userParameterLong[i].value << std::endl;
    }
  }

  serialize(h,std::cout);
  */
  return 0;
}
