#include <string.h>
#include <stdlib.h>

/* Language and Cross platform section for defining types */
#ifdef __cplusplus
#include <cmath>
#include <cassert>

#ifdef DEBUG
#include <cstdio>
#endif /* DEBUG */

#else
/* C99 compiler */
#include <math.h>
#include <assert.h>

#ifdef DEBUG
#include <stdio.h>
#endif /* DEBUG */

#endif /* __cplusplus */

#include "ismrmrd/ismrmrd.h"

#ifdef __cplusplus
namespace ISMRMRD {
extern "C" {
#endif

void ismrmrd_init_acquisition_header(ISMRMRD_AcquisitionHeader *hdr) {
    memset(hdr, 0, sizeof(ISMRMRD_AcquisitionHeader));
    hdr->version = ISMRMRD_VERSION;
    hdr->number_of_samples = 0;
    hdr->available_channels = 1;
    hdr->active_channels = 1;
}

void ismrmrd_init_acquisition(ISMRMRD_Acquisition *acq) {
    ismrmrd_init_acquisition_header(&acq->head);
    acq->traj = (float *)malloc(0); /* can be passed to free */
    acq->data = (complex_float_t *)malloc(0);
}

void ismrmrd_cleanup_acquisition(ISMRMRD_Acquisition *acq) {
    free(acq->data);
    free(acq->traj);
}
    
ISMRMRD_Acquisition * ismrmrd_create_acquisition() {
    ISMRMRD_Acquisition *acq = (ISMRMRD_Acquisition *) malloc(sizeof(ISMRMRD_Acquisition));
    ismrmrd_init_acquisition(acq);
    return acq;
}

void ismrmrd_free_acquisition(ISMRMRD_Acquisition *acq) {
    free(acq->data);
    free(acq->traj);
    free(acq);
}

void ismrmrd_copy_acquisition(ISMRMRD_Acquisition *acqdest, const ISMRMRD_Acquisition *acqsource) {
    /* Copy the header */
    memcpy(&acqdest->head, &acqsource->head, sizeof(ISMRMRD_AcquisitionHeader));
    /* Reallocate memory for the trajectory and the data*/
    ismrmrd_make_consistent_acquisition(acqdest);
    /* Copy the trajectory and the data */
    memcpy(acqdest->traj, acqsource->traj, ismrmrd_size_of_acquisition_traj(acqsource));
    memcpy(acqdest->data, acqsource->data, ismrmrd_size_of_acquisition_data(acqsource));
}

int ismrmrd_make_consistent_acquisition(ISMRMRD_Acquisition *acq) {
	size_t traj_size, data_size;

    if (acq->head.available_channels < acq->head.active_channels) {
        acq->head.available_channels = acq->head.active_channels;
    }

	traj_size = ismrmrd_size_of_acquisition_traj(acq);
    if (traj_size > 0) {
        acq->traj = (float *)realloc(acq->traj, traj_size);
        if (acq->traj == NULL) {
            ISMRMRD_THROW(ISMRMRD_MEMORYERROR,
                    "Failed to realloc acquisition trajectory array");
            return ISMRMRD_MEMORYERROR;
        }
    }

    data_size = ismrmrd_size_of_acquisition_data(acq);
    if (data_size > 0) {
        acq->data = (complex_float_t *)realloc(acq->data, data_size);
        if (acq->data == NULL) {
            ISMRMRD_THROW(ISMRMRD_MEMORYERROR,
                    "Failed to realloc acquisition data array");
            return ISMRMRD_MEMORYERROR;
        }
    }

    return ISMRMRD_NOERROR;
}

size_t ismrmrd_size_of_acquisition_traj(const ISMRMRD_Acquisition *acq) {
    int num_traj = acq->head.number_of_samples * acq->head.trajectory_dimensions;
    return num_traj * sizeof(*acq->traj);
}

size_t ismrmrd_size_of_acquisition_data(const ISMRMRD_Acquisition *acq) {
    int num_data = acq->head.number_of_samples * acq->head.active_channels;
    return num_data * sizeof(*acq->data);
}

/* ImageHeader functions */
void ismrmrd_init_image_header(ISMRMRD_ImageHeader *hdr) {
    memset(hdr, 0, sizeof(ISMRMRD_ImageHeader));
    hdr->version = ISMRMRD_VERSION;
    hdr->matrix_size[0] = 0;
    hdr->matrix_size[1] = 1;
    hdr->matrix_size[2] = 1;
    hdr->channels = 1;
}

/* Image functions */
ISMRMRD_Image * ismrmrd_create_image() {
    ISMRMRD_Image *im = (ISMRMRD_Image *) malloc(sizeof(ISMRMRD_Image));
    ismrmrd_init_image(im);
    return im;
}

void ismrmrd_free_image(ISMRMRD_Image *im) {
    free(im->data);
    free(im->attribute_string);
    free(im);
}
    
void ismrmrd_init_image(ISMRMRD_Image *im) {
    ismrmrd_init_image_header(&im->head);
    im->attribute_string = (char *) malloc(0); /* can be passed to free */
    im->data = (void *) malloc(0);
}

void ismrmrd_cleanup_image(ISMRMRD_Image *im) {
    free(im->attribute_string);
    free(im->data);
}
    
void ismrmrd_copy_image(ISMRMRD_Image *imdest, const ISMRMRD_Image *imsource) {
    memcpy(&imdest->head, &imsource->head, sizeof(ISMRMRD_ImageHeader));
    ismrmrd_make_consistent_image(imdest);
    memcpy(&imdest->attribute_string, &imsource->attribute_string,
            ismrmrd_size_of_image_attribute_string(imdest));
    memcpy(&imdest->data, &imsource->data, ismrmrd_size_of_image_data(imdest));
}

int ismrmrd_make_consistent_image(ISMRMRD_Image *im) {
    size_t attr_size, data_size;
	
	attr_size = ismrmrd_size_of_image_attribute_string(im);
    if (attr_size > 0) {
        im->attribute_string = (char *)realloc(im->attribute_string, attr_size);
        if (im->attribute_string == NULL) {
            ISMRMRD_THROW(ISMRMRD_MEMORYERROR,
                    "Failed to realloc image attribute string");
            return ISMRMRD_MEMORYERROR;
        }
    }

    data_size = ismrmrd_size_of_image_data(im);
    if (data_size > 0) {
        im->data = realloc(im->data, data_size);
        if (im->data == NULL) {
            ISMRMRD_THROW(ISMRMRD_MEMORYERROR,
                    "Failed to realloc image data array");
            return ISMRMRD_MEMORYERROR;
        }
    }

    return ISMRMRD_NOERROR;
}

size_t ismrmrd_size_of_image_data(const ISMRMRD_Image *im) {
    size_t data_size = 0;
    int num_data = im->head.matrix_size[0] * im->head.matrix_size[1] *
            im->head.matrix_size[2] * im->head.channels;

    switch (im->head.data_type) {
    case ISMRMRD_USHORT:
        data_size = num_data * sizeof(uint16_t);
        break;
    case ISMRMRD_SHORT:
        data_size = num_data * sizeof(int16_t);
        break;
    case ISMRMRD_UINT:
        data_size = num_data * sizeof(uint32_t);
        break;
    case ISMRMRD_INT:
        data_size = num_data * sizeof(int32_t);
        break;
    case ISMRMRD_FLOAT:
        data_size = num_data * sizeof(float);
        break;
    case ISMRMRD_DOUBLE:
        data_size = num_data * sizeof(double);
        break;
    case ISMRMRD_CXFLOAT:
        data_size = num_data * sizeof(complex_float_t);
        break;
    case ISMRMRD_CXDOUBLE:
        data_size = num_data * sizeof(complex_double_t);
        break;
    default:
        ISMRMRD_THROW(ISMRMRD_TYPEERROR, "Invalid image data type");
        data_size = 0;
    }
    return data_size;
}

size_t ismrmrd_size_of_image_attribute_string(const ISMRMRD_Image *im) {
    return im->head.attribute_string_len * sizeof(*im->attribute_string);
}

/* NDArray functions */
ISMRMRD_NDArray * ismrmrd_create_ndarray() {
    ISMRMRD_NDArray *arr = (ISMRMRD_NDArray *) malloc(sizeof(ISMRMRD_NDArray));
    ismrmrd_init_ndarray(arr);
    return arr;
}

void ismrmrd_free_ndarray(ISMRMRD_NDArray *arr) {
    free(arr->data);
    free(arr);
}

void ismrmrd_init_ndarray(ISMRMRD_NDArray *arr) {
	int n;

	arr->version = ISMRMRD_VERSION;
    arr->data_type = 0; // no default data type
    arr->ndim = 0;

    for (n = 0; n < ISMRMRD_NDARRAY_MAXDIM; n++) {
        arr->dims[n] = 0;
    }
    arr->data = malloc(0);  /* Can be freed */
}

void ismrmrd_cleanup_ndarray(ISMRMRD_NDArray *arr) {
    free(arr->data);
}

void ismrmrd_copy_ndarray(ISMRMRD_NDArray *arrdest, const ISMRMRD_NDArray *arrsource) {
    int n;

    arrdest->version = arrsource->version;
    arrdest->data_type = arrsource->data_type;
    arrdest->ndim = arrsource->ndim;

	for (n = 0; n < ISMRMRD_NDARRAY_MAXDIM; n++) {
        arrdest->dims[n] = arrsource->dims[n];
    }
    ismrmrd_make_consistent_ndarray(arrdest);
    memcpy(&arrdest->data, &arrsource->data, ismrmrd_size_of_ndarray_data(arrdest));
}

int ismrmrd_make_consistent_ndarray(ISMRMRD_NDArray *arr) {
    size_t data_size = ismrmrd_size_of_ndarray_data(arr);
    if (data_size > 0) {
        arr->data = realloc(arr->data, data_size);
        if (arr->data == NULL) {
            ISMRMRD_THROW(ISMRMRD_MEMORYERROR,
                    "Failed to realloc NDArray data array");
            return ISMRMRD_MEMORYERROR;
        }
    }
    else {
        /* data_size == 0 */
        /* the data type is invalid for some other reason */
        return ISMRMRD_MEMORYERROR;
    }
    return ISMRMRD_NOERROR;
}

size_t ismrmrd_size_of_ndarray_data(const ISMRMRD_NDArray *arr) {
    size_t data_size = 0;
    int num_data = 1;
	int n;
    for (n = 0; n < arr->ndim; n++) {
        num_data *= arr->dims[n];
    }

    switch (arr->data_type) {
    case ISMRMRD_USHORT:
        data_size = num_data * sizeof(uint16_t);
        break;
    case ISMRMRD_SHORT:
        data_size = num_data * sizeof(int16_t);
        break;
    case ISMRMRD_UINT:
        data_size = num_data * sizeof(uint32_t);
        break;
    case ISMRMRD_INT:
        data_size = num_data * sizeof(int32_t);
        break;
    case ISMRMRD_FLOAT:
        data_size = num_data * sizeof(float);
        break;
    case ISMRMRD_DOUBLE:
        data_size = num_data * sizeof(double);
        break;
    case ISMRMRD_CXFLOAT:
        data_size = num_data * sizeof(complex_float_t);
        break;
    case ISMRMRD_CXDOUBLE:
        data_size = num_data * sizeof(complex_double_t);
        break;
    default:
        ISMRMRD_THROW(ISMRMRD_TYPEERROR, "Invalid NDArray data type");
        data_size = 0;
    }

    return data_size;
}

bool ismrmrd_is_flag_set(const uint64_t flags, const uint64_t val) {
    uint64_t bitmask = 1 << (val - 1);
    return (flags & bitmask) > 0;
}

void ismrmrd_set_flag(uint64_t *flags, const uint64_t val) {
    uint64_t bitmask = 1 << (val - 1);
    *flags |= bitmask;
}

void ismrmrd_clear_flag(uint64_t *flags, const uint64_t val) {
    uint64_t bitmask = 1 << (val - 1);
    *flags &= ~bitmask;
}

void ismrmrd_clear_all_flags(uint64_t *flags) {
    *flags = 0;
}

static void ismrmrd_error_default(const char *file, int line, const char *func, int err, char *msg)
{
#ifdef DEBUG
    char *msgtype = ismrmrd_strerror(err);
    fprintf(stderr, "ERROR: %s in %s, line %d: %s\n", msgtype, file, line, msg);
#endif
}

ismrmrd_error_handler_t ismrmrd_error_handler = ismrmrd_error_default;

void ismrmrd_set_error_handler(ismrmrd_error_handler_t handler) {
    ismrmrd_error_handler = handler;
}

char *ismrmrd_strerror(int err) {
    /* Match the ISMRMRD_ErrorCodes */
    char *error_messages[] = {
        "No Error",
        "Memory Error",
        "File Error"
    };
    
    assert(err > ISMRMRD_BEGINERROR);
    assert(err < ISMRMRD_ENDERROR);

    return error_messages[err];
}

int ismrmrd_sign_of_directions(float read_dir[3], float phase_dir[3], float slice_dir[3]) {
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

void ismrmrd_directions_to_quaternion(float read_dir[3], float phase_dir[3],
                                      float slice_dir[3], float quat[4]) {
    float r11 = read_dir[0], r12 = phase_dir[0], r13 = slice_dir[0];
    float r21 = read_dir[1], r22 = phase_dir[1], r23 = slice_dir[1];
    float r31 = read_dir[2], r32 = phase_dir[2], r33 = slice_dir[2];

    double a = 1, b = 0, c = 0, d = 0, s = 0;
    double trace = 0;
    double xd, yd, zd;

    /* verify the sign of the rotation*/
    if (ismrmrd_sign_of_directions(read_dir, phase_dir, slice_dir) < 0) {
        /* flip 3rd column */
        r13 = -r13;
        r23 = -r23;
        r33 = -r33;
    }

    /* Compute quaternion parameters */
    /* http://www.cs.princeton.edu/~gewang/projects/darth/stuff/quat_faq.html#Q55 */
    trace = 1.0l + r11 + r22 + r33;
    if (trace > 0.00001l) { /* simplest case */
        s = sqrt(trace) * 2;
        a = (r32 - r23) / s;
        b = (r13 - r31) / s;
        c = (r21 - r12) / s;
        d = 0.25l * s;
    } else {
        /* trickier case...
		 * determine which major diagonal element has
		 * the greatest value... */
        xd = 1.0 + r11 - (r22 + r33); /* 4**b**b */
        yd = 1.0 + r22 - (r11 + r33); /* 4**c**c */
        zd = 1.0 + r33 - (r11 + r22); /* 4**d**d */
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

    quat[0] = (float)a;
    quat[1] = (float)b;
    quat[2] = (float)c;
    quat[3] = (float)d;
}

/* http://www.cs.princeton.edu/~gewang/projects/darth/stuff/quat_faq.html#Q54 */
void ismrmrd_quaternion_to_directions(float quat[4], float read_dir[3],
                                      float phase_dir[3], float slice_dir[3]) {
    float a = quat[0], b = quat[1], c = quat[2], d = quat[3];

    read_dir[0] = 1.0f - 2.0f * (b * b + c * c);
    phase_dir[0] = 2.0f * (a * b - c * d);
    slice_dir[0] = 2.0f * (a * c + b * d);

    read_dir[1] = 2.0f * (a * b + c * d);
    phase_dir[1] = 1.0f - 2.0f * (a * a + c * c);
    slice_dir[1] = 2.0f * (b * c - a * d);

    read_dir[2] = 2.0f * (a * c - b * d);
    phase_dir[2] = 2.0f * (b * c + a * d);
    slice_dir[2] = 1.0f - 2.0f * (a * a + b * b);
}

#ifdef __cplusplus
} // extern "C"
} // namespace ISMRMRD
#endif
