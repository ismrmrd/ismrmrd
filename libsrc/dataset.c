/* Language and Cross platform section for defining types */
#ifdef __cplusplus
#include <cstring>
#include <cstdlib>
#include <cstdio>
#else
/* C99 compiler */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#endif /* __cplusplus */

#include <hdf5.h>
#include "ismrmrd/dataset.h"

#ifdef __cplusplus
namespace ISMRMRD {
extern "C" {
#endif
    
/******************************/
/* Private (Static) Functions */
/******************************/
static bool link_exists(const ISMRMRD_Dataset *dset, const char *link_path) {
    if (dset) {
        
        htri_t val = H5Lexists(dset->fileid, link_path, H5P_DEFAULT);
        
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
    else {
        return false;
    }
}

static int create_link(const ISMRMRD_Dataset *dset, const char *link_path) {
    hid_t lcpl_id, gid;
    if (dset) {
        if (link_exists(dset, link_path)) {
            return ISMRMRD_NOERROR;
        }
        else {
            lcpl_id = H5Pcreate(H5P_LINK_CREATE);
            H5Pset_create_intermediate_group(lcpl_id, 1);
            gid = H5Gcreate2(dset->fileid, link_path, lcpl_id, H5P_DEFAULT, H5P_DEFAULT);
            H5Gclose(gid);
            H5Pclose(lcpl_id);
            // TODO can this thing ever return an error?
            return ISMRMRD_NOERROR;
        }
    }
    else {
        return ISMRMRD_RUNTIMEERROR;
    }
}

static char * make_path(const ISMRMRD_Dataset *dset, const char * var) {
    if (dset) {
        if (var) {
            size_t len = strlen(dset->groupname) + strlen(var) + 2;
            char *path = (char *) malloc(len);
            if (path == NULL) {
                ISMRMRD_THROW(ISMRMRD_MEMORYERROR, "Failed to malloc path");
                return NULL;
            }
            memset(path, 0, len);
            strcat(path, dset->groupname);
            strcat(path, "/");
            strcat(path, var);
            return path;
        }
        else {
            return NULL;
        }
    }
    else {
        return NULL;
    }
}

static char * append_to_path(const ISMRMRD_Dataset *dset, const char * path, const char * var) {
    if (dset) {
        if (path) {
            if (var) {
                size_t len = strlen(path) + strlen(var) + 2;
                char *newpath = (char *) malloc(len);
                if (newpath == NULL) {
                    ISMRMRD_THROW(ISMRMRD_MEMORYERROR, "Failed to realloc newpath");
                    return NULL;
                }
                memset(newpath, 0, len);
                strcat(newpath, path);
                strcat(newpath, "/");
                strcat(newpath, var);
                return newpath;
            }
            else {
                return NULL;
            }
        }
        else {
            return NULL;
        }
    }
    else {
        return NULL;
    }
}

static int delete_var(const ISMRMRD_Dataset *dset, const char *var) {
    if (dset) {
        int status = ISMRMRD_NOERROR;
        herr_t h5status;
        char *path = make_path(dset, var);
        if (link_exists(dset, path)) {
            h5status = H5Ldelete(dset->fileid, path, H5P_DEFAULT);
            if (h5status < 0) {
                status = ISMRMRD_FILEERROR;
                ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed to delete H5 path");
            }
        }
        free(path);
        return status;
    }
    else {
        return ISMRMRD_RUNTIMEERROR;
    }
}

/*********************************************/
/* Private (Static) Functions for HDF5 Types */
/*********************************************/
typedef struct HDF5_Acquisition
{
    ISMRMRD_AcquisitionHeader head;
    hvl_t traj;
    hvl_t data;
} HDF5_Acquisition;

typedef struct HDF5_NDArrayBlob
{
    uint16_t version;
    uint16_t data_type;
    uint16_t ndim;
    uint16_t dims[ISMRMRD_NDARRAY_MAXDIM];
    hvl_t data;
} HDF5_NDArrayBlob;

static hid_t get_hdf5type_char(void) {
    hid_t datatype = H5Tcopy(H5T_NATIVE_CHAR);
    return datatype;
}

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

static hid_t get_hdf5type_complexfloat(void) {
    hid_t datatype;
    herr_t h5status;
    datatype = H5Tcreate(H5T_COMPOUND, sizeof(complex_float_t));
    h5status = H5Tinsert(datatype, "real", 0, H5T_NATIVE_FLOAT);
    h5status = H5Tinsert(datatype, "imag", sizeof(float), H5T_NATIVE_FLOAT);
    if (h5status < 0) {
        ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed get complex float data type");
    }
    return datatype;
}
    
static hid_t get_hdf5type_complexdouble(void) {
    hid_t datatype;
    herr_t h5status;
    datatype = H5Tcreate(H5T_COMPOUND, sizeof(complex_double_t));
    h5status = H5Tinsert(datatype, "real", 0, H5T_NATIVE_DOUBLE);
    h5status = H5Tinsert(datatype, "imag", sizeof(double), H5T_NATIVE_DOUBLE);
    if (h5status < 0) {
        ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed get complex double data type");
    }
    return datatype;
}

static hid_t get_hdf5type_xmlheader(void) {
    hid_t datatype = H5Tcopy(H5T_C_S1);
    herr_t h5status = H5Tset_size(datatype, H5T_VARIABLE);
    if (h5status < 0) {
        ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed get XML header data type");
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
        ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed get endoding data type");
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
    h5status = H5Tinsert(datatype, " measurement_uid", HOFFSET(ISMRMRD_AcquisitionHeader,  measurement_uid), H5T_NATIVE_UINT32);
    h5status = H5Tinsert(datatype, "scan_counter", HOFFSET(ISMRMRD_AcquisitionHeader, scan_counter), H5T_NATIVE_UINT32);
    h5status = H5Tinsert(datatype, "acquisition_time_stamp", HOFFSET(ISMRMRD_AcquisitionHeader, acquisition_time_stamp), H5T_NATIVE_UINT32);
    
    arraydims[0] = ISMRMRD_PHYS_STAMPS;
    vartype = H5Tarray_create2(H5T_NATIVE_UINT32, 1, arraydims);
    h5status = H5Tinsert(datatype, "physiology_time_stamp", HOFFSET(ISMRMRD_AcquisitionHeader, physiology_time_stamp), vartype);
    
    h5status = H5Tinsert(datatype, "number_of_samples", HOFFSET(ISMRMRD_AcquisitionHeader, number_of_samples), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "available_channels", HOFFSET(ISMRMRD_AcquisitionHeader, available_channels), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "active_channels", HOFFSET(ISMRMRD_AcquisitionHeader, active_channels), H5T_NATIVE_UINT16);
    
    arraydims[0] = ISMRMRD_CHANNEL_MASKS;
    vartype = H5Tarray_create2(H5T_NATIVE_UINT64, 1, arraydims);
    h5status = H5Tinsert(datatype, "channel_mask", HOFFSET(ISMRMRD_AcquisitionHeader, channel_mask), vartype);
    
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
    
    vartype = get_hdf5type_encoding();
    h5status = H5Tinsert(datatype, "idx", HOFFSET(ISMRMRD_AcquisitionHeader, idx), vartype);
    
    arraydims[0] = ISMRMRD_USER_INTS;
    vartype = H5Tarray_create2(H5T_NATIVE_INT32, 1, arraydims);
    h5status = H5Tinsert(datatype, "user_int", HOFFSET(ISMRMRD_AcquisitionHeader, user_int), vartype);
    
    arraydims[0] = ISMRMRD_USER_FLOATS;
    vartype = H5Tarray_create2(H5T_NATIVE_FLOAT, 1, arraydims);
    h5status = H5Tinsert(datatype, "user_float", HOFFSET(ISMRMRD_AcquisitionHeader, user_float), vartype);
    
    /* Clean up */
    H5Tclose(vartype);
    
    if (h5status < 0) {
        ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed get acquisitionheader data type");
    }
    
    return datatype;   
}

static hid_t get_hdf5type_acquisition(void) {
    hid_t datatype, vartype, vlvartype;
    herr_t h5status;
    
    datatype = H5Tcreate(H5T_COMPOUND, sizeof(HDF5_Acquisition));
    vartype = get_hdf5type_acquisitionheader();
    h5status = H5Tinsert(datatype, "head", HOFFSET(HDF5_Acquisition, head), vartype);
    vartype =  get_hdf5type_float();
    vlvartype = H5Tvlen_create(vartype);
    h5status = H5Tinsert(datatype, "traj", HOFFSET(HDF5_Acquisition, traj), vlvartype);
    vartype = get_hdf5type_complexfloat();
    vlvartype = H5Tvlen_create(vartype);
    h5status = H5Tinsert(datatype, "data", HOFFSET(HDF5_Acquisition, data), vlvartype);
    
    H5Tclose(vartype);
    H5Tclose(vlvartype);
    
    if (h5status < 0) {
        ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed get acquisition data type");
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
    h5status = H5Tinsert(datatype, " measurement_uid", HOFFSET(ISMRMRD_ImageHeader,  measurement_uid), H5T_NATIVE_UINT32);
    arraydims[0] = 3;
    vartype = H5Tarray_create2(H5T_NATIVE_UINT16, 1, arraydims);
    h5status = H5Tinsert(datatype, "matrix_size", HOFFSET(ISMRMRD_ImageHeader, matrix_size), vartype);
    vartype = H5Tarray_create2(H5T_NATIVE_FLOAT, 1, arraydims);
    h5status = H5Tinsert(datatype, "field_of_view", HOFFSET(ISMRMRD_ImageHeader, field_of_view), vartype);
    h5status = H5Tinsert(datatype, "channels", HOFFSET(ISMRMRD_ImageHeader, channels), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "position", HOFFSET(ISMRMRD_ImageHeader, position), vartype);
    h5status = H5Tinsert(datatype, "read_dir", HOFFSET(ISMRMRD_ImageHeader, read_dir), vartype);
    h5status = H5Tinsert(datatype, "phase_dir", HOFFSET(ISMRMRD_ImageHeader, phase_dir), vartype);
    h5status = H5Tinsert(datatype, "slice_dir", HOFFSET(ISMRMRD_ImageHeader, slice_dir), vartype);
    h5status = H5Tinsert(datatype, "patient_table_position", HOFFSET(ISMRMRD_ImageHeader, patient_table_position), vartype);
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
    h5status = H5Tinsert(datatype, "image_type", HOFFSET(ISMRMRD_ImageHeader, image_type), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "image_index", HOFFSET(ISMRMRD_ImageHeader, image_index), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "image_series_index", HOFFSET(ISMRMRD_ImageHeader, image_series_index), H5T_NATIVE_UINT16);
    
    arraydims[0] = ISMRMRD_USER_INTS;
    vartype = H5Tarray_create2(H5T_NATIVE_INT32, 1, arraydims);
    h5status = H5Tinsert(datatype, "user_int", HOFFSET(ISMRMRD_ImageHeader, user_int), vartype);
    arraydims[0] = ISMRMRD_USER_FLOATS;
    vartype = H5Tarray_create2(H5T_NATIVE_FLOAT, 1, arraydims);
    h5status = H5Tinsert(datatype, "user_float", HOFFSET(ISMRMRD_ImageHeader, user_float), vartype);
    h5status = H5Tinsert(datatype, "attribute_string_len", HOFFSET(ISMRMRD_ImageHeader, attribute_string_len), H5T_NATIVE_UINT32);
    
    /* Clean up */
    H5Tclose(vartype);

    if (h5status < 0) {
        ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed get imageheader data type");
    }
    
    return datatype;   
}

static hid_t get_hdf5type_image_attribute_string(void) {
    hid_t datatype = H5Tcopy(H5T_C_S1);
    herr_t h5status = H5Tset_size(datatype, H5T_VARIABLE);
    if (h5status < 0) {
        ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed get image attribute string data type");
    }
    return datatype;
}
    
static hid_t get_hdf5type_ndarrayblob(void) {
    hid_t datatype, vartype, vlvartype;
    herr_t h5status;
    hsize_t arraydims[1];
    
    datatype = H5Tcreate(H5T_COMPOUND, sizeof(HDF5_NDArrayBlob));
    h5status = H5Tinsert(datatype, "version", HOFFSET(HDF5_NDArrayBlob, version), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "data_type", HOFFSET(HDF5_NDArrayBlob, data_type), H5T_NATIVE_UINT16);
    arraydims[0] = ISMRMRD_NDARRAY_MAXDIM;
    vartype = H5Tarray_create2(H5T_NATIVE_UINT16, 1, arraydims);
    h5status = H5Tinsert(datatype, "dims", HOFFSET(HDF5_NDArrayBlob, dims), vartype);
    vartype = get_hdf5type_char();
    vlvartype = H5Tvlen_create(vartype);
    h5status = H5Tinsert(datatype, "data", HOFFSET(HDF5_NDArrayBlob, data), vlvartype);
    
    H5Tclose(vartype);
    H5Tclose(vlvartype);
    
    if (h5status < 0) {
        ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed get NDArrayBlob data type");
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
            ISMRMRD_THROW(ISMRMRD_TYPEERROR, "Failed to get HDF5 data type.");
    }
    return hdfdatatype;
}
    
uint32_t get_number_of_elements(const ISMRMRD_Dataset *dset, const char * path)
{
    if (dset) {
        herr_t h5status;
        uint32_t num;
        
        if (link_exists(dset, path)) {
            hid_t dataset, dataspace;
            hsize_t rank, *dims, *maxdims;
            dataset = H5Dopen2(dset->fileid, path, H5P_DEFAULT);
            dataspace = H5Dget_space(dataset);
            rank = H5Sget_simple_extent_ndims(dataspace);
            dims = (hsize_t *) malloc(rank*sizeof(hsize_t));
            maxdims = (hsize_t *) malloc(rank*sizeof(hsize_t));
            h5status = H5Sget_simple_extent_dims(dataspace, dims, maxdims);
            num = dims[0];
            free(dims);
            free(maxdims);
            h5status = H5Sclose(dataspace);
            h5status= H5Dclose(dataset);
            if (h5status < 0) {
                ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed to get number of elements in vector.");
            }
        }
        else {
            /* none */
            num = 0;
        }
        
        return num;
    }
    else {
        return 0;
    }
}

int append_element(const ISMRMRD_Dataset * dset, const char * path, void * elem, const hid_t datatype, const uint16_t ndim, const uint16_t *dims)
{
    if (dset) {
        hid_t dataset, dataspace, props, filespace, memspace;
        herr_t h5status;
        int n;
        
        /* Check the path, extend or create if needed, and select the last block */
        if (link_exists(dset, path)) {
            hsize_t rank, *hdfdims, *maxdims, *offset, *ext_dims;
            /* open */
            dataset = H5Dopen2(dset->fileid, path, H5P_DEFAULT);
            /* TODO check that the header dataset's datatype is correct */
            dataspace = H5Dget_space(dataset);
            rank = H5Sget_simple_extent_ndims(dataspace);
            if (rank != (ndim+1)) {
                ISMRMRD_THROW(ISMRMRD_FILEERROR, "Dimensions are incorrect.");
                return ISMRMRD_FILEERROR;
            }
            hdfdims =(hsize_t *) malloc(rank*sizeof(hsize_t));
            maxdims =(hsize_t *) malloc(rank*sizeof(hsize_t));
            h5status = H5Sget_simple_extent_dims(dataspace, hdfdims, maxdims);
            for (n = 0; n<ndim; n++) {
                if (dims[n] != hdfdims[n+1]) {
                    ISMRMRD_THROW(ISMRMRD_FILEERROR, "Dimensions are incorrect.");
                    return ISMRMRD_FILEERROR;
                }
            }
            /* extend it by one */
            hdfdims[0] += 1;
            h5status = H5Dset_extent(dataset, hdfdims);
            /* Select the last block */
            offset = (hsize_t *) malloc(rank*sizeof(hsize_t));
            ext_dims = (hsize_t *) malloc(rank*sizeof(hsize_t));
            offset[0] = hdfdims[0]-1;
            ext_dims[0] = 1;
            for (n = 0; n<ndim; n++) {
                offset[n+1] = 0;
                ext_dims[n+1] = dims[n];
            }
            filespace = H5Dget_space(dataset);
            h5status  = H5Sselect_hyperslab (filespace, H5S_SELECT_SET, offset, NULL, ext_dims, NULL);
            memspace = H5Screate_simple(rank, ext_dims, NULL);
            free(hdfdims);
            free(maxdims);
            free(offset);
            free(ext_dims);
        }
        else {
            int rank;
            hsize_t *hdfdims, *maxdims, *offset, *ext_dims, *chunk_dims;
            /* create a new dataset */
            rank = ndim+1;
            hdfdims = (hsize_t *) malloc(rank*sizeof(hsize_t));
            maxdims = (hsize_t *) malloc(rank*sizeof(hsize_t));
            offset = (hsize_t *) malloc(rank*sizeof(hsize_t));
            ext_dims = (hsize_t *) malloc(rank*sizeof(hsize_t));
            chunk_dims = (hsize_t *) malloc(rank*sizeof(hsize_t));
            hdfdims[0] = 1;
            maxdims[0] = H5S_UNLIMITED;
            offset[0] = 1;
            ext_dims[0] = 1;
            chunk_dims[0] = 1;
            for (n = 0; n<ndim; n++) {
                hdfdims[n+1] = dims[n];
                maxdims[n+1] = dims[n];
                offset[n+1] = 0;
                ext_dims[n+1] = dims[n];
                chunk_dims[n+1] = dims[n];
            }
            dataspace = H5Screate_simple(rank, hdfdims, maxdims);
            props = H5Pcreate(H5P_DATASET_CREATE);
            /* enable chunking so that the dataset is extensible */
            h5status = H5Pset_chunk (props, rank, chunk_dims);
            /* create */
            dataset = H5Dcreate2(dset->fileid, path, datatype, dataspace, H5P_DEFAULT, props,  H5P_DEFAULT);
            h5status = H5Pclose(props);
            /* Select the last block */
            offset[0] = hdfdims[0]-1;
            filespace = H5Dget_space(dataset);
            h5status  = H5Sselect_hyperslab (filespace, H5S_SELECT_SET, offset, NULL, ext_dims, NULL);
            memspace = H5Screate_simple(rank, ext_dims, NULL);
            free(hdfdims);
            free(maxdims);
            free(offset);
            free(ext_dims);
            free(chunk_dims);
        }

        /* Write it */
        /* since this is a 1 element array we can just pass the pointer to the header */
        h5status = H5Dwrite(dataset, datatype, memspace, filespace, H5P_DEFAULT, elem);
        
        /* Clean up */
        h5status = H5Sclose(dataspace);
        h5status = H5Sclose(filespace);
        h5status = H5Sclose(memspace);
        h5status = H5Dclose(dataset);

        if (h5status < 0) {
            ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed to get number of acquisitions.");
            return ISMRMRD_FILEERROR;
        }
        
        return ISMRMRD_NOERROR;
    }
    else {
        return ISMRMRD_RUNTIMEERROR;
    }
}

/********************/
/* Public functions */
/********************/
int ismrmrd_init_dataset(ISMRMRD_Dataset *dset, const char *filename, const char *groupname) {
    if (dset) {
        dset->filename = (char *) malloc(strlen(filename) + 1);
        if (dset->filename == NULL) {
            ISMRMRD_THROW(ISMRMRD_MEMORYERROR, "Failed to malloc dataset groupname");
            return ISMRMRD_MEMORYERROR;
        }
        strcpy(dset->filename, filename);
        
        dset->groupname = (char *) malloc(strlen(groupname) + 1);
        if (dset->groupname == NULL) {
            ISMRMRD_THROW(ISMRMRD_MEMORYERROR, "Failed to malloc dataset groupname");
            return ISMRMRD_MEMORYERROR;
        }
        strcpy(dset->groupname, groupname);
        dset->fileid = 0;
        return ISMRMRD_NOERROR;
    }
    else {
        return ISMRMRD_RUNTIMEERROR;
    }
}

int ismrmrd_open_dataset(ISMRMRD_Dataset *dset, const bool create_if_needed) {
    // TODO add a mode for clobbering the dataset if it exists.
    if (dset) {
        hid_t fileid;
        
        /* Try opening the file */
        /* Note the is_hdf5 function doesn't work well when trying to open multiple files */
        /* Suppress errors with the try macro. */
        H5E_BEGIN_TRY {
            fileid = H5Fopen(dset->filename, H5F_ACC_RDWR, H5P_DEFAULT);
        } H5E_END_TRY
    
        if (fileid > 0) {
            dset->fileid = fileid;
        }
        else {
            /* Some sort of error opening the file */
            /* Maybe it doesn't exist? */
            if (create_if_needed == false) {
            ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed to open file.");
            return ISMRMRD_FILEERROR;
        }
        else {
            /* Try creating a new file using the default properties. */
            /* this will be readwrite */
            H5E_BEGIN_TRY {
                fileid = H5Fcreate(dset->filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
            } H5E_END_TRY
            if (fileid > 0) {
                dset->fileid = fileid;
            }
            else {
            /* Error opening the file */
                ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed to open file.");
                return ISMRMRD_FILEERROR;
            }
        }
        }
        
        /* Open the existing dataset */
        /* insure that /groupname exists */
        create_link(dset, dset->groupname);
        
        return ISMRMRD_NOERROR;
    }
    else {
        return ISMRMRD_RUNTIMEERROR;
    }
}

int ismrmrd_close_dataset(ISMRMRD_Dataset *dset) {
    if (dset) {
        herr_t h5status;
        
        /* Check for a valid fileid before trying to close the file */
        if (dset->fileid > 0) {
            h5status = H5Fclose (dset->fileid);
            dset->fileid = 0;
            if (h5status < 0) {
                ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed to close dataset.");
                return ISMRMRD_FILEERROR;
            }
        }
        
        return ISMRMRD_NOERROR;
    }
    else {
        return ISMRMRD_RUNTIMEERROR;
    }
}

int ismrmrd_write_header(const ISMRMRD_Dataset *dset, const char *xmlstring) {
    hid_t dataset, dataspace, datatype, props;
    hsize_t dims[] = {1};
    herr_t h5status;
    void *buff[1];
    char * path;

    if (dset==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
        return ISMRMRD_RUNTIMEERROR;
    }

    if (xmlstring==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "xmlstring should not be NULL.");
        return ISMRMRD_RUNTIMEERROR;
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
        
    /* Write it out */
    /* We have to wrap the xmlstring in an array */
    buff[0] = (void *) xmlstring;  /* safe to get rid of const the type */
    h5status = H5Dwrite(dataset, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, buff);
    
    /* Clean up */
    h5status = H5Pclose(props);
    h5status = H5Tclose(datatype);
    h5status = H5Sclose(dataspace);
    h5status = H5Dclose(dataset);
    
    if (h5status < 0) {
        ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed to write header.");
        return ISMRMRD_FILEERROR;
    }
        
    return ISMRMRD_NOERROR;
}

char * ismrmrd_read_header(const ISMRMRD_Dataset *dset) {
    hid_t dataset, datatype;
    herr_t h5status;
    char * xmlstring, *path;
        
    if (dset==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Pointer should not be NULL.");
        return NULL;
    }

    /* The path to the xml header */
    path = make_path(dset, "xml");
        
    if (link_exists(dset, path)) {
        void *buff[1];
        dataset = H5Dopen2(dset->fileid, path, H5P_DEFAULT);
        datatype = get_hdf5type_xmlheader();
        /* Read it into a 1D buffer*/
        h5status = H5Dread(dataset, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, buff);
        
        /* Unpack */
        xmlstring = (char *) malloc(strlen(buff[0])+1);
        if (xmlstring == NULL) {
            ISMRMRD_THROW(ISMRMRD_MEMORYERROR, "Failed to malloc xmlstring");
        } else {
            memcpy(xmlstring, buff[0], strlen(buff[0])+1);
        }
        
        /* Clean up */
        h5status = H5Tclose(datatype);
        h5status = H5Dclose(dataset);
        free(path);
        
        if (h5status < 0) {
            ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed to read header.");
            return NULL;
        }
            
        return xmlstring;
    }
    else {
        // No XML String found
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "No XML Header found.");
        free(path);
        return NULL;
    }
}

uint32_t ismrmrd_get_number_of_acquisitions(const ISMRMRD_Dataset *dset) {
    char *path;
    uint32_t numacq;
    
    if (dset==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Pointer should not be NULL.");
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
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
        return ISMRMRD_RUNTIMEERROR;
    }
    if (acq==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Acquisition pointer should not be NULL.");
        return ISMRMRD_RUNTIMEERROR;
    }
    
    /* The path to the acqusition data */    
    path = make_path(dset, "data");
            
    /* The acquisition datatype */
    datatype = get_hdf5type_acquisition();
    
    /* Create the HDF5 version of the acquisition */
    hdf5acq[0].head = acq->head;
    hdf5acq[0].traj.len = acq->head.number_of_samples * acq->head.trajectory_dimensions;
    hdf5acq[0].traj.p = acq->traj;
    hdf5acq[0].data.len = acq->head.number_of_samples * acq->head.active_channels;
    hdf5acq[0].data.p = acq->data;
    
    /* Write it */
    status = append_element(dset, path, hdf5acq, datatype, 0, NULL);
    if (status != ISMRMRD_NOERROR) {
        ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed to append acquisition.");
        return ISMRMRD_FILEERROR;
    }

    /* Clean up */
    H5Tclose(datatype);
    free(path);
    
    return ISMRMRD_NOERROR;
}

int ismrmrd_read_acquisition(const ISMRMRD_Dataset *dset, uint32_t index, ISMRMRD_Acquisition *acq)
{
    hid_t dataset, datatype, filespace, memspace;
    hsize_t dims[1];
    hsize_t offset[1];
    hsize_t dimsr[1] = {1};
    herr_t h5status;
    HDF5_Acquisition hdf5acq;
    char *path;        

    if (dset==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
        return ISMRMRD_RUNTIMEERROR;
    }
    if (acq==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Acquisition pointer should not be NULL.");
        return ISMRMRD_RUNTIMEERROR;
    }

    /* The path to the acqusition data */    
    path = make_path(dset, "data");
            
    /* Check the path, extend or create if needed */
    if (link_exists(dset, path)) {
        /* open */
        dataset = H5Dopen2(dset->fileid, path, H5P_DEFAULT);
        
        /* The acquisition datatype */
        datatype = get_hdf5type_acquisition();
        /* TODO check that the dataset's datatype is correct */
                
        filespace = H5Dget_space(dataset);
        h5status = H5Sget_simple_extent_dims(filespace, dims, NULL);
                
        if (index < dims[0]) {
            offset[0] = index;
            h5status = H5Sselect_hyperslab (filespace, H5S_SELECT_SET, offset, NULL, dimsr, NULL);
            memspace = H5Screate_simple(1, dimsr, NULL);
            h5status = H5Dread(dataset, datatype, memspace, filespace, H5P_DEFAULT, (void *) &hdf5acq);
            memcpy(&acq->head, &hdf5acq.head, sizeof(ISMRMRD_AcquisitionHeader));
            ismrmrd_make_consistent_acquisition(acq);
            memcpy(acq->traj, hdf5acq.traj.p, ismrmrd_size_of_acquisition_traj(acq));
            memcpy(acq->data, hdf5acq.data.p, ismrmrd_size_of_acquisition_data(acq));
                    
            /* clean up */
            free(hdf5acq.traj.p);
            free(hdf5acq.data.p);
            h5status = H5Tclose(datatype);
            h5status = H5Sclose(filespace);
            h5status = H5Sclose(memspace);
            h5status = H5Dclose(dataset);
            if (h5status < 0) {
                ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed to read acquisition.");
                return ISMRMRD_FILEERROR;
            }
        }
        else {
            /* index out of range */
            ISMRMRD_THROW(ISMRMRD_FILEERROR, "Acquisition index out of range.");
            return ISMRMRD_FILEERROR;
        }
    }
    else {
        /* No data */
        ISMRMRD_THROW(ISMRMRD_FILEERROR, "Data not found.");
        return ISMRMRD_FILEERROR;
    }
    
    return ISMRMRD_NOERROR;
}

int ismrmrd_append_image(const ISMRMRD_Dataset *dset, const char *varname,
                         const int block_mode, const ISMRMRD_Image *im) {
    int status;
    hid_t datatype;
    char *path, *headerpath, *attrpath, *datapath;
    uint16_t dims[4];

    if (dset==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
        return ISMRMRD_RUNTIMEERROR;
    }
    if (varname==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Varname should not be NULL.");
        return ISMRMRD_RUNTIMEERROR;
    }
    if (im==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Image pointer should not be NULL.");
        return ISMRMRD_RUNTIMEERROR;
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
        ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed to append image header.");
        return ISMRMRD_FILEERROR;
    }
    free(headerpath);
            
    /* Handle the attribute string */
    attrpath = append_to_path(dset, path, "attributes");
    datatype = get_hdf5type_image_attribute_string();
    status = append_element(dset, attrpath, (void *) &im->attribute_string, datatype, 0, NULL);
    if (status != ISMRMRD_NOERROR) {
        ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed to append image attribute string.");
        return ISMRMRD_FILEERROR;
    }
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
        ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed to append image data.");
        return ISMRMRD_FILEERROR;
    }
    free(datapath);
            
    /* Final cleanup */
    H5Tclose(datatype);
    free(path);
    
    return ISMRMRD_NOERROR;
};

uint32_t ismrmrd_get_number_of_images(const ISMRMRD_Dataset *dset, const char *varname)
{
    char *path, *headerpath;
    uint32_t numimages;
    
    if (dset==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
        return 0;
    }
    if (varname==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Varname should not be NULL.");
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

int ismrmrd_append_array(const ISMRMRD_Dataset *dset, const char *varname,
                         const int block_mode, const ISMRMRD_NDArray *arr) {
    int status;
    hid_t datatype;
    uint16_t ndim, *dims;
    int n;
    char *path;
    
    if (dset==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
        return 0;
    }
    if (varname==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Varname should not be NULL.");
        return 0;
    }
    if (arr==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Array pointer should not be NULL.");
        return 0;
    }
    
    /* The group for this set */
    /* /groupname/varname */
    path = make_path(dset, varname);
            
    /* Handle the data */
    datatype = get_hdf5type_ndarray(arr->data_type);
    ndim = arr->ndim;
    dims = (uint16_t *) malloc(ndim*sizeof(uint16_t));
    /* permute the dimensions in the hdf5 file */
    for (n=0; n<ndim; n++) {
        dims[ndim-n-1] = arr->dims[n];
    }
    status = append_element(dset, path, arr->data, datatype, ndim, dims);
    if (status != ISMRMRD_NOERROR) {
        ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed to append array.");
        return ISMRMRD_FILEERROR;
    }
    
    /* Final cleanup */
    free(dims);
    H5Tclose(datatype);
    free(path);
    
    return ISMRMRD_NOERROR;
}

uint32_t ismrmrd_get_number_of_arrays(const ISMRMRD_Dataset *dset, const char *varname) {
    char *path;
    uint32_t numarrays;
    
    if (dset==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
        return 0;
    }
    if (varname==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Varname should not be NULL.");
        return 0;
    }

    /* The group for this set */
    /* /groupname/varname */
    path = make_path(dset, varname);
    numarrays = get_number_of_elements(dset, path);
    free(path);
    return numarrays;
}

/*****************************/
/* TODO Implement these ones */  
/*****************************/
int ismrmrd_read_image(const ISMRMRD_Dataset *dset, const char *varname,
                       const uint32_t index, ISMRMRD_Image *im) {
    if (dset==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
        return 0;
    }
    if (varname==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Varname should not be NULL.");
        return 0;
    }
    if (im==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Image should not be NULL.");
        return 0;
    }

    return ISMRMRD_NOERROR;
}

int ismrmrd_read_array(const ISMRMRD_Dataset *dset, const char *varname,
                       const uint32_t index, ISMRMRD_NDArray *arr) {
    if (dset==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Dataset pointer should not be NULL.");
        return 0;
    }
    if (varname==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Varname should not be NULL.");
        return 0;
    }
    if (arr==NULL) {
        ISMRMRD_THROW(ISMRMRD_RUNTIMEERROR, "Array pointer should not be NULL.");
        return 0;
    }

    return ISMRMRD_NOERROR;
}


#ifdef __cplusplus
} /* extern "C" */
} /* ISMRMRD namespace */
#endif
