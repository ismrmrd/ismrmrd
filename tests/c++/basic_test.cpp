#include <iostream>
#include "ismrmrd.hpp"

int main (int args, char** argv) {

  using namespace ISMRMRD;
  
  AcquisitionHeader acqhdr;
  std::cout << "Version: " << acqhdr.version << std::endl;
  std::cout << "Flags 0: " << acqhdr.flags << std::endl;
  std::cout << "ACQ_FIRST_IN_SLICE: " << acqhdr.isFlagSet(ISMRMRD_ACQ_FIRST_IN_SLICE) << std::endl;
  acqhdr.setFlag(ISMRMRD_ACQ_FIRST_IN_SLICE);
  std::cout << "Flags 1: " << acqhdr.flags << std::endl;
  std::cout << "ACQ_FIRST_IN_SLICE: " << acqhdr.isFlagSet(ISMRMRD_ACQ_FIRST_IN_SLICE) << std::endl;
  acqhdr.clearFlag(ISMRMRD_ACQ_FIRST_IN_SLICE);
  std::cout << "Flags 2: " << acqhdr.flags << std::endl;
  std::cout << "ACQ_FIRST_IN_SLICE: " << acqhdr.isFlagSet(ISMRMRD_ACQ_FIRST_IN_SLICE) << std::endl;

  Acquisition acq;
  std::cout << "Version: " << acq.version() << std::endl;
  std::cout << "Flags 0: " << acq.flags() << std::endl;
  std::cout << "ACQ_FIRST_IN_SLICE: " << acq.isFlagSet(ISMRMRD_ACQ_FIRST_IN_SLICE) << std::endl;
  acq.setFlag(ISMRMRD_ACQ_FIRST_IN_SLICE);
  std::cout << "Flags 1: " << acq.flags() << std::endl;
  std::cout << "ACQ_FIRST_IN_SLICE: " << acq.isFlagSet(ISMRMRD_ACQ_FIRST_IN_SLICE) << std::endl;
  acq.clearFlag(ISMRMRD_ACQ_FIRST_IN_SLICE);
  std::cout << "Flags 2: " << acq.flags() << std::endl;
  std::cout << "ACQ_FIRST_IN_SLICE: " << acq.isFlagSet(ISMRMRD_ACQ_FIRST_IN_SLICE) << std::endl;
  acq.flags() = 64;
  std::cout << "Flags 3: " << acq.flags() << std::endl;
  std::cout << "ACQ_FIRST_IN_SLICE: " << acq.isFlagSet(ISMRMRD_ACQ_FIRST_IN_SLICE) << std::endl;
  

  return 0;
}
