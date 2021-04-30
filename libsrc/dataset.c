/* Language and Cross platform section for defining types */
#ifdef __cplusplus
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#else
/* C99 compiler */
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#endif /* __cplusplus */

#include <hdf5.h>
#include <ismrmrd/waveform.h>
#include "ismrmrd/dataset.h"

#ifdef __cplusplus
namespace ISMRMRD {
extern "C" {
#endif

/******************************/
/* Private (Static) Functions */
/******************************/

static herr_t walk_hdf5_errors(unsigned int n, const H5E_error2_t *desc, void *client_data)
{
    (void)n;
    (void)client_data;
    ismrmrd_push_error(desc->file_name, desc->line, desc->func_name, ISMRMRD_HDF5ERROR, desc->desc);
    return 0;
}

static bool link_exists(const ISMRMRD_Dataset *dset, const char *link_path) {
    htri_t val = H5Lexists(dset->fileid, link_path, H5P_DEFAULT);

    if (NULL == dset) {
        ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "NULL Dataset parameter");
        return false;
    }

    if (val < 0 ) {
        return false;
    }
    else if (val) {
        return true;
    }
    else {
        return false;
    }
}

static int create_link(const ISMRMRD_Dataset *dset, const char *link_path) {
    hid_t lcpl_id, gid;

    if (NULL == dset) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "NULL Dataset parameter");
    }

    if (link_exists(dset, link_path)) {
        return ISMRMRD_NOERROR;
    }
    else {
        lcpl_id = H5Pcreate(H5P_LINK_CREATE);
        H5Pset_create_intermediate_group(lcpl_id, 1);
        gid = H5Gcreate2(dset->fileid, link_path, lcpl_id, H5P_DEFAULT, H5P_DEFAULT);
        H5Gclose(gid);
        H5Pclose(lcpl_id);
        /* TODO can this thing ever return an error? */
        return ISMRMRD_NOERROR;
    }
}

static char * make_path(const ISMRMRD_Dataset *dset, const char * var) {
    char *path = NULL;
    size_t len = 0;

    if (NULL == dset) {
        ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "NULL Dataset parameter");
        return NULL;
    }
    if (NULL == var) {
        ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "NULL var parameter");
        return NULL;
    }

    len = strlen(dset->groupname) + strlen(var) + 2;
    path = (char *)malloc(len);
    if (path == NULL) {
        ISMRMRD_PUSH_ERR(ISMRMRD_MEMORYERROR, "Failed to malloc path");
        return NULL;
    }
    memset(path, 0, len);
    strcat(path, dset->groupname);
    strcat(path, "/");
    strcat(path, var);
    return path;
}

static char * append_to_path(const ISMRMRD_Dataset *dset,
        const char * path, const char * var)
{
    size_t len = 0;
    char *newpath = NULL;

    if (NULL == dset) {
        ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "NULL Dataset parameter");
        return NULL;
    }
    if (NULL == path) {
        ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "NULL path parameter");
        return NULL;
    }
    if (NULL == var) {
        ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "NULL var parameter");
        return NULL;
    }

    len = strlen(path) + strlen(var) + 2;
    newpath = (char *) malloc(len);
    if (newpath == NULL) {
        ISMRMRD_PUSH_ERR(ISMRMRD_MEMORYERROR, "Failed to realloc newpath");
        return NULL;
    }
    memset(newpath, 0, len);
    strcat(newpath, path);
    strcat(newpath, "/");
    strcat(newpath, var);
    return newpath;
}

static int delete_var(const ISMRMRD_Dataset *dset, const char *var) {
    int status = ISMRMRD_NOERROR;
    herr_t h5status;
    char *path = NULL;

    if (NULL == dset) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "NULL Dataset parameter");
    }

    path = make_path(dset, var);
    if (link_exists(dset, path)) {
        h5status = H5Ldelete(dset->fileid, path, H5P_DEFAULT);
        if (h5status < 0) {
            H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
            status = ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to delete H5 path");
        }
    }
    free(path);
    return status;
}

/*********************************************/
/* Private (Static) Functions for HDF5 Types */
/*********************************************/
typedef struct HDF5_Acquisiton
{
    ISMRMRD_AcquisitionHeader head;
    hvl_t traj;
    hvl_t data;
} HDF5_Acquisition;

typedef struct HDF5_Waveform
{
    ISMRMRD_WaveformHeader head;
    hvl_t data;
} HDF5_Waveform;

static hid_t get_hdf5type_uint16(void) {
    hid_t datatype = H5Tcopy(H5T_NATIVE_UINT16);
    return datatype;
}

static hid_t get_hdf5type_int16(void) {
    hid_t datatype = H5Tcopy(H5T_NATIVE_INT16);
    return datatype;
}

static hid_t get_hdf5type_uint32(void) {
    hid_t datatype = H5Tcopy(H5T_NATIVE_UINT32);
    return datatype;
}
    
static hid_t get_hdf5type_int32(void) {
    hid_t datatype = H5Tcopy(H5T_NATIVE_INT32);
    return datatype;
}

static hid_t get_hdf5type_float(void) {
    hid_t datatype = H5Tcopy(H5T_NATIVE_FLOAT);
    return datatype;
}

static hid_t get_hdf5type_double(void) {
    hid_t datatype = H5Tcopy(H5T_NATIVE_DOUBLE);
    return datatype;
}

/* TODO for all get_hdf5type_xxx functions:
 *      Check return code of each H5Tinsert call */

static hid_t get_hdf5type_complexfloat(void) {
    hid_t datatype;
    herr_t h5status;
    datatype = H5Tcreate(H5T_COMPOUND, sizeof(complex_float_t));
    h5status = H5Tinsert(datatype, "real", 0, H5T_NATIVE_FLOAT);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed get complex float data type");
    }
    h5status = H5Tinsert(datatype, "imag", sizeof(float), H5T_NATIVE_FLOAT);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed get complex float data type");
    }
    return datatype;
}
    
static hid_t get_hdf5type_complexdouble(void) {
    hid_t datatype;
    herr_t h5status;
    datatype = H5Tcreate(H5T_COMPOUND, sizeof(complex_double_t));
    h5status = H5Tinsert(datatype, "real", 0, H5T_NATIVE_DOUBLE);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed get complex double data type");
    }
    h5status = H5Tinsert(datatype, "imag", sizeof(double), H5T_NATIVE_DOUBLE);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed get complex double data type");
    }
    return datatype;
}

static hid_t get_hdf5type_xmlheader(void) {
    hid_t datatype = H5Tcopy(H5T_C_S1);
    herr_t h5status = H5Tset_size(datatype, H5T_VARIABLE);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed get XML header data type");
    }
    return datatype;
}

