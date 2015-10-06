#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/version.h"
#include <boost/test/unit_test.hpp>

using namespace ISMRMRD;

BOOST_AUTO_TEST_SUITE(Channels)

BOOST_AUTO_TEST_CASE(set_channel_active)
{
    Acquisition acq;

    for (int chan = 0; chan < 64 * ISMRMRD_CHANNEL_MASKS; chan++) {
        BOOST_CHECK_EQUAL(acq.isChannelActive(chan), false);
    }

    for (int i = 0; i < ISMRMRD_CHANNEL_MASKS * 64; i++) {
        acq.setChannelActive(i);
    }

    for (int chan = 0; chan < 64 * ISMRMRD_CHANNEL_MASKS; chan++) {
        BOOST_CHECK_EQUAL(acq.isChannelActive(chan), true);
    }
}

BOOST_AUTO_TEST_CASE(set_channel_not_active)
{
    Acquisition acq;

    for (int i = 0; i < ISMRMRD_CHANNEL_MASKS * 64; i++) {
        acq.setChannelActive(i);
    }

    for (int chan = 0; chan < 64 * ISMRMRD_CHANNEL_MASKS; chan++) {
        acq.setChannelNotActive(chan);
        BOOST_CHECK_EQUAL(acq.isChannelActive(chan), false);
    }
}

BOOST_AUTO_TEST_CASE(set_all_channels_not_active)
{
    Acquisition acq;

    for (int i = 0; i < ISMRMRD_CHANNEL_MASKS * 64; i++) {
        acq.setChannelActive(i);
    }

    acq.setAllChannelsNotActive();

    for (int chan = 0; chan < 64 * ISMRMRD_CHANNEL_MASKS; chan++) {
        BOOST_CHECK_EQUAL(acq.isChannelActive(chan), false);
    }
}

BOOST_AUTO_TEST_SUITE_END()
