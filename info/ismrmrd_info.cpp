#include <iostream>
#include "ismrmrd_version.h"

int main(int argc, char** argv)
{
  std::cout << "ISMRMRD VERSION INFO: " << std::endl;
  std::cout << "   -- version " << ISMRMRD_VERSION_MAJOR << "." << ISMRMRD_VERSION_MINOR << "." << ISMRMRD_VERSION_PATCH << std::endl;
  std::cout << "   -- SHA1    " << ismrmrd_git_sha1 << std::endl;
  return 0;
}