static hid_t get_hdf5type_encoding(void) {
    hid_t datatype;
    herr_t h5status;
    hsize_t arraydims[] = {ISMRMRD_USER_INTS};
    hid_t arraytype;
    datatype = H5Tcreate(H5T_COMPOUND, sizeof(ISMRMRD_EncodingCounters));
    h5status = H5Tinsert(datatype, "kspace_encode_step_1", HOFFSET(ISMRMRD_EncodingCounters, kspace_encode_step_1),  H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "kspace_encode_step_2", HOFFSET(ISMRMRD_EncodingCounters, kspace_encode_step_2), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "average", HOFFSET(ISMRMRD_EncodingCounters, average), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "slice", HOFFSET(ISMRMRD_EncodingCounters, slice), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "contrast", HOFFSET(ISMRMRD_EncodingCounters, contrast), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "phase", HOFFSET(ISMRMRD_EncodingCounters, phase), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "repetition", HOFFSET(ISMRMRD_EncodingCounters, repetition), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "set", HOFFSET(ISMRMRD_EncodingCounters, set), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "segment", HOFFSET(ISMRMRD_EncodingCounters, segment), H5T_NATIVE_UINT16);
    arraytype = H5Tarray_create2(H5T_NATIVE_UINT16, 1, arraydims);
    h5status = H5Tinsert(datatype, "user", HOFFSET(ISMRMRD_EncodingCounters, user), arraytype);
    if (h5status < 0) {
        ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed get endoding data type");
    }
    H5Tclose(arraytype);
    return datatype;
}



static hid_t get_hdf5type_acquisitionheader(void) {
    hid_t datatype;
    herr_t h5status;
    hsize_t arraydims[1];
    hid_t vartype;
    
    datatype = H5Tcreate(H5T_COMPOUND, sizeof(ISMRMRD_AcquisitionHeader));
    h5status = H5Tinsert(datatype, "version", HOFFSET(ISMRMRD_AcquisitionHeader, version), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "flags", HOFFSET(ISMRMRD_AcquisitionHeader, flags), H5T_NATIVE_UINT64);
    h5status = H5Tinsert(datatype, "measurement_uid", HOFFSET(ISMRMRD_AcquisitionHeader,  measurement_uid), H5T_NATIVE_UINT32);
    h5status = H5Tinsert(datatype, "scan_counter", HOFFSET(ISMRMRD_AcquisitionHeader, scan_counter), H5T_NATIVE_UINT32);
    h5status = H5Tinsert(datatype, "acquisition_time_stamp", HOFFSET(ISMRMRD_AcquisitionHeader, acquisition_time_stamp), H5T_NATIVE_UINT32);
    
    arraydims[0] = ISMRMRD_PHYS_STAMPS;
    vartype = H5Tarray_create2(H5T_NATIVE_UINT32, 1, arraydims);
    h5status = H5Tinsert(datatype, "physiology_time_stamp", HOFFSET(ISMRMRD_AcquisitionHeader, physiology_time_stamp), vartype);
    H5Tclose(vartype);
    
    h5status = H5Tinsert(datatype, "number_of_samples", HOFFSET(ISMRMRD_AcquisitionHeader, number_of_samples), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "available_channels", HOFFSET(ISMRMRD_AcquisitionHeader, available_channels), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "active_channels", HOFFSET(ISMRMRD_AcquisitionHeader, active_channels), H5T_NATIVE_UINT16);
    
    arraydims[0] = ISMRMRD_CHANNEL_MASKS;
    vartype = H5Tarray_create2(H5T_NATIVE_UINT64, 1, arraydims);
    h5status = H5Tinsert(datatype, "channel_mask", HOFFSET(ISMRMRD_AcquisitionHeader, channel_mask), vartype);
    H5Tclose(vartype);
    
    h5status = H5Tinsert(datatype, "discard_pre", HOFFSET(ISMRMRD_AcquisitionHeader, discard_pre), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "discard_post", HOFFSET(ISMRMRD_AcquisitionHeader, discard_post), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "center_sample", HOFFSET(ISMRMRD_AcquisitionHeader, center_sample), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "encoding_space_ref", HOFFSET(ISMRMRD_AcquisitionHeader, encoding_space_ref), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "trajectory_dimensions", HOFFSET(ISMRMRD_AcquisitionHeader, trajectory_dimensions), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "sample_time_us", HOFFSET(ISMRMRD_AcquisitionHeader, sample_time_us), H5T_NATIVE_FLOAT);
    
    arraydims[0] = 3;
    vartype = H5Tarray_create2(H5T_NATIVE_FLOAT, 1, arraydims);
    h5status = H5Tinsert(datatype, "position", HOFFSET(ISMRMRD_AcquisitionHeader, position), vartype);
    h5status = H5Tinsert(datatype, "read_dir", HOFFSET(ISMRMRD_AcquisitionHeader, read_dir), vartype);
    h5status = H5Tinsert(datatype, "phase_dir", HOFFSET(ISMRMRD_AcquisitionHeader, phase_dir), vartype);
    h5status = H5Tinsert(datatype, "slice_dir", HOFFSET(ISMRMRD_AcquisitionHeader, slice_dir), vartype);
    h5status = H5Tinsert(datatype, "patient_table_position", HOFFSET(ISMRMRD_AcquisitionHeader, patient_table_position), vartype);
    H5Tclose(vartype);
    
    vartype = get_hdf5type_encoding();
    h5status = H5Tinsert(datatype, "idx", HOFFSET(ISMRMRD_AcquisitionHeader, idx), vartype);
    H5Tclose(vartype);
    
    arraydims[0] = ISMRMRD_USER_INTS;
    vartype = H5Tarray_create2(H5T_NATIVE_INT32, 1, arraydims);
    h5status = H5Tinsert(datatype, "user_int", HOFFSET(ISMRMRD_AcquisitionHeader, user_int), vartype);
    H5Tclose(vartype);
    
    arraydims[0] = ISMRMRD_USER_FLOATS;
    vartype = H5Tarray_create2(H5T_NATIVE_FLOAT, 1, arraydims);
    h5status = H5Tinsert(datatype, "user_float", HOFFSET(ISMRMRD_AcquisitionHeader, user_float), vartype);
    H5Tclose(vartype);
    
    /* Clean up */
    if (h5status < 0) {
        ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed get acquisitionheader data type");
    }
    
    return datatype;   
}

