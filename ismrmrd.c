#include <ismrmrd.h>

#ifdef __cplusplus
namespace ISMRMRD
{
extern "C" {
#endif

    /**********************/
    /* Acquisition Header */
    /**********************/
    void InitAcquisitionHeader(AcquisitionHeader* hdr)
    {
	memset(hdr, 0, sizeof(AcquisitionHeader));
	hdr->version = ISMRMRD_VERSION;
	hdr->number_of_samples = 0;
	hdr->available_channels = 1;
	hdr->active_channels = 1;
    }

    /***************/
    /* Acquisition */
    /***************/
    void InitAcquisition(Acquisition* acq) {
	InitAcquisitionHeader(&(acq->head));
	acq->traj = NULL;
	acq->data = NULL;
    }

    void CopyAcquisition(Acquisition* acqdest, const Acquisition* acqsource) {
	memcpy(&(acqdest->head), &(acqsource->head), sizeof(AcquisitionHeader));
	MakeConsistentAcquisition(acqdest);
	memcpy(&(acqdest->traj), &(acqsource->traj), acqdest->head.number_of_samples *  acqdest->head.trajectory_dimensions * sizeof(acqdest->traj));
	memcpy(&(acqdest->data), &(acqsource->data), acqdest->head.number_of_samples *  acqdest->head.active_channels * sizeof(acqdest->data));
    }

    void FreeAcquisition(Acquisition* acq) {
	free(acq->data);
	free(acq->traj);
	free(acq);
    }

    void MakeConsistentAcquisition(Acquisition* acq) {
	if (acq->head.available_channels < acq->head.active_channels) {
	    acq->head.available_channels = acq->head.active_channels;
	}

	int num_traj = acq->head.number_of_samples *  acq->head.trajectory_dimensions;
	if (num_traj > 0) {
	    acq->traj = (float *) realloc(acq->traj, num_traj * sizeof(acq->traj));
	}

	int num_data = acq->head.number_of_samples *  acq->head.active_channels;
	if (num_data > 0) {
	    acq->data = (complex_float_t *)realloc(acq->data, num_data * sizeof(acq->data));
	}
    }

    /****************/
    /* Image Header */
    /****************/
    void InitImageHeader(ImageHeader* hdr) {
	memset(hdr, 0, sizeof(ImageHeader));
	hdr->version = ISMRMRD_VERSION;
	hdr->matrix_size[0] = 0;
	hdr->matrix_size[1] = 1;
	hdr->matrix_size[2] = 1;
	hdr->channels  = 1;
    }


    /*********/
    /* Image */
    /*********/
    void InitImage(Image* im) {
	InitImageHeader(&(im->head));
	im->attribute_string = NULL;
	im->data = NULL;
    }

    void FreeImage(Image* im) {
	free(im->data);
	free(im->attribute_string);
	free(im);
    }

    int SizeOfImageData(const Image* im) {
	int data_size = 0;
	int num_data =  im->head.matrix_size[0] * im->head.matrix_size[1] * im->head.matrix_size[2] * im->head.channels;
	if (num_data > 0) {
	    switch (im->head.data_type) {
	    case DATA_UNSIGNED_SHORT:
		data_size =num_data * sizeof(uint16_t);
		break;
	    case DATA_SHORT:
		data_size = num_data * sizeof(int16_t);
		break;
	    case DATA_UNSIGNED_INT:
		data_size = num_data * sizeof(uint32_t);
		break;
	    case DATA_INT:
		data_size = num_data * sizeof(int32_t);
		break;
	    case DATA_FLOAT:
		data_size = num_data * sizeof(float);
		break;
	    case DATA_DOUBLE:
		data_size = num_data * sizeof(double);
		break;
	    case DATA_COMPLEX_FLOAT:
		data_size = num_data * sizeof(complex_float_t);
		break;
	    case DATA_COMPLEX_DOUBLE:
		data_size = num_data * sizeof(complex_double_t);
		break;
	    }
	}
	return data_size;
    }

    int SizeOfImageAttributeString(const Image* im) {
	int attribute_string_size = 0;
	if (im->head.attribute_string_len > 0) {
	    attribute_string_size = im->head.attribute_string_len * sizeof(im->attribute_string);
	}
	return attribute_string_size;
    }

    void MakeConsistentImage(Image* im) {
	int attr_size =  SizeOfImageAttributeString(im);
	if (attr_size > 0) {
	    im->attribute_string = (char *) realloc(im->attribute_string,  attr_size);
	}
	int data_size =  SizeOfImageData(im);
	if (data_size > 0) {
	    im->data = realloc(im->data, data_size);
	}
    }

    void CopyImage(Image* imdest, const Image* imsource) {
	memcpy(&(imdest->head), &(imsource->head), sizeof(ImageHeader));
	MakeConsistentImage(imdest);
	int attr_size =  SizeOfImageAttributeString(imdest);
	if (attr_size > 0) {	
	    memcpy(&(imdest->attribute_string), &(imsource->attribute_string), attr_size);
	}
	int data_size =  SizeOfImageData(imdest);
	if (data_size > 0) {
	    memcpy(&(imdest->data), &(imsource->data), data_size);
	}
    }

    /***********/
    /* NDArray */
    /***********/
    void InitNDArray(NDArray* arr) {
	arr->version = ISMRMRD_VERSION;
	arr->data_type = 0;  // no default data type
	arr->ndim = 0;
	for (int n=0; n<ISMRMRD_NDARRAY_MAXDIM; n++) {
	    arr->dims[n] = 1;
	}
	arr->data = NULL;
    }

    void FreeNDArray(NDArray* arr) {
	free(arr->data);
	free(arr);
    }

    int SizeOfNDArrayData(const NDArray* arr) {
	int data_size = 0;
	int num_data =  1;
	for (int n=0; n<ISMRMRD_NDARRAY_MAXDIM; n++) {
	    num_data *= arr->dims[n];
	}
	if (num_data > 0) {
	    switch (arr->data_type) {
	    case DATA_UNSIGNED_SHORT:
		data_size = num_data * sizeof(uint16_t);
		break;
	    case DATA_SHORT:
		data_size = num_data * sizeof(int16_t);
		break;
	    case DATA_UNSIGNED_INT:
		data_size = num_data * sizeof(uint32_t);
		break;
	    case DATA_INT:
		data_size = num_data * sizeof(int32_t);
		break;
	    case DATA_FLOAT:
		data_size = num_data * sizeof(float);
		break;
	    case DATA_DOUBLE:
		data_size = num_data * sizeof(double);
		break;
	    case DATA_COMPLEX_FLOAT:
		data_size= num_data * sizeof(complex_float_t);
		break;
	    case DATA_COMPLEX_DOUBLE:
		data_size = num_data * sizeof(complex_double_t);
		break;
	    }
	}
	return data_size;
    }

    void MakeConsistentNDArray(NDArray* arr) {
	int data_size =  SizeOfNDArrayData(arr);
	if (data_size > 0) {
	    arr->data = realloc(arr->data, data_size);
	}
    }

    void CopyNDArray(NDArray* arrdest, const NDArray* arrsource) {
	arrdest->version = arrsource->version;
	arrdest->data_type = arrsource->data_type;
	arrdest->ndim = arrsource->ndim;
	for (int n=0; n<ISMRMRD_NDARRAY_MAXDIM; n++) {
	    arrdest->dims[n] = arrsource->dims[n];
	}
	MakeConsistentNDArray(arrdest);
	int data_size =  SizeOfNDArrayData(arrdest);
	if (data_size > 0) {
	    memcpy(&(arrdest->data), &(arrsource->data), SizeOfNDArrayData(arrdest));
	}
    }


    /******************************/
    /* Flag convenience functions */
    /******************************/
    bool flags_is_set(const uint64_t flags, const uint64_t val) {
	uint64_t bitmask = 1;
	bitmask = (bitmask << (val-1));
	return ((flags & bitmask) > 0);
    }
  
    void flags_set(uint64_t* flags, const uint64_t val) {
	uint64_t bitmask = 1;
	bitmask = (bitmask << (val-1));
	*flags |= bitmask;
    }

    void flags_clear(uint64_t* flags, const uint64_t val) {
	uint64_t bitmask = 1;
	bitmask = (bitmask << (val-1));
	if (flags_is_set(*flags, val)) {
	    *flags &= ~bitmask;
	};
    }

    void flags_clear_all(uint64_t* flags) {
	*flags = 0;
    }

    /******************************/
    /* Quaternions and Rotations  */
    /******************************/

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

	double a = 1, b = 0, c = 0, d = 0, s = 0;
	double trace = 0;
	double xd, yd, zd;

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

	quat[0] = (float)a; quat[1] = (float)b; quat[2] = (float)c; quat[3] = (float)d;
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

	read_dir[0]  = 1.0f - 2.0f * ( b*b + c*c );
	phase_dir[0] = 2.0f * ( a*b - c*d );
	slice_dir[0] = 2.0f * ( a*c + b*d );

	read_dir[1]  = 2.0f * ( a*b + c*d );
	phase_dir[1] = 1.0f - 2.0f * ( a*a + c*c );
	slice_dir[1] = 2.0f * ( b*c - a*d );

	read_dir[2]  = 2.0f * ( a*c - b*d );
	phase_dir[2] = 2.0f * ( b*c + a*d );
	slice_dir[2] = 1.0f - 2.0f * ( a*a + b*b );
    }

#ifdef __cplusplus
} //End of extern C
} //End of namespace
#endif
