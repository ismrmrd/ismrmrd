#include "ismrmrd/meta.h"

#include <boost/test/unit_test.hpp>

const std::string XML("\
<?xml version=\"1.0\"?>\n\
<ismrmrdMeta>\n\
	<meta>\n\
		<name>param1</name>\n\
		<value>value_string</value>\n\
	</meta>\n\
	<meta>\n\
		<name>param2</name>\n\
		<value>677797</value>\n\
	</meta>\n\
	<meta>\n\
		<name>param3</name>\n\
		<value>1.456</value>\n\
		<value>66797</value>\n\
		<value>foobar</value>\n\
	</meta>\n\
</ismrmrdMeta>\n\
");

using namespace ISMRMRD;

BOOST_AUTO_TEST_SUITE(MetaTest)

BOOST_AUTO_TEST_CASE(test_serialize_meta)
{
    MetaContainer container;

    container.set<char*>("param1", const_cast<char*>("value_string"));
    container.set<long>("param2", 677797);

    container.append<double>("param3", 1.456);
    container.append<long>("param3", 66797);
    container.append<char*>("param3", const_cast<char*>("foobar"));

    std::stringstream sstr;
    ISMRMRD::serialize(container, sstr);

    BOOST_CHECK_EQUAL(XML, sstr.str());
}

BOOST_AUTO_TEST_CASE(test_deserialize_meta)
{
    MetaContainer container;

    ISMRMRD::deserialize(XML.c_str(), container);

    BOOST_CHECK_EQUAL(container.as_str("param1"), "value_string");
    BOOST_CHECK_EQUAL(container.as_long("param2"), 677797);
    BOOST_CHECK_EQUAL(container.as_double("param3", 0), 1.456);
    BOOST_CHECK_EQUAL(container.as_long("param3", 1), 66797);
    BOOST_CHECK_EQUAL(container.as_str("param3", 2), "foobar");
}

BOOST_AUTO_TEST_SUITE_END()