static hid_t get_hdf5type_acquisition(void) {
    hid_t datatype, vartype, vlvartype;
    herr_t h5status;
    
    datatype = H5Tcreate(H5T_COMPOUND, sizeof(HDF5_Acquisition));
    vartype = get_hdf5type_acquisitionheader();
    h5status = H5Tinsert(datatype, "head", HOFFSET(HDF5_Acquisition, head), vartype);
    H5Tclose(vartype);
    vartype =  get_hdf5type_float();
    vlvartype = H5Tvlen_create(vartype);
    h5status = H5Tinsert(datatype, "traj", HOFFSET(HDF5_Acquisition, traj), vlvartype);
    H5Tclose(vartype);
    H5Tclose(vlvartype);
    
    /* Store acquisition data as an array of floats */
    vartype = get_hdf5type_float();
    vlvartype = H5Tvlen_create(vartype);
    h5status = H5Tinsert(datatype, "data", HOFFSET(HDF5_Acquisition, data), vlvartype);
    H5Tclose(vartype);
    H5Tclose(vlvartype);
    
    if (h5status < 0) {
        ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed get acquisition data type");
    }
    
    return datatype;
}

static hid_t get_hdf5type_imageheader(void) {
    hid_t datatype;
    herr_t h5status;
    hsize_t arraydims[1];
    hid_t vartype;
    
    datatype = H5Tcreate(H5T_COMPOUND, sizeof(ISMRMRD_ImageHeader));
    h5status = H5Tinsert(datatype, "version", HOFFSET(ISMRMRD_ImageHeader, version), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "data_type", HOFFSET(ISMRMRD_ImageHeader, data_type), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "flags", HOFFSET(ISMRMRD_ImageHeader, flags), H5T_NATIVE_UINT64);
    h5status = H5Tinsert(datatype, "measurement_uid", HOFFSET(ISMRMRD_ImageHeader,  measurement_uid), H5T_NATIVE_UINT32);
    arraydims[0] = 3;
    vartype = H5Tarray_create2(H5T_NATIVE_UINT16, 1, arraydims);
    h5status = H5Tinsert(datatype, "matrix_size", HOFFSET(ISMRMRD_ImageHeader, matrix_size), vartype);
    H5Tclose(vartype);

    vartype = H5Tarray_create2(H5T_NATIVE_FLOAT, 1, arraydims);
    h5status = H5Tinsert(datatype, "field_of_view", HOFFSET(ISMRMRD_ImageHeader, field_of_view), vartype);
    h5status = H5Tinsert(datatype, "channels", HOFFSET(ISMRMRD_ImageHeader, channels), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "position", HOFFSET(ISMRMRD_ImageHeader, position), vartype);
    h5status = H5Tinsert(datatype, "read_dir", HOFFSET(ISMRMRD_ImageHeader, read_dir), vartype);
    h5status = H5Tinsert(datatype, "phase_dir", HOFFSET(ISMRMRD_ImageHeader, phase_dir), vartype);
    h5status = H5Tinsert(datatype, "slice_dir", HOFFSET(ISMRMRD_ImageHeader, slice_dir), vartype);
    h5status = H5Tinsert(datatype, "patient_table_position", HOFFSET(ISMRMRD_ImageHeader, patient_table_position), vartype);
    H5Tclose(vartype);

    h5status = H5Tinsert(datatype, "average", HOFFSET(ISMRMRD_ImageHeader, average), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "slice", HOFFSET(ISMRMRD_ImageHeader, slice), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "contrast", HOFFSET(ISMRMRD_ImageHeader, contrast), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "phase", HOFFSET(ISMRMRD_ImageHeader, phase), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "repetition", HOFFSET(ISMRMRD_ImageHeader, repetition), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "set", HOFFSET(ISMRMRD_ImageHeader, set), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "acquisition_time_stamp", HOFFSET(ISMRMRD_ImageHeader, acquisition_time_stamp), H5T_NATIVE_UINT32);
    arraydims[0] = ISMRMRD_PHYS_STAMPS;
    vartype = H5Tarray_create2(H5T_NATIVE_UINT32, 1, arraydims);
    h5status = H5Tinsert(datatype, "physiology_time_stamp", HOFFSET(ISMRMRD_ImageHeader, physiology_time_stamp), vartype);
    H5Tclose(vartype);

    h5status = H5Tinsert(datatype, "image_type", HOFFSET(ISMRMRD_ImageHeader, image_type), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "image_index", HOFFSET(ISMRMRD_ImageHeader, image_index), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "image_series_index", HOFFSET(ISMRMRD_ImageHeader, image_series_index), H5T_NATIVE_UINT16);
    
    arraydims[0] = ISMRMRD_USER_INTS;
    vartype = H5Tarray_create2(H5T_NATIVE_INT32, 1, arraydims);
    h5status = H5Tinsert(datatype, "user_int", HOFFSET(ISMRMRD_ImageHeader, user_int), vartype);
    H5Tclose(vartype);

    arraydims[0] = ISMRMRD_USER_FLOATS;
    vartype = H5Tarray_create2(H5T_NATIVE_FLOAT, 1, arraydims);
    h5status = H5Tinsert(datatype, "user_float", HOFFSET(ISMRMRD_ImageHeader, user_float), vartype);
    H5Tclose(vartype);

    h5status = H5Tinsert(datatype, "attribute_string_len", HOFFSET(ISMRMRD_ImageHeader, attribute_string_len), H5T_NATIVE_UINT32);
    
    /* Clean up */
    if (h5status < 0) {
        ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed get imageheader data type");
    }
    
    return datatype;   
}

static hid_t get_hdf5type_image_attribute_string(void) {
    hid_t datatype = H5Tcopy(H5T_C_S1);
    herr_t h5status = H5Tset_size(datatype, H5T_VARIABLE);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed get image attribute string data type");
    }
    return datatype;
}



static hid_t get_hdf5type_waveformheader(void) {
    hid_t datatype;
    herr_t h5status;

	datatype = H5Tcreate(H5T_COMPOUND, sizeof(ISMRMRD_WaveformHeader));
    h5status = H5Tinsert(datatype, "version", HOFFSET(ISMRMRD_WaveformHeader, version), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "flags", HOFFSET(ISMRMRD_WaveformHeader, flags), H5T_NATIVE_UINT64);
    h5status = H5Tinsert(datatype, "measurement_uid", HOFFSET(ISMRMRD_WaveformHeader,  measurement_uid), H5T_NATIVE_UINT32);
    h5status = H5Tinsert(datatype, "scan_counter", HOFFSET(ISMRMRD_WaveformHeader, scan_counter), H5T_NATIVE_UINT32);
    h5status = H5Tinsert(datatype, "time_stamp", HOFFSET(ISMRMRD_WaveformHeader, time_stamp ), H5T_NATIVE_UINT32);
    h5status = H5Tinsert(datatype, "number_of_samples", HOFFSET(ISMRMRD_WaveformHeader, number_of_samples), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "channels", HOFFSET(ISMRMRD_WaveformHeader, channels), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "sample_time_us", HOFFSET(ISMRMRD_WaveformHeader, sample_time_us), H5T_NATIVE_FLOAT);
	h5status = H5Tinsert(datatype, "waveform_id", HOFFSET(ISMRMRD_WaveformHeader, waveform_id), H5T_NATIVE_UINT16);

      /* Clean up */
    if (h5status < 0) {
        ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed get waveform header data type");
    }

    return  datatype;

}

