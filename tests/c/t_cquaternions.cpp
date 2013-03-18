#include <stdlib.h>
#include <stdio.h>

#include "ismrmrd.h"


int main(void)
{
    float read_dir[3] = {1.0, 0, 0};
    float phase_dir[3] = {0, 1.0, 0};
    float slice_dir[3] = {0, 0, 1.0};
    float quaternion[4];

    /* convert the direction vectors to a quaternion and verify */
    ISMRMRD::directions_to_quaternion(read_dir, phase_dir, slice_dir, quaternion);

    if (!((quaternion[0] == 0.0) && (quaternion[1] == 0.0) && (quaternion[2] == 0.0) && (quaternion[3] == 1.0))) {
        return EXIT_FAILURE;
    }

    /* convert the quaternion back to direction cosines and verify */
    ISMRMRD::quaternion_to_directions(quaternion, read_dir, phase_dir, slice_dir);
    int rd = ((read_dir[0] == 1.0) && (read_dir[1] == 0.0) && (read_dir[2] == 0.0));
    int pd = ((phase_dir[0] == 0.0) && (phase_dir[1] == 1.0) && (phase_dir[2] == 0.0));
    int sd = ((slice_dir[0] == 0.0) && (slice_dir[1] == 0.0) && (slice_dir[2] == 1.0));

    if (!(rd && pd && sd)) {
        return EXIT_FAILURE;
    }

    /* check that determinant is > 0 */
    if (ISMRMRD::sign_of_directions(read_dir, phase_dir, slice_dir) < 0) {
        return EXIT_FAILURE;
    }

    /* flip sign of third column and check that determinant is < 0 */
    slice_dir[0] = -slice_dir[0];
    slice_dir[1] = -slice_dir[1];
    slice_dir[2] = -slice_dir[2];
    if (ISMRMRD::sign_of_directions(read_dir, phase_dir, slice_dir) > 0) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
