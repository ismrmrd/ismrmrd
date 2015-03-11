#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/version.h"
#include <boost/test/unit_test.hpp>

using namespace ISMRMRD;

BOOST_AUTO_TEST_SUITE(AcquisitionTest)

BOOST_AUTO_TEST_CASE(test_acquisition_header)
{
    ISMRMRD_AcquisitionHeader chead;

    BOOST_CHECK_EQUAL(ismrmrd_init_acquisition_header(NULL), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_init_acquisition_header(&chead), ISMRMRD_NOERROR);
    BOOST_CHECK_EQUAL(chead.version, ISMRMRD_VERSION_MAJOR);
    BOOST_CHECK_EQUAL(chead.number_of_samples, 0);
    BOOST_CHECK_EQUAL(chead.available_channels, 1);
    BOOST_CHECK_EQUAL(chead.active_channels, 1);

}

BOOST_AUTO_TEST_SUITE_END()