static hid_t get_hdf5type_waveform(void) {
    hid_t datatype, vartype, vlvartype;
    herr_t h5status;

    datatype = H5Tcreate(H5T_COMPOUND, sizeof(HDF5_Waveform));
    vartype = get_hdf5type_waveformheader();
    h5status = H5Tinsert(datatype, "head", HOFFSET(HDF5_Waveform, head), vartype);
	if (h5status < 0) {
		ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed get waveform header data type");
	}
    H5Tclose(vartype);
	vartype = get_hdf5type_uint32();
    vlvartype = H5Tvlen_create(vartype);
    h5status = H5Tinsert(datatype, "data", HOFFSET(HDF5_Waveform, data), vlvartype);
    H5Tclose(vartype);
    H5Tclose(vlvartype);

    /* Store acquisition data as an array of floats */

    if (h5status < 0) {
        ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed get waveform data type");
    }

    return datatype;
}
static hid_t get_hdf5type_ndarray(uint16_t data_type) {
    
    hid_t hdfdatatype = -1;
    switch (data_type) {
        case ISMRMRD_USHORT:
            hdfdatatype = get_hdf5type_uint16();
            break;
        case ISMRMRD_SHORT:
            hdfdatatype = get_hdf5type_int16();
            break;
        case ISMRMRD_UINT:
            hdfdatatype = get_hdf5type_uint32();
            break;
        case ISMRMRD_INT:
            hdfdatatype = get_hdf5type_int32();
            break;
        case ISMRMRD_FLOAT:
            hdfdatatype = get_hdf5type_float();
            break;
        case ISMRMRD_DOUBLE:
            hdfdatatype = get_hdf5type_double();
            break;
        case ISMRMRD_CXFLOAT:
            hdfdatatype = get_hdf5type_complexfloat();
            break;
        case ISMRMRD_CXDOUBLE:
            hdfdatatype = get_hdf5type_complexdouble();
            break;
        default :
            ISMRMRD_PUSH_ERR(ISMRMRD_TYPEERROR, "Failed to get HDF5 data type.");
    }
    return hdfdatatype;
}

static uint16_t get_ndarray_data_type(hid_t hdf5type) {

    uint16_t dtype = 0;
    hid_t t;
    
    t = get_hdf5type_uint16();
    if (H5Tequal(hdf5type, t)) {
        dtype = ISMRMRD_USHORT;
    }
    H5Tclose(t);

    t = get_hdf5type_int16();
    if (H5Tequal(hdf5type, t)) {
        dtype = ISMRMRD_SHORT;
    }
    H5Tclose(t);

    t = get_hdf5type_uint32();
    if (H5Tequal(hdf5type, t)) {
        dtype = ISMRMRD_UINT;
    }
    H5Tclose(t);

    t = get_hdf5type_int32();
    if (H5Tequal(hdf5type, t)) {
        dtype = ISMRMRD_INT;
    }
    H5Tclose(t);

    t = get_hdf5type_float();
    if (H5Tequal(hdf5type, t)) {
        dtype = ISMRMRD_FLOAT;
    }
    H5Tclose(t);

    t = get_hdf5type_double();
    if (H5Tequal(hdf5type, t)) {
        dtype = ISMRMRD_DOUBLE;
    }
    H5Tclose(t);

    t = get_hdf5type_complexfloat();
    if (H5Tequal(hdf5type, t)) {
        dtype = ISMRMRD_CXFLOAT;
    }
    H5Tclose(t);

    t = get_hdf5type_complexdouble();
    if (H5Tequal(hdf5type, t)) {
        dtype = ISMRMRD_CXDOUBLE;
    }
    H5Tclose(t);

    if (dtype == 0) {
        //ISMRMRD_PUSH_ERR(ISMRMRD_TYPEERROR, "Failed to get data type from HDF5 data type.");
    }
    
    return dtype;
}

static uint32_t get_number_of_elements(const ISMRMRD_Dataset *dset, const char * path)
{
    herr_t h5status;
    uint32_t num;

    if (NULL == dset) {
        ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "NULL Dataset parameter");
        return 0;
    }

    if (link_exists(dset, path)) {
        hid_t dataset, dataspace;
        hsize_t rank, *dims, *maxdims;
        dataset = H5Dopen2(dset->fileid, path, H5P_DEFAULT);
        dataspace = H5Dget_space(dataset);
        rank = H5Sget_simple_extent_ndims(dataspace);
        dims = (hsize_t *) malloc(rank*sizeof(hsize_t));
        maxdims = (hsize_t *) malloc(rank*sizeof(hsize_t));
        h5status = H5Sget_simple_extent_dims(dataspace, dims, maxdims);
        num = (uint32_t)dims[0];
        free(dims);
        free(maxdims);
        h5status = H5Sclose(dataspace);
        h5status= H5Dclose(dataset);
        if (h5status < 0) {
            H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
            ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR,
                    "Failed to get number of elements in vector.");
        }
    }
    else {
        /* none */
        num = 0;
    }
    
    return num;
}

