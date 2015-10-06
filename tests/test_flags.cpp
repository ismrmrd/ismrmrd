#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/version.h"
#include <boost/test/unit_test.hpp>

using namespace ISMRMRD;

BOOST_AUTO_TEST_SUITE(Flags)

BOOST_AUTO_TEST_CASE(no_flags)
{
    Acquisition acq;
    Image<float> img;

    for (int f = 1; f <= 64; f++) {
        BOOST_CHECK_EQUAL(acq.isFlagSet(f), false);
        BOOST_CHECK_EQUAL(img.isFlagSet(f), false);
    }
}

BOOST_AUTO_TEST_CASE(set_flag)
{
    Acquisition acq;
    Image<float> img;

    for (int f = 1; f <= 64; f++) {
        acq.setFlag(f);
        BOOST_CHECK_EQUAL(acq.isFlagSet(f), true);

        img.setFlag(f);
        BOOST_CHECK_EQUAL(img.isFlagSet(f), true);
    }
}

BOOST_AUTO_TEST_CASE(set_flags)
{
    Acquisition acq;
    Image<float> img;

    uint64_t flags = 0xFFFFFFFFFFFFFFFF;

    acq.setFlags(flags);
    BOOST_CHECK_EQUAL(acq.getFlags(), flags);

    img.setFlags(flags);
    BOOST_CHECK_EQUAL(img.getFlags(), flags);
}

BOOST_AUTO_TEST_CASE(clear_flag)
{
    Acquisition acq;
    Image<float> img;

    uint64_t flags = 0xFFFFFFFFFFFFFFFF;

    acq.setFlags(flags);
    img.setFlags(flags);

    for (int f = 1; f <= 64; f++) {
        acq.clearFlag(f);
        BOOST_CHECK_EQUAL(acq.getFlags() & (1UL << (f - 1)), 0);

        img.clearFlag(f);
        BOOST_CHECK_EQUAL(img.getFlags() & (1UL << (f - 1)), 0);
    }
}

BOOST_AUTO_TEST_CASE(clear_all_flags)
{
    Acquisition acq;
    Image<float> img;

    uint64_t flags = 0xFFFFFFFFFFFFFFFF;

    acq.clearAllFlags();
    BOOST_CHECK_EQUAL(acq.getFlags(), 0);

    img.clearAllFlags();
    BOOST_CHECK_EQUAL(img.getFlags(), 0);

    acq.setFlags(flags);
    img.setFlags(flags);

    acq.clearAllFlags();
    BOOST_CHECK_EQUAL(acq.getFlags(), 0);

    img.clearAllFlags();
    BOOST_CHECK_EQUAL(img.getFlags(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
