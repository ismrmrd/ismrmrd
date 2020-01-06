#define BOOST_TEST_MODULE "ISMRMRD Unit Tests"
#include <boost/test/unit_test.hpp>

#include "ismrmrd/ismrmrd.h"
using namespace ISMRMRD;

void silent_error_handler(const char *file, int line,
        const char *function, int code, const char *msg)
{
}

struct GlobalConfig {
    // global setup
    GlobalConfig()
    {
        // enable more verbose testing output
        boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_test_units);
        // silence ISMRMRD errors on stdout
        ismrmrd_set_error_handler(silent_error_handler);
    }
    // global teardown
    ~GlobalConfig() { }
};

BOOST_GLOBAL_FIXTURE(GlobalConfig);
