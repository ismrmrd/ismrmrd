#include "ismrmrd.h"
#include <math.h>

#ifdef __cplusplus
namespace ISMRMRD {
#endif

/**
 * Calculates the determinant of the matrix and return the sign
 */
int sign_of_directions(float read_dir[3], float phase_dir[3], float slice_dir[3])
{
    float r11 = read_dir[0], r12 = phase_dir[0], r13 = slice_dir[0];
    float r21 = read_dir[1], r22 = phase_dir[1], r23 = slice_dir[1];
    float r31 = read_dir[2], r32 = phase_dir[2], r33 = slice_dir[2];

    /* Determinant should be 1 or -1 */
    float deti = (r11 * r22 * r33) + (r12 * r23 * r31) + (r21 * r32 * r13) -
                 (r13 * r22 * r31) - (r12 * r21 * r33) - (r11 * r23 * r32);

    if (deti < 0) {
        return -1;
    } else {
        return 1;
    }
}

/**
 * Creates a normalized quaternion from a 3x3 rotation matrix
 */
void directions_to_quaternion(float read_dir[3], float phase_dir[3],
        float slice_dir[3], float quat[4])
{
    float r11 = read_dir[0], r12 = phase_dir[0], r13 = slice_dir[0];
    float r21 = read_dir[1], r22 = phase_dir[1], r23 = slice_dir[1];
    float r31 = read_dir[2], r32 = phase_dir[2], r33 = slice_dir[2];

    float a = 1, b = 0, c = 0, d = 0, s = 0;
    float trace = 0;
    float xd, yd, zd;

    /* verify the sign of the rotation*/
    if (sign_of_directions(read_dir, phase_dir, slice_dir) < 0) {
        /* flip 3rd column */
        r13 = -r13;
        r23 = -r23;
        r33 = -r33;
    }

    /* Compute quaternion parameters */
    /* http://www.cs.princeton.edu/~gewang/projects/darth/stuff/quat_faq.html#Q55 */
    trace = 1.0l + r11 + r22 + r33;
    if (trace > 0.00001l) {                /* simplest case */
        s = sqrt(trace) * 2;
        a = (r32 - r23) / s;
        b = (r13 - r31) / s;
        c = (r21 - r12) / s;
        d = 0.25l * s;
    } else {
        /* trickier case...
         * determine which major diagonal element has
         * the greatest value... */
        xd = 1.0 + r11 - (r22 + r33);  /* 4**b**b */
        yd = 1.0 + r22 - (r11 + r33);  /* 4**c**c */
        zd = 1.0 + r33 - (r11 + r22);  /* 4**d**d */
        /* if r11 is the greatest */
        if (xd > 1.0) {
            s = 2.0 * sqrt(xd);
            a = 0.25l * s;
            b = (r21 + r12) / s;
            c = (r31 + r13) / s;
            d = (r32 - r23) / s;
        }
        /* else if r22 is the greatest */
        else if (yd > 1.0) {
            s = 2.0 * sqrt(yd);
            a = (r21 + r12) / s;
            b = 0.25l * s;
            c = (r32 + r23) / s;
            d = (r13 - r31) / s;
        }
        /* else, r33 must be the greatest */
        else {
            s = 2.0 * sqrt(zd);
            a = (r13 + r31) / s;
            b = (r23 + r32) / s;
            c = 0.25l * s;
            d = (r21 - r12) / s;
        }

        if (a < 0.0l) {
            b = -b;
            c = -c;
            d = -d;
            a = -a;
        }
    }

    quat[0] = a; quat[1] = b; quat[2] = c; quat[3] = d;
}

/**
 * Converts a quaternion of the form | a b c d | to a
 * 3x3 rotation matrix
 *
 * http://www.cs.princeton.edu/~gewang/projects/darth/stuff/quat_faq.html#Q54
 */
void quaternion_to_directions(float quat[4], float read_dir[3],
        float phase_dir[3], float slice_dir[3])
{
    float a = quat[0], b = quat[1], c = quat[2], d = quat[3];

    read_dir[0]  = 1.0 - 2.0 * ( b*b + c*c );
    phase_dir[0] = 2.0 * ( a*b - c*d );
    slice_dir[0] = 2.0 * ( a*c + b*d );

    read_dir[1]  = 2.0 * ( a*b + c*d );
    phase_dir[1] = 1.0 - 2.0 * ( a*a + c*c );
    slice_dir[1] = 2.0 * ( b*c - a*d );

    read_dir[2]  = 2.0 * ( a*c - b*d );
    phase_dir[2] = 2.0 * ( b*c + a*d );
    slice_dir[2] = 1.0 - 2.0 * ( a*a + b*b );
}

#ifdef __cplusplus
} /* namespace ISMRMRD */
#endif
