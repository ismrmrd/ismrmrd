#include <iostream>
#include "ismrmrd/version.h"

int main(int argc, char** argv)
{
  std::cout << "ISMRMRD VERSION INFO: " << std::endl;
  std::cout << "   -- Version:         " << ISMRMRD_VERSION_MAJOR << "." <<
        ISMRMRD_VERSION_MINOR << "." << ISMRMRD_VERSION_PATCH << std::endl;
  std::cout << "   -- SHA1:            " << ISMRMRD_GIT_SHA1_HASH << std::endl;
  std::cout << "   -- Dataset support: " << (ISMRMRD_DATASET_SUPPORT ? "yes" : "no") << std::endl;
  return 0;
}