static int append_element(const ISMRMRD_Dataset * dset, const char * path,
        void * elem, const hid_t datatype,
        const uint16_t ndim, const size_t *dims)
{
    hid_t dataset, dataspace, props, filespace, memspace;
    herr_t h5status = 0;
    hsize_t *hdfdims = NULL, *ext_dims = NULL, *offset = NULL, *maxdims = NULL, *chunk_dims = NULL;
    int n = 0, rank = 0;
    
    if (NULL == dset) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "NULL Dataset parameter");
    }

    /* Check the path and find rank */
    if (link_exists(dset, path)) {
        /* open dataset */
        dataset = H5Dopen2(dset->fileid, path, H5P_DEFAULT);
        /* TODO check that the header dataset's datatype is correct */
        dataspace = H5Dget_space(dataset);
        rank = H5Sget_simple_extent_ndims(dataspace);
        if (rank != ndim + 1) {
            return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Dimensions are incorrect.");
        }
    } else {
        dataset   = -1; /* will be initialized below */
        dataspace = -1; /* will be initialized below */
        rank = ndim + 1;
    }

    hdfdims = (hsize_t *) malloc(rank * sizeof(hsize_t));
    maxdims = (hsize_t *) malloc(rank * sizeof(hsize_t));
    offset = (hsize_t *) malloc(rank * sizeof(hsize_t));
    ext_dims = (hsize_t *) malloc(rank * sizeof(hsize_t));
    chunk_dims = (hsize_t *) malloc(rank * sizeof(hsize_t));

    /* extend or create if needed, and select the last block */
    if (link_exists(dset, path)) {
        h5status = H5Sget_simple_extent_dims(dataspace, hdfdims, maxdims);
        for (n = 0; n<ndim; n++) {
            if (dims[n] != hdfdims[n+1]) {
                free(hdfdims);
                free(ext_dims);
                free(offset);
                free(maxdims);
                free(chunk_dims);
                return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Dimensions are incorrect.");
            }
        }
        /* extend it by one */
        hdfdims[0] += 1;
        h5status = H5Dset_extent(dataset, hdfdims);
        /* Select the last block */
        ext_dims[0] = 1;
        for (n = 0; n < ndim; n++) {
            offset[n + 1] = 0;
            ext_dims[n + 1] = dims[n];
        }
    } else {
        hdfdims[0] = 1;
        maxdims[0] = H5S_UNLIMITED;
        ext_dims[0] = 1;
        chunk_dims[0] = 1;
        for (n = 0; n < ndim; n++) {
            hdfdims[n + 1] = dims[n];
            maxdims[n + 1] = dims[n];
            offset[n + 1] = 0;
            ext_dims[n + 1] = dims[n];
            chunk_dims[n + 1] = dims[n];
        }
        dataspace = H5Screate_simple(rank, hdfdims, maxdims);
        props = H5Pcreate(H5P_DATASET_CREATE);
        /* enable chunking so that the dataset is extensible */
        h5status = H5Pset_chunk (props, rank, chunk_dims);
        /* create */
        dataset = H5Dcreate2(dset->fileid, path, datatype, dataspace, H5P_DEFAULT, props,  H5P_DEFAULT);
        if (dataset < 0) {
            free(hdfdims);
            free(ext_dims);
            free(offset);
            free(maxdims);
            free(chunk_dims);
            H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
            return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to create dataset");
        }
        h5status = H5Pclose(props);
        if (h5status < 0) {
            free(hdfdims);
            free(ext_dims);
            free(offset);
            free(maxdims);
            free(chunk_dims);
            H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
            return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to close property list");
        }
    }

    /* Select the last block */
    offset[0] = hdfdims[0]-1;
    filespace = H5Dget_space(dataset);
    h5status  = H5Sselect_hyperslab (filespace, H5S_SELECT_SET, offset, NULL, ext_dims, NULL);
	
	if (h5status < 0) {
	
		H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
		return ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to select hyperslab");
	}
    memspace = H5Screate_simple(rank, ext_dims, NULL);

    free(hdfdims);
    free(ext_dims);
    free(offset);
    free(maxdims);
    free(chunk_dims);

    /* Write it */
    /* since this is a 1 element array we can just pass the pointer to the header */
    h5status = H5Dwrite(dataset, datatype, memspace, filespace, H5P_DEFAULT, elem);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to write dataset");
    }

    /* Clean up */
    h5status = H5Sclose(dataspace);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to close dataspace");
    }
    h5status = H5Sclose(filespace);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to close filespace");
    }
    h5status = H5Sclose(memspace);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to close memspace");
    }
    h5status = H5Dclose(dataset);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to close dataset");
    }

    return ISMRMRD_NOERROR;
}

static int get_array_properties(const ISMRMRD_Dataset *dset, const char *path,
        uint16_t *ndim, size_t dims[ISMRMRD_NDARRAY_MAXDIM],
        uint16_t *data_type)
{
    hid_t dataset, filespace, hdf5type;
    hsize_t *hdfdims = NULL;
    herr_t h5status = 0;
    int rank, n;

    if (NULL == dset) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
    }

    /* Check path existence */
    if (!link_exists(dset, path)) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Path to element not found.");
    }

    /* open dataset */
    dataset = H5Dopen2(dset->fileid, path, H5P_DEFAULT);

    /* get the data type */
    hdf5type = H5Dget_type(dataset);

    /* get the file space */
    filespace = H5Dget_space(dataset);

    /* get the rank */
    rank = H5Sget_simple_extent_ndims(filespace);

    /* get the dimensions */
    hdfdims = (hsize_t *)malloc(rank * sizeof(*hdfdims));
    h5status = H5Sget_simple_extent_dims(filespace, hdfdims, NULL);

    /* set the return values - permute dimensions */
    *data_type = get_ndarray_data_type(hdf5type);
    *ndim = rank;
    for (n=0; n<rank; n++) {
        dims[n] = hdfdims[rank-n-1];
    }

    free(hdfdims);

    /* clean up */
    h5status = H5Tclose(hdf5type);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to close datatype.");
    }
    h5status = H5Sclose(filespace);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to close filespace");
    }
    h5status = H5Dclose(dataset);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to close dataset.");
    }

    return ISMRMRD_NOERROR;

}

int read_element(const ISMRMRD_Dataset *dset, const char *path, void *elem,
        const hid_t datatype, const uint32_t index)
{
    hid_t dataset, filespace, memspace;
    hsize_t *hdfdims = NULL, *offset = NULL, *count = NULL;
    herr_t h5status = 0;
    int rank = 0;
    int n;
    int ret_code = ISMRMRD_NOERROR;


    if (NULL == dset) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
    }

    /* Check path existence */
    if (!link_exists(dset, path)) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Path to element not found.");
    }

    /* open dataset */
    dataset = H5Dopen2(dset->fileid, path, H5P_DEFAULT);

    /* TODO check that the dataset's datatype is correct */
    filespace = H5Dget_space(dataset);

    rank = H5Sget_simple_extent_ndims(filespace);

    hdfdims = (hsize_t *)malloc(rank * sizeof(*hdfdims));
    offset = (hsize_t *)malloc(rank * sizeof(*offset));
    count = (hsize_t *)malloc(rank * sizeof(*count));

    h5status = H5Sget_simple_extent_dims(filespace, hdfdims, NULL);

    if (index >= hdfdims[0]) {
        ret_code = ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Index out of range.");
        goto cleanup;
    }

    offset[0] = index;
    count[0] = 1;
    for (n=1; n< rank; n++) {
        offset[n] = 0;
        count[n] = hdfdims[n];
    }

    h5status = H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offset, NULL, count, NULL);

    /* create space for one */
    memspace = H5Screate_simple(rank, count, NULL);

    h5status = H5Dread(dataset, datatype, memspace, filespace, H5P_DEFAULT, elem);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        ret_code = ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to read from dataset.");
        goto cleanup;
    }

    h5status = H5Sclose(filespace);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        ret_code = ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to close filespace.");
        goto cleanup;
    }
    h5status = H5Sclose(memspace);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        ret_code = ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to close memspace.");
        goto cleanup;
    }
    h5status = H5Dclose(dataset);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        ret_code = ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to close dataset.");
        goto cleanup;
    }

