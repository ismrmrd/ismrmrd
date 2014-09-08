#include <iostream>
#include "ismrmrd.h"
#include "ismrmrd_xml.h"
#include "ismrmrd_dataset.h"

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

  // The C++ AcquisitionHeader can be mem copied from the C struct ISMRMRD_AcquisitionHeader
  ISMRMRD_AcquisitionHeader c_acqhdr;
  ismrmrd_init_acquisition_header(&c_acqhdr);
  c_acqhdr.flags = 64;

  AcquisitionHeader acqhdr2;
  memcpy(&acqhdr2, &c_acqhdr, sizeof(AcquisitionHeader));
  std::cout << "AcquisitionHeader copy: flags: " << acqhdr2.flags << std::endl;
  
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
  std::cout << "Number of samples: " << acq.number_of_samples() << std::endl;
  acq.number_of_samples(64);
  std::cout << "Number of samples: " << acq.number_of_samples() << std::endl;
  
  Acquisition acq2(acq);
  std::cout << "Acquisition flags: " << acq.flags() << "    Acquisition copy flags: " << acq2.flags() << std::endl;
  std::cout << "Acquisition ACQ_FIRST_IN_SLICE: " << acq.isFlagSet(ISMRMRD_ACQ_FIRST_IN_SLICE) << std::endl;
  std::cout << "Acquisition copy ACQ_FIRST_IN_SLICE: " << acq2.isFlagSet(ISMRMRD_ACQ_FIRST_IN_SLICE) << std::endl;
  std::cout << "Acquisition nsamp: " << acq.number_of_samples() << "    Acquisition copy nsamp: " << acq2.number_of_samples() << std::endl;
 
  // How to use the C++ Acquisition as a wrapper to the C struct ISMRMRD_Acquistion
  ISMRMRD_Acquisition c_acq;
  ismrmrd_init_acquisition(&c_acq);
  c_acq.head.number_of_samples = 128;
  c_acq.head.active_channels = 4;
  ismrmrd_make_consistent_acquisition(&c_acq);
  for (int k=0; k<c_acq.head.number_of_samples; k++) {
      for (int c=0; c<c_acq.head.active_channels; c++) {
          c_acq.data[k*c_acq.head.active_channels + c] = std::complex<float>(1.0, 1.0);
      }
  }
  ismrmrd_set_flag(&(c_acq.head.flags), ISMRMRD_ACQ_FIRST_IN_SLICE);
  Acquisition *acq3 = reinterpret_cast<Acquisition *>(&c_acq);
  std::cout << "C Acquisition nsamp: " << c_acq.head.number_of_samples << "    C++ Acquisition wrapper nsamp: " << acq3->number_of_samples() << std::endl;
  std::cout << "C Acquisition data[4]: " << c_acq.data[4].real() << "      C++ Acquisition wrapper data[4]: " << acq3->getData()[4].real() << std::endl;

  // Open an existing file
  Dataset dataset1 = Dataset("myfile.h5", "/dataset", false);

  // Open a second dataset
  Dataset dataset2 = Dataset("myfile2.h5", "/dataset", true);
  
  // Read the header from file 1
  std::string xmlstring;
  dataset1.readHeader(xmlstring);
  std::cout << "XML Header: " << xmlstring << std::endl;

  // Write the header to file 2
  dataset2.writeHeader(xmlstring);
  
  // Read the number of acquisitions
  unsigned long numacq = dataset1.getNumberOfAcquisitions();
  std::cout << "Number of acquisitions: " << numacq << std::endl;
  
  // Read the first few acquisitions
  if (numacq > 3) { numacq=3; }

  for (unsigned long n = 0; n<numacq; n++) {
      Acquisition * acqref = dataset1.readAcquisition(n);
      std::cout << "Acquisition " << n << " nsamp: " << acqref->number_of_samples() << std::endl;
      std::cout << "Acquisition " << n << " flags: " << acqref->flags() << std::endl;
      std::cout << "Acquisition " << n << " data[4]: " << acqref->getData()[4].real() << std::endl;
      dataset2.appendAcquisition(* acqref);
      delete acqref;
  }
  std::cout << "Dataset2: number of acquisitions: " << dataset2.getNumberOfAcquisitions() << std::endl;
  
  //ISMRMRD::IsmrmrdHeader h;
  //serialize(h,std::cout);

  return 0;
}
