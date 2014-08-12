#include <iostream>
#include "ismrmrd.h"
#include "ismrmrd_hdf5.h"
#include "ismrmrd_xml.h"

int main(int argc, char** argv)
{

  if (argc < 2) {
    std::cout << "Usage:\n\tdump_ismrmrd_header <ISMRMRD_FILE_NAME>\n" << std::endl;
    return -1;
  }

  std::string filename(argv[1]);

  std::cout << "ISMRMRD Header Dump: " << filename << std::endl;

  ISMRMRD::IsmrmrdDataset d(filename.c_str(),"/dataset");
  
  boost::shared_ptr<std::string> xml = d.readHeader();
  //std::cout << "XML: \n" << *xml << std::endl;

  ISMRMRD::IsmrmrdHeader h;
  deserialize(xml->c_str(),h);

  std::cout << "Resonance frequency: " << h.experimentalConditions.get().H1resonanceFrequency_Hz << std::endl;
  std::cout << "TR: " << h.sequenceParameters.get().TR[0] << std::endl;
  if (h.userParameters) {
    std::cout << "User parameters found" << std::endl;
    for (size_t i = 0; i < h.userParameters->userParameterLong.size(); i++) {
      std::cout << "UserLong: " << h.userParameters->userParameterLong[i].name.c_str() << ", " 
		<< h.userParameters->userParameterLong[i].value << std::endl;
    }
  }

  serialize(h,std::cout);
  return 0;
}