cleanup:
    free(count);
    free(offset);
    free(hdfdims);
    return ret_code;
}

/********************/
/* Public functions */
/********************/
int ismrmrd_init_dataset(ISMRMRD_Dataset *dset, const char *filename,
        const char *groupname)
{
    if (NULL == dset) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "NULL Dataset parameter");
    }

    /* Disable HDF5 automatic error prenting */
    H5Eset_auto2(H5E_DEFAULT, NULL, NULL);

    dset->filename = (char *) malloc(strlen(filename) + 1);
    if (dset->filename == NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_MEMORYERROR, "Failed to malloc dataset filename");
    }
    strcpy(dset->filename, filename);

    dset->groupname = (char *) malloc(strlen(groupname) + 1);
    if (dset->groupname == NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_MEMORYERROR, "Failed to malloc dataset groupname");
    }
    strcpy(dset->groupname, groupname);

    dset->fileid = 0;
    return ISMRMRD_NOERROR;
}

int ismrmrd_open_dataset(ISMRMRD_Dataset *dset, const bool create_if_needed) {
    /* TODO add a mode for clobbering the dataset if it exists. */
    hid_t fileid;

    if (NULL == dset) {
        ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "NULL Dataset parameter");
        return false;
    }

    /* Try opening the file */
    /* Note the is_hdf5 function doesn't work well when trying to open multiple files */
    fileid = H5Fopen(dset->filename, H5F_ACC_RDWR, H5P_DEFAULT);

    if (fileid > 0) {
        dset->fileid = fileid;
    }
    else if (create_if_needed == false) {
        /*Try opening the file as read-only*/
        fileid = H5Fopen(dset->filename, H5F_ACC_RDONLY, H5P_DEFAULT);
        if (fileid > 0) {
            dset->fileid = fileid;
        }
        else{
            H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
            /* Some sort of error opening the file - Maybe it doesn't exist? */
            return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to open file.");
        }
    }
    else {
        /* Try creating a new file using the default properties. */
        /* this will be readwrite */
        fileid = H5Fcreate(dset->filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
        if (fileid > 0) {
            dset->fileid = fileid;
        }
        else {
            /* Error opening the file */
            H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
            return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to open file.");
        }
    }
    /* Open the existing dataset */
    /* ensure that /groupname exists */
    create_link(dset, dset->groupname);

    return ISMRMRD_NOERROR;
}

int ismrmrd_close_dataset(ISMRMRD_Dataset *dset) {
    herr_t h5status;

    if (NULL == dset) {
        ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "NULL Dataset parameter");
        return false;
    }

    if (dset->filename != NULL) {
        free(dset->filename);
        dset->filename = NULL;
    }

    if (dset->groupname != NULL) {
        free(dset->groupname);
        dset->groupname = NULL;
    }

    /* Check for a valid fileid before trying to close the file */
    if (dset->fileid > 0) {
        h5status = H5Fclose (dset->fileid);
        dset->fileid = 0;
        if (h5status < 0) {
            H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
            return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to close dataset.");
        }
    }

    return ISMRMRD_NOERROR;
}

int ismrmrd_write_header(const ISMRMRD_Dataset *dset, const char *xmlstring) {
    hid_t dataset, dataspace, datatype, props;
    hsize_t dims[] = {1};
    herr_t h5status;
    void *buff[1];
    char * path;

    if (dset==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
    }

    if (xmlstring==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "xmlstring should not be NULL.");
    }

    /* The path to the xml header */
    path = make_path(dset, "xml");

    /* Delete the old header if it exists */
    h5status = delete_var(dset, "xml");

    /* Create a new dataset for the xmlstring */
    /* i.e. create the memory type, data space, and data set */
    dataspace = H5Screate_simple(1, dims, NULL);
    datatype = get_hdf5type_xmlheader();
    props = H5Pcreate (H5P_DATASET_CREATE);
    dataset = H5Dcreate2(dset->fileid, path, datatype, dataspace, H5P_DEFAULT, props,  H5P_DEFAULT);
    free(path);

    /* Write it out */
    /* We have to wrap the xmlstring in an array */
    buff[0] = (void *) xmlstring;  /* safe to get rid of const the type */
    h5status = H5Dwrite(dataset, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, buff);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to write xml string to dataset");
    }

    /* Clean up */
    h5status = H5Pclose(props);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to close property list.");
    }
    h5status = H5Tclose(datatype);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to close datatype.");
    }
    h5status = H5Sclose(dataspace);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to close dataspace.");
    }
    h5status = H5Dclose(dataset);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to close dataset.");
    }

    return ISMRMRD_NOERROR;
}

char * ismrmrd_read_header(const ISMRMRD_Dataset *dset) {
    hid_t dataset, datatype;
    herr_t h5status;
    char* xmlstring = NULL;
    char* path = NULL;

    if (dset==NULL) {
        ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Pointer should not be NULL.");
        return NULL;
    }

    /* The path to the xml header */
    path = make_path(dset, "xml");

    if (!link_exists(dset, path)) {
        /* No XML String found */
        ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "No XML Header found.");
        goto cleanup_path;
    }

    dataset = H5Dopen2(dset->fileid, path, H5P_DEFAULT);
    datatype = get_hdf5type_xmlheader();
    /* Read it into a 1D buffer*/
    h5status = H5Dread(dataset, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, &xmlstring);
    if (h5status < 0 || xmlstring == NULL) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to read header.");
        goto cleanup_path;
    }

    /* Clean up */
    h5status = H5Tclose(datatype);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to close XML header HDF5 datatype.");
        goto cleanup_xmlstring;
    }
    h5status = H5Dclose(dataset);
    if (h5status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to close XML header HDF5 dataset.");
        goto cleanup_xmlstring;
    }

    goto cleanup_path;

cleanup_xmlstring:
    free(xmlstring);
    xmlstring = NULL;
