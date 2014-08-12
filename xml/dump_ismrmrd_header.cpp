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
  std::cout << "XML: \n" << *xml << std::endl;

  ISMRMRD::IsmrmrdHeaderProxy hp(xml->c_str());

  std::cout << "Resonance frequency: " << hp.h().experimentalConditions.get().H1resonanceFrequency_Hz << std::endl;
  std::cout << "TR: " << hp.h().sequenceParameters.get().TR[0] << std::endl;
  //for (size_t i = 0; i < hp.h().userParameters.get().userParameterLong.size(); i++) {
    //std::cout << "UserLong: " << hp.h().userParameters.get().userParameterLong[i].name.size() << std::endl;
    /*
    std::cout << "UserLong: " << hp.h().userParameters.get().userParameterLong[i].name.c_str() << ", " 
	      << hp.h().userParameters.get().userParameterLong[i].value << std::endl;
    */
  //}
  return 0;
}
