#include "ismrmrd/ismrmrd.h"
#include <boost/test/unit_test.hpp>

using namespace ISMRMRD;

BOOST_AUTO_TEST_SUITE(QuaternionTest)

BOOST_AUTO_TEST_CASE(test_quaternions)
{
    float read_dir[3] = {1.0, 0, 0};
    float phase_dir[3] = {0, 1.0, 0};
    float slice_dir[3] = {0, 0, 1.0};
    float quaternion[4];

    /* convert the direction vectors to a quaternion and verify */
    ismrmrd_directions_to_quaternion(read_dir, phase_dir, slice_dir, quaternion);

    BOOST_REQUIRE(quaternion[0] == 0.0);
    BOOST_REQUIRE(quaternion[1] == 0.0);
    BOOST_REQUIRE(quaternion[2] == 0.0);
    BOOST_REQUIRE(quaternion[3] == 1.0);

    /* convert the quaternion back to direction cosines and verify */
    ismrmrd_quaternion_to_directions(quaternion, read_dir, phase_dir, slice_dir);
    BOOST_REQUIRE(read_dir[0] == 1.0);
    BOOST_REQUIRE(read_dir[1] == 0.0);
    BOOST_REQUIRE(read_dir[2] == 0.0);
    BOOST_REQUIRE(phase_dir[0] == 0.0);
    BOOST_REQUIRE(phase_dir[1] == 1.0);
    BOOST_REQUIRE(phase_dir[2] == 0.0);
    BOOST_REQUIRE(slice_dir[0] == 0.0);
    BOOST_REQUIRE(slice_dir[1] == 0.0);
    BOOST_REQUIRE(slice_dir[2] == 1.0);

    /* check that determinant is > 0 */
    BOOST_REQUIRE(ismrmrd_sign_of_directions(read_dir, phase_dir, slice_dir) > 0);

    /* flip sign of third column and check that determinant is < 0 */
    slice_dir[0] = -slice_dir[0];
    slice_dir[1] = -slice_dir[1];
    slice_dir[2] = -slice_dir[2];
    BOOST_REQUIRE(ismrmrd_sign_of_directions(read_dir, phase_dir, slice_dir) < 0);
}

BOOST_AUTO_TEST_SUITE_END()
