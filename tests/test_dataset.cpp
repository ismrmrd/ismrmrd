#include "ismrmrd/dataset.h"
#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/version.h"
#include <boost/filesystem.hpp>
#include <boost/random.hpp>
#include <boost/test/unit_test.hpp>

using namespace ISMRMRD;

static boost::random::mt19937 rng;
static boost::random::uniform_real_distribution<float> dist = boost::random::uniform_real_distribution<float>();

float create_random_float() {
    return dist(rng);
}


BOOST_AUTO_TEST_SUITE(DatasetTest)

BOOST_AUTO_TEST_CASE(test_read_write) {

    boost::filesystem::path temp = boost::filesystem::unique_path();

    Acquisition acq = Acquisition(32, 4, 2);

    std::vector<Acquisition> acqs(10, acq);
    for (size_t i = 0; i < acqs.size(); i++) {
        std::generate((float *)acqs[i].data_begin(), (float *)acqs[i].data_end(), create_random_float);
        std::generate((float *)acqs[i].traj_begin(), (float *)acqs[i].traj_end(), create_random_float);
    }

    {
        Dataset dataset = Dataset(temp.string().c_str(), "/test", true);
        for (size_t i = 0; i < acqs.size(); i++)
            dataset.appendAcquisition(acqs[i]);
    }

    {
        Dataset dataset = Dataset(temp.string().c_str(), "/test", false);
        for (size_t i = 0; i < acqs.size(); i++) {
            Acquisition acq;
            dataset.readAcquisition(uint32_t(i), acq);

            const Acquisition &acq_ref = acqs[i];

            BOOST_REQUIRE(acq.getHead() == acq_ref.getHead());

            BOOST_CHECK(std::equal(acq.data_begin(), acq.data_end(), acq_ref.data_begin()));
            BOOST_CHECK(std::equal(acq.traj_begin(), acq.traj_end(), acq_ref.traj_begin()));
        }
    }

    boost::filesystem::remove(temp);
}

BOOST_AUTO_TEST_SUITE_END()
