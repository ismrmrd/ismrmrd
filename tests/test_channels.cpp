#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/version.h"
#include <boost/test/unit_test.hpp>

using namespace ISMRMRD;

BOOST_AUTO_TEST_SUITE(ChannelTest)

void fill_channels(uint64_t mask[ISMRMRD_CHANNEL_MASKS])
{
    for (int i = 0; i < ISMRMRD_CHANNEL_MASKS; i++) {
        mask[i] = 0xFFFFFFFFFFFFFFFF;
    }
}

BOOST_AUTO_TEST_CASE(test_is_channel_on)
{
    uint64_t channel_mask[ISMRMRD_CHANNEL_MASKS] = {0};

    // TODO: this returns and ISMRMRD_RUNTIMEERROR, which casts to "true"
    /* BOOST_CHECK_EQUAL(ismrmrd_is_channel_on(NULL, 0), false); */

    for (int chan = 0; chan < 64 * ISMRMRD_CHANNEL_MASKS; chan++) {
        BOOST_CHECK_EQUAL(ismrmrd_is_channel_on(channel_mask, chan), false);
    }

    fill_channels(channel_mask);
    for (int chan = 0; chan < 64 * ISMRMRD_CHANNEL_MASKS; chan++) {
        BOOST_CHECK_EQUAL(ismrmrd_is_channel_on(channel_mask, chan), true);
    }
}

BOOST_AUTO_TEST_CASE(test_set_channel_on)
{
    uint64_t channel_mask[ISMRMRD_CHANNEL_MASKS] = {0};

    BOOST_CHECK_EQUAL(ismrmrd_set_channel_on(NULL, 0), ISMRMRD_RUNTIMEERROR);

    for (int chan = 0; chan < 64 * ISMRMRD_CHANNEL_MASKS; chan++) {
        BOOST_CHECK_EQUAL(ismrmrd_set_channel_on(channel_mask, chan), ISMRMRD_NOERROR);
        uint64_t bitmask = static_cast<uint64_t>(1) << (chan % 64);
        size_t offset = chan / 64;
        BOOST_REQUIRE((channel_mask[offset] & bitmask) != 0);
    }
}

BOOST_AUTO_TEST_CASE(test_set_channel_off)
{
    uint64_t channel_mask[ISMRMRD_CHANNEL_MASKS] = {0};
    fill_channels(channel_mask);

    BOOST_CHECK_EQUAL(ismrmrd_set_channel_off(NULL, 0), ISMRMRD_RUNTIMEERROR);

    for (int chan = 0; chan < 64 * ISMRMRD_CHANNEL_MASKS; chan++) {
        BOOST_CHECK_EQUAL(ismrmrd_set_channel_off(channel_mask, chan), ISMRMRD_NOERROR);

        uint64_t bitmask = static_cast<uint64_t>(1) << (chan % 64);
        size_t offset = chan / 64;
        BOOST_REQUIRE((channel_mask[offset] & bitmask) == 0);
    }
}

BOOST_AUTO_TEST_CASE(test_set_all_channels_off)
{
    uint64_t channel_mask[ISMRMRD_CHANNEL_MASKS] = {0};
    fill_channels(channel_mask);

    BOOST_CHECK_EQUAL(ismrmrd_set_all_channels_off(NULL), ISMRMRD_RUNTIMEERROR);

    BOOST_CHECK_EQUAL(ismrmrd_set_all_channels_off(channel_mask), ISMRMRD_NOERROR);
    for (int idx = 0; idx < ISMRMRD_CHANNEL_MASKS; idx++) {
        BOOST_REQUIRE(channel_mask[idx] == 0);
    }
}

BOOST_AUTO_TEST_SUITE_END()