cleanup_path:
    free(path);
    return xmlstring;
}

uint32_t ismrmrd_get_number_of_acquisitions(const ISMRMRD_Dataset *dset) {
    char *path;
    uint32_t numacq;

    if (dset==NULL) {
        ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Pointer should not be NULL.");
        return 0;
    }
    /* The path to the acqusition data */    
    path = make_path(dset, "data");
    numacq = get_number_of_elements(dset, path);
    free(path);
    return numacq;
}

int ismrmrd_append_acquisition(const ISMRMRD_Dataset *dset, const ISMRMRD_Acquisition *acq) {
    int status;
    char *path;
    hid_t datatype;
    HDF5_Acquisition hdf5acq[1];

    if (dset==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
    }
    if (acq==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Acquisition pointer should not be NULL.");
    }

    /* The path to the acqusition data */    
    path = make_path(dset, "data");

    /* The acquisition datatype */
    datatype = get_hdf5type_acquisition();

    /* Create the HDF5 version of the acquisition */
    hdf5acq[0].head = acq->head;
    hdf5acq[0].traj.len = (size_t)(acq->head.number_of_samples) * (size_t)(acq->head.trajectory_dimensions);
    hdf5acq[0].traj.p = acq->traj;
    hdf5acq[0].data.len = 2 * (size_t)(acq->head.number_of_samples) * (size_t)(acq->head.active_channels);
    hdf5acq[0].data.p = acq->data;

    /* Write it */
    status = append_element(dset, path, hdf5acq, datatype, 0, NULL);
    if (status != ISMRMRD_NOERROR) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to append acquisition.");
    }

    free(path);

    /* Clean up */
    status = H5Tclose(datatype);
    if (status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to close datatype.");
    }

    return ISMRMRD_NOERROR;
}

int ismrmrd_read_acquisition(const ISMRMRD_Dataset *dset, uint32_t index, ISMRMRD_Acquisition *acq)
{
    hid_t datatype;
    herr_t status;
    HDF5_Acquisition hdf5acq;
    char *path;

    if (dset==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
    }
    if (acq==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Acquisition pointer should not be NULL.");
    }

    /* The path to the acquisition data */
    path = make_path(dset, "data");

    /* The acquisition datatype */
    datatype = get_hdf5type_acquisition();

    status = read_element(dset, path, &hdf5acq, datatype, index);
    memcpy(&acq->head, &hdf5acq.head, sizeof(ISMRMRD_AcquisitionHeader));
    ismrmrd_make_consistent_acquisition(acq);
    memcpy(acq->traj, hdf5acq.traj.p, ismrmrd_size_of_acquisition_traj(acq));
    memcpy(acq->data, hdf5acq.data.p, ismrmrd_size_of_acquisition_data(acq));

    /* clean up */
    free(path);
    free(hdf5acq.traj.p);
    free(hdf5acq.data.p);

    status = H5Tclose(datatype);
    if (status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to close datatype.");
    }

    return ISMRMRD_NOERROR;
}

int ismrmrd_append_image(const ISMRMRD_Dataset *dset, const char *varname, const ISMRMRD_Image *im) {
    int status;
    hid_t datatype;
    char *path, *headerpath, *attrpath, *datapath;
    size_t dims[4];

    if (dset==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
    }
    if (varname==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Varname should not be NULL.");
    }
    if (im==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Image pointer should not be NULL.");
    }

    /* The group for this set of images */
    /* /groupname/varname */
    path = make_path(dset, varname);
    /* Make sure the path exists */
    create_link(dset, path);        

    /* Handle the header */
    headerpath = append_to_path(dset, path, "header");
    datatype = get_hdf5type_imageheader();
    status = append_element(dset, headerpath, (void *) &im->head, datatype, 0, NULL);
    if (status != ISMRMRD_NOERROR) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to append image header.");
    }
    status = H5Tclose(datatype);
    free(headerpath);

    /* Handle the attribute string */
    attrpath = append_to_path(dset, path, "attributes");
    datatype = get_hdf5type_image_attribute_string();
    status = append_element(dset, attrpath, (void *) &im->attribute_string, datatype, 0, NULL);
    if (status != ISMRMRD_NOERROR) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to append image attribute string.");
    }
    status = H5Tclose(datatype);
    free(attrpath);

    /* Handle the data */
    datapath = append_to_path(dset, path, "data");
    datatype = get_hdf5type_ndarray(im->head.data_type);
    /* permute the dimensions in the hdf5 file */
    dims[3] = im->head.matrix_size[0];
    dims[2] = im->head.matrix_size[1];
    dims[1] = im->head.matrix_size[2];
    dims[0] = im->head.channels;
    status = append_element(dset, datapath, im->data, datatype, 4, dims);
    if (status != ISMRMRD_NOERROR) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to append image data.");
    }
    status = H5Tclose(datatype);
    free(datapath);

    /* Final cleanup */
    if (status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to close datatype.");
    }
    free(path);

    return ISMRMRD_NOERROR;
}

uint32_t ismrmrd_get_number_of_images(const ISMRMRD_Dataset *dset, const char *varname)
{
    char *path, *headerpath;
    uint32_t numimages;

    if (dset==NULL) {
        ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
        return 0;
    }
    if (varname==NULL) {
        ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Varname should not be NULL.");
        return 0;
    }
    /* The group for this set of images */
    /* /groupname/varname */
    path = make_path(dset, varname);
    /* The path to the acqusition image headers */
    headerpath = append_to_path(dset, path, "header");
    numimages = get_number_of_elements(dset, headerpath);
    free(headerpath);
    free(path);
    return numimages;
}


int ismrmrd_read_image(const ISMRMRD_Dataset *dset, const char *varname,
        const uint32_t index, ISMRMRD_Image *im) {

    int status;
    hid_t datatype;
    char *path, *headerpath, *attrpath, *datapath, *attr_string;
    uint32_t numims;

    if (dset==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
    }
    if (varname==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Varname should not be NULL.");
    }
    if (im==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Image pointer should not be NULL.");
    }

    numims = ismrmrd_get_number_of_images(dset, varname);

    if (index > numims) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Index requested exceeds number of images in the dataset.");
    }

    /* The group for this set of images */
    /* /groupname/varname */
    path = make_path(dset, varname);

    /* Handle the header */
    headerpath = append_to_path(dset, path, "header");
    datatype = get_hdf5type_imageheader();
    status = read_element(dset, headerpath, (void *) &im->head, datatype, index);
    if (status != ISMRMRD_NOERROR) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to read image header.");
    }
    free(headerpath);
    H5Tclose(datatype);

    /* Allocate the memory for the attribute string and the data */
    ismrmrd_make_consistent_image(im);

    /* Handle the attribute string */
    attrpath = append_to_path(dset, path, "attributes");
    datatype = get_hdf5type_image_attribute_string();
    status = read_element(dset, attrpath, (void *) &attr_string, datatype, index);
    if (status != ISMRMRD_NOERROR) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to read image attribute string.");
    }
    free(attrpath);
    H5Tclose(datatype);

    /* copy the attribute string read from the file into the Image */
    memcpy(im->attribute_string, attr_string, ismrmrd_size_of_image_attribute_string(im));
    free(attr_string);

    /* Handle the data */
    datapath = append_to_path(dset, path, "data");
    datatype = get_hdf5type_ndarray(im->head.data_type);
    status = read_element(dset, datapath, im->data, datatype, index);
    if (status != ISMRMRD_NOERROR) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to read image data.");
    }
    free(datapath);

    /* Final cleanup */
    status = H5Tclose(datatype);
    if (status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to close datatype.");
    }
    free(path);

    return ISMRMRD_NOERROR;
}


int ismrmrd_append_waveform(const ISMRMRD_Dataset *dset, const ISMRMRD_Waveform *wav) {
    int status;
    char *path;
    hid_t datatype;
    HDF5_Waveform hdf5wav[1];

    if (dset==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
    }
    if (wav==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Acquisition pointer should not be NULL.");
    }

    /* The path to the acqusition data */
    path = make_path(dset, "waveforms");

    /* The acquisition datatype */
    datatype = get_hdf5type_waveform();

    /* Create the HDF5 version of the acquisition */
    hdf5wav[0].head = wav->head;
    hdf5wav[0].data.len = (size_t)(wav->head.number_of_samples) * (size_t)(wav->head.channels);
    hdf5wav[0].data.p = wav->data;

    /* Write it */
    status = append_element(dset, path, hdf5wav, datatype, 0, NULL);
    if (status != ISMRMRD_NOERROR) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to append acquisition.");
    }

    free(path);

    /* Clean up */
    status = H5Tclose(datatype);
    if (status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to close datatype.");
    }

    return ISMRMRD_NOERROR;
}

int ismrmrd_read_waveform(const ISMRMRD_Dataset *dset, uint32_t index, ISMRMRD_Waveform *wav)
{
    hid_t datatype;
    herr_t status;
    HDF5_Waveform hdf5wav;
    char *path;

    if (dset==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
    }
    if (wav==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Waveform pointer should not be NULL.");
    }

    /* The path to the acquisition data */
    path = make_path(dset, "waveforms");

    /* The acquisition datatype */
    datatype = get_hdf5type_waveform();

    status = read_element(dset, path, &hdf5wav, datatype, index);
    memcpy(&wav->head, &hdf5wav.head, sizeof(ISMRMRD_WaveformHeader));
    ismrmrd_make_consistent_waveform(wav);
    memcpy(wav->data, hdf5wav.data.p, ismrmrd_size_of_waveform_data(wav));

    /* clean up */
    free(path);
    free(hdf5wav.data.p);

    status = H5Tclose(datatype);
    if (status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to close datatype.");
    }

    return ISMRMRD_NOERROR;
}

uint32_t ismrmrd_get_number_of_waveforms(const ISMRMRD_Dataset *dset) {
    char *path;
    uint32_t numacq;

    if (dset==NULL) {
        ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Pointer should not be NULL.");
        return 0;
    }
    /* The path to the acqusition data */
    path = make_path(dset, "waveforms");
    numacq = get_number_of_elements(dset, path);
    free(path);
    return numacq;
}

int ismrmrd_append_array(const ISMRMRD_Dataset *dset, const char *varname, const ISMRMRD_NDArray *arr) {
    int status;
    hid_t datatype;
    uint16_t ndim;
    size_t *dims;
    int n;
    char *path;

    if (dset==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
    }
    if (varname==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Varname should not be NULL.");
    }
    if (arr==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Array pointer should not be NULL.");
    }

    /* The group for this set */
    /* /groupname/varname */
    path = make_path(dset, varname);

    /* Handle the data */
    datatype = get_hdf5type_ndarray(arr->data_type);
    ndim = arr->ndim;
    dims = (size_t *) malloc(ndim*sizeof(size_t));
    /* permute the dimensions in the hdf5 file */
    for (n=0; n<ndim; n++) {
        dims[ndim-n-1] = arr->dims[n];
    }
    status = append_element(dset, path, arr->data, datatype, ndim, dims);
    if (status != ISMRMRD_NOERROR) {
        free(dims);
        return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to append array.");
    }

    /* Final cleanup */
    free(dims);
    status = H5Tclose(datatype);
    if (status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to close datatype.");
    }
    free(path);

    return ISMRMRD_NOERROR;
}

uint32_t ismrmrd_get_number_of_arrays(const ISMRMRD_Dataset *dset, const char *varname) {
    char *path;
    uint32_t numarrays;

    if (dset==NULL) {
        ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
        return 0;
    }
    if (varname==NULL) {
        ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Varname should not be NULL.");
        return 0;
    }

    /* The group for this set */
    /* /groupname/varname */
    path = make_path(dset, varname);
    numarrays = get_number_of_elements(dset, path);
    free(path);
    return numarrays;
}

int ismrmrd_read_array(const ISMRMRD_Dataset *dset, const char *varname,
        const uint32_t index, ISMRMRD_NDArray *arr) {    
    int status;
    hid_t datatype;
    char *path;

    if (dset==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
    }
    if (varname==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Varname should not be NULL.");
    }
    if (arr==NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Array pointer should not be NULL.");
    }

    /* The group for this set */
    /* /groupname/varname */
    path = make_path(dset, varname);

    /* get the array properties */
    get_array_properties(dset, path, &arr->ndim, arr->dims, &arr->data_type);
    datatype = get_hdf5type_ndarray(arr->data_type);

    /* allocate the memory */
    ismrmrd_make_consistent_ndarray(arr);

    /* read the data */
    status = read_element(dset, path, arr->data, datatype, index);
    if (status != ISMRMRD_NOERROR) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_FILEERROR, "Failed to append array.");
    }

    /* Final cleanup */
    status = H5Tclose(datatype);
    if (status < 0) {
        H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, walk_hdf5_errors, NULL);
        return ISMRMRD_PUSH_ERR(ISMRMRD_HDF5ERROR, "Failed to close datatype.");
    }
    free(path);

    return ISMRMRD_NOERROR;
}


#ifdef __cplusplus
} /* extern "C" */
} /* ISMRMRD namespace */
#endif
