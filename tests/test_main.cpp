#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "ISMRMRD Unit Tests"
#include <boost/test/unit_test.hpp>

#include "ismrmrd/ismrmrd.h"
using namespace ISMRMRD;

struct GlobalConfig {
    // global setup
    GlobalConfig()
    {
        // enable more verbose testing output
        boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_test_units);
    }
    // global teardown
    ~GlobalConfig() { }
};

BOOST_GLOBAL_FIXTURE(GlobalConfig);
