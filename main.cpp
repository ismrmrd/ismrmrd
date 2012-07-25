#include <iostream>

#include "ismrmrd.hxx"

using namespace ISMRMRD;

int main (int argc, char* argv[])
{
  try
  {
    std::cout << "Trying to parse...\"" << argv[1] << "\"" << std::endl;
    std::auto_ptr<ismrmrdHeader> s (ismrmrdHeader_ (argv[1]));
    std::cout << "Done with the parsing." << std::endl;

    std::cout << "Number of encoding spaces: " << s->encoding().size() << std::endl;
    for (ismrmrdHeader::encoding_sequence::iterator i (s->encoding().begin ()); i != s->encoding().end(); ++i) {
      std::cout << "FOV: " << i->encodedSpace().fieldOfView_m().x() 
		<< ", " << i->encodedSpace().fieldOfView_m().y() 
		<< ", " << i->encodedSpace().fieldOfView_m().z() << std::endl;
    }
  }
  catch (const xml_schema::exception& e) {
    std::cerr << e << std::endl;
    return 1;
  }
}
