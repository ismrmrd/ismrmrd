#include <iostream>
#include <sstream>
#include "ismrmrd_meta.h"


int main(int argc, char** argv)
{
  std::cout << "Test routine for ISMRMRD meta data structures" << std::endl << std::endl;

  ISMRMRD::MetaContainer h;

  h.set("my_para","this is one value");
  h.append("my_para",77887L);
  h.append("my_para",1.66754);
  h.append("my_para","Blah Blah");  
  h.set("my_para_2",4L);
  h.set("my_para_3",4.778);

  std::stringstream str;
  ISMRMRD::serialize(h,str);

  std::cout << "Serialized version of header: " << std::endl;
  std::cout << str.str() << std::endl;

  ISMRMRD::MetaContainer h2;
  std::string xml = str.str();
  deserialize(xml.c_str(),h2);

  std::cout << "Header after deserialization and serialization" << std::endl;
  serialize(h2, std::cout);

  return 0;
}
