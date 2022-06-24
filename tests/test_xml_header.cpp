#include "ismrmrd/xml.h"
#include "ismrmrd/version.h"
#include "embedded_xml.h"
#include <boost/test/unit_test.hpp>
#include <sstream>

using namespace ISMRMRD;

BOOST_AUTO_TEST_SUITE(HeaderTest)

BOOST_AUTO_TEST_CASE(test_basic_xml_header)
{

    IsmrmrdHeader header;
    deserialize(basic_xml.c_str(),header);
    std::stringstream stream;
    serialize(header,stream);

    IsmrmrdHeader header2;
    deserialize(stream.str().c_str(),header2);


    BOOST_CHECK_EQUAL(header,header2);
}

BOOST_AUTO_TEST_CASE(test_extended_xml_header)
{

    IsmrmrdHeader header;
    deserialize(extended_xml.c_str(),header);
    std::stringstream stream;
    serialize(header,stream);

    IsmrmrdHeader header2;
    deserialize(stream.str().c_str(),header2);

    BOOST_CHECK_EQUAL(header,header2);



}
BOOST_AUTO_TEST_SUITE_END()