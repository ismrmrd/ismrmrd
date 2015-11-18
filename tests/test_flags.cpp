#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/version.h"
#include <boost/test/unit_test.hpp>

using namespace ISMRMRD;

BOOST_AUTO_TEST_SUITE(FlagTest)

BOOST_AUTO_TEST_CASE(test_is_flag_set)
{
    uint64_t flags = 0;

    for (int f = 1; f <= 64; f++) {
        BOOST_CHECK_EQUAL(ismrmrd_is_flag_set(flags, f), false);
    }

    for (int f = 1; f <= 64; f++) {
        flags |= ((uint64_t)1 << (f - 1));
        BOOST_CHECK_EQUAL(ismrmrd_is_flag_set(flags, f), true);
    }
}

BOOST_AUTO_TEST_CASE(test_set_flag)
{
    uint64_t flags = 0;

    BOOST_CHECK_EQUAL(ismrmrd_set_flag(NULL, ISMRMRD_ACQ_USER8), ISMRMRD_RUNTIMEERROR);

    for (int f = 1; f <= 64; f++) {
        BOOST_CHECK_EQUAL(ismrmrd_set_flag(&flags, f), ISMRMRD_NOERROR);
        BOOST_REQUIRE((flags & ((uint64_t)1 << (f - 1))) != 0);
    }
}

BOOST_AUTO_TEST_CASE(test_set_flags)
{
    uint64_t flags = 0;

    BOOST_CHECK_EQUAL(ismrmrd_set_flags(NULL, 0xFFFFFFFFFFFFFFFF), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_set_flags(&flags, 0xFFFFFFFFFFFFFFFF), ISMRMRD_NOERROR);

    BOOST_CHECK_EQUAL(flags, 0xFFFFFFFFFFFFFFFF);
}

BOOST_AUTO_TEST_CASE(test_clear_flag)
{
    uint64_t flags = 0xFFFFFFFFFFFFFFFF;

    BOOST_CHECK_EQUAL(ismrmrd_clear_flag(NULL, ISMRMRD_IMAGE_USER8), ISMRMRD_RUNTIMEERROR);
    for (int f = 1; f <= 64; f++) {
        BOOST_CHECK_EQUAL(ismrmrd_clear_flag(&flags, f), ISMRMRD_NOERROR);
        BOOST_REQUIRE((flags & ((uint64_t)1 << (f - 1))) == 0);
    }
}

BOOST_AUTO_TEST_CASE(test_clear_all_flags)
{
    uint64_t flags = 0xFFFFFFFFFFFFFFFF;

    BOOST_CHECK_EQUAL(ismrmrd_clear_all_flags(NULL), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_clear_all_flags(&flags), ISMRMRD_NOERROR);
    BOOST_CHECK_EQUAL(flags, 0);
}

BOOST_AUTO_TEST_SUITE_END()
