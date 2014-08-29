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
#include "ismrmrd_dataset.h"

#ifdef __cplusplus
namespace ISMRMRD {
extern "C" {
#endif
    
/******************************/
/* Private (Static) Functions */
/******************************/
static bool link_exists(const ISMRMRD_Dataset *dset, const char *link_path) {

    htri_t val;
    val = H5Lexists(dset->fileid, link_path, H5P_DEFAULT);

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
    if (link_exists(dset, link_path)) {
        return ISMRMRD_NOERROR;
    }
    else {
        hid_t lcpl_id;
        lcpl_id = H5Pcreate(H5P_LINK_CREATE);
        H5Pset_create_intermediate_group(lcpl_id, 1);
        hid_t gid = H5Gcreate2(dset->fileid, link_path, lcpl_id, H5P_DEFAULT, H5P_DEFAULT);
        H5Gclose(gid);
        // TODO does this thing return error
        return ISMRMRD_NOERROR;
    }
}

static char * make_path(const ISMRMRD_Dataset *dset, const char * var) {
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

static int delete_var(const ISMRMRD_Dataset *dset, const char *var) {
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

/*********************************************/
/* Private (Static) Functions for HDF5 Types */
/*********************************************/
typedef struct HDF5_Acquisition
{
    ISMRMRD_AcquisitionHeader head;
    hvl_t traj;
    hvl_t data;
} HDF5_Acquisition;
    
static hid_t get_hdf5type_xmlheader(void) {
    hid_t datatype = H5Tcopy(H5T_C_S1);
    herr_t h5status = H5Tset_size(datatype, H5T_VARIABLE);
    if (h5status < 0) {
        ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed get XML header data type");
    }
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

static hid_t get_hdf5type_encoding(void) {
    hid_t datatype;
    herr_t h5status;
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
    hsize_t arraydims[] = {ISMRMRD_USER_INTS};
    hid_t arraytype = H5Tarray_create(H5T_NATIVE_UINT16, 1, arraydims);
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
    h5status = H5Tinsert(datatype, "acquisition_time_stamp", HOFFSET(ISMRMRD_AcquisitionHeader, acquisition_time_stamp), H5T_NATIVE_UINT32);

    arraydims[0] = ISMRMRD_PHYS_STAMPS;
    vartype = H5Tarray_create(H5T_NATIVE_UINT32, 1, arraydims);
    h5status = H5Tinsert(datatype, "physiology_time_stamp", HOFFSET(ISMRMRD_AcquisitionHeader, physiology_time_stamp), vartype);
    
    h5status = H5Tinsert(datatype, "number_of_samples", HOFFSET(ISMRMRD_AcquisitionHeader, number_of_samples), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "available_channels", HOFFSET(ISMRMRD_AcquisitionHeader, available_channels), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "active_channels", HOFFSET(ISMRMRD_AcquisitionHeader, active_channels), H5T_NATIVE_UINT16);

    arraydims[0] = ISMRMRD_CHANNEL_MASKS;
    vartype = H5Tarray_create(H5T_NATIVE_UINT64, 1, arraydims);
    h5status = H5Tinsert(datatype, "channel_mask", HOFFSET(ISMRMRD_AcquisitionHeader, channel_mask), vartype);
    
    h5status = H5Tinsert(datatype, "discard_pre", HOFFSET(ISMRMRD_AcquisitionHeader, discard_pre), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "discard_post", HOFFSET(ISMRMRD_AcquisitionHeader, discard_post), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "center_sample", HOFFSET(ISMRMRD_AcquisitionHeader, center_sample), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "encoding_space_ref", HOFFSET(ISMRMRD_AcquisitionHeader, encoding_space_ref), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "trajectory_dimensions", HOFFSET(ISMRMRD_AcquisitionHeader, trajectory_dimensions), H5T_NATIVE_UINT16);
    h5status = H5Tinsert(datatype, "sample_time_us", HOFFSET(ISMRMRD_AcquisitionHeader, sample_time_us), H5T_NATIVE_FLOAT);

    arraydims[0] = 3;
    vartype = H5Tarray_create(H5T_NATIVE_FLOAT, 1, arraydims);
    h5status = H5Tinsert(datatype, "position", HOFFSET(ISMRMRD_AcquisitionHeader, position), vartype);
    h5status = H5Tinsert(datatype, "read_dir", HOFFSET(ISMRMRD_AcquisitionHeader, read_dir), vartype);
    h5status = H5Tinsert(datatype, "phase_dir", HOFFSET(ISMRMRD_AcquisitionHeader, phase_dir), vartype);
    h5status = H5Tinsert(datatype, "slice_dir", HOFFSET(ISMRMRD_AcquisitionHeader, slice_dir), vartype);
    h5status = H5Tinsert(datatype, "patient_table_position", HOFFSET(ISMRMRD_AcquisitionHeader, patient_table_position), vartype);

    vartype = get_hdf5type_encoding();
    h5status = H5Tinsert(datatype, "idx", HOFFSET(ISMRMRD_AcquisitionHeader, idx), vartype);
    
    arraydims[0] = ISMRMRD_USER_INTS;
    vartype = H5Tarray_create(H5T_NATIVE_INT32, 1, arraydims);
    h5status = H5Tinsert(datatype, "user_int", HOFFSET(ISMRMRD_AcquisitionHeader, user_int), vartype);
    
    arraydims[0] = ISMRMRD_USER_FLOATS;
    vartype = H5Tarray_create(H5T_NATIVE_FLOAT, 1, arraydims);
    h5status = H5Tinsert(datatype, "user_float", HOFFSET(ISMRMRD_AcquisitionHeader, user_float), vartype);

    /* Clean up */
    H5Tclose(vartype);

    if (h5status < 0) {
        ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed get acquisition data type");
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

/********************/
/* Public functions */
/********************/
int ismrmrd_init_dataset(ISMRMRD_Dataset *dset, const char *filename, const char *groupname) {

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

int ismrmrd_open_dataset(ISMRMRD_Dataset *dset, const bool create_if_needed) {
    // TODO add a mode for clobbering the dataset if it exists.

    hid_t       fileid;
    herr_t      h5status;

    /* Turn of HDF5 Errors */
    /* TODO, this is bad.  Maybe have it compile time dependent */
    /* or add to our error log */
    H5Eset_auto1(NULL, NULL);
    
    /* Check if the file exists and is an HDF5 File */
    h5status = H5Fis_hdf5(dset->filename);
    
    if (h5status > 0) {
        /* Positive value for exists and is HDF5 */
        /* Open it in readwrite mode */
        fileid = H5Fopen(dset->filename, H5F_ACC_RDWR, H5P_DEFAULT);
        if (fileid > 0) {
            dset->fileid = fileid;
        }
        else {
           /* Error opening the existing file */
           // TODO raise error
           return ISMRMRD_FILEERROR;
        }
    }
    else if (h5status == 0) {
       /* Zero value for exists and is NOT HDF5 */
       //TODO raise error
       return ISMRMRD_FILEERROR;
    }
    else {
        /* Negative value for does NOT exist or other error */
        if (create_if_needed == false) {
            ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed to open file.");
            return ISMRMRD_FILEERROR;
        }
        else {
            /* Create a new file using the default properties. */
            /* this will be readwrite */
            fileid = H5Fcreate(dset->filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
            if (fileid > 0) {
                dset->fileid = fileid;
            }
            else {
                /* Error creating file */
                // TODO raise error
                return ISMRMRD_FILEERROR;
            }
        }
    }

    /* Open the existing dataset */
    /* insure that /groupname exists */
    int val = create_link(dset, dset->groupname);

    return val;
};

int ismrmrd_close_dataset(ISMRMRD_Dataset *dset) {

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
};


int ismrmrd_write_header(const ISMRMRD_Dataset *dset, const char *xmlstring) {

    hid_t dataset, dataspace, datatype, props;
    hsize_t dims[] = {1};
    herr_t h5status;

    /* The path to the xml header */
    char *path = make_path(dset, "xml");

    /* Delete the old header if it exists */
    h5status = delete_var(dset, "xml");

    /* Create a new dataset for the xmlstring */
    /* i.e. create the memory type, data space, and data set */
    dataspace = H5Screate_simple(1, dims, NULL);
    datatype = get_hdf5type_xmlheader();
    props = H5Pcreate (H5P_DATASET_CREATE);
    dataset = H5Dcreate(dset->fileid, path, datatype, dataspace, H5P_DEFAULT, props,  H5P_DEFAULT);

    /* Write it out */
    /* We have to wrap the xmlstring in an array */
    void *buff[1];
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
};

char * ismrmrd_read_header(const ISMRMRD_Dataset *dset) {

    hid_t dataset, datatype;
    herr_t h5status;
    char * xmlstring;
    
    /* The path to the xml header */
    char *path = make_path(dset, "xml");

    if (link_exists(dset, path)) {
        dataset = H5Dopen(dset->fileid, path, H5P_DEFAULT);
        datatype = get_hdf5type_xmlheader();
        /* Read it into a 1D buffer*/
        void *buff[1];
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
        // TODO handle errors
        free(path);
        return NULL;
    }

};

unsigned long ismrmrd_get_number_of_acquisitions(const ISMRMRD_Dataset *dset) {

    hid_t dataset, dataspace;
    hsize_t dims[1], maxdims[1];
    herr_t h5status;
    unsigned long numacq;
    
    /* The path to the acqusition data */    
    char *path = make_path(dset, "data");

    if (link_exists(dset, path)) {
        dataset = H5Dopen(dset->fileid, path, H5P_DEFAULT);
        dataspace = H5Dget_space(dataset);
        h5status = H5Sget_simple_extent_dims(dataspace, dims, maxdims);
        numacq = dims[0];
        h5status = H5Sclose(dataspace);
        h5status= H5Dclose(dataset);
        if (h5status < 0) {
            ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed to get number of acquisitions.");
        }
    }
    else {
        /* none */
        numacq = 0;
    }

    free(path);

    return numacq;
};


int ismrmrd_append_acquisition(const ISMRMRD_Dataset *dset, const ISMRMRD_Acquisition *acq) {

    hid_t dataset, dataspace, datatype, props, filespace, memspace;
    hsize_t dims[1] = {1};
    hsize_t maxdims[1] = {H5S_UNLIMITED};
    hsize_t chunk_dims[] = {1};
    hsize_t ext_dims[] = {1};
    hsize_t offset[1] = {0};
    herr_t h5status;
    
    /* The path to the acqusition data */    
    char *path = make_path(dset, "data");

    /* The acquisition datatype */
    datatype = get_hdf5type_acquisition();
    
    /* Check the path, extend or create if needed */
    if (link_exists(dset, path)) {
        /* open */
        dataset = H5Dopen(dset->fileid, path, H5P_DEFAULT);
        /* TODO check that the dataset's datatype is correct */
        dataspace = H5Dget_space(dataset);
        h5status = H5Sget_simple_extent_dims(dataspace, dims, maxdims);
        /* extend it by one */
        dims[0] += 1;
        h5status = H5Dset_extent(dataset, dims);
    }
    else {
        /* create a new dataset for the data */
        dims[0] = 1;
        maxdims[0] = H5S_UNLIMITED;
        dataspace = H5Screate_simple(1, dims, maxdims);
        props = H5Pcreate(H5P_DATASET_CREATE);
        /* enable chunking so that the dataset is extensible */
        h5status = H5Pset_chunk (props, 1, chunk_dims);
        /* create */
        dataset = H5Dcreate(dset->fileid, path, datatype, dataspace, H5P_DEFAULT, props,  H5P_DEFAULT);
        h5status = H5Pclose(props);
    }

    /* Select the last block */
    offset[0] = dims[0]-1;
    filespace = H5Dget_space(dataset);
    h5status  = H5Sselect_hyperslab (filespace, H5S_SELECT_SET, offset, NULL, ext_dims, NULL);
    memspace = H5Screate_simple(1, ext_dims, NULL);
    
    /* Create the HDF5 version of the acquisition */
    HDF5_Acquisition hdf5acq[1];
    hdf5acq[0].head = acq->head;
    hdf5acq[0].traj.len = acq->head.number_of_samples * acq->head.trajectory_dimensions;
    hdf5acq[0].traj.p = acq->traj;
    hdf5acq[0].data.len = acq->head.number_of_samples * acq->head.active_channels;
    hdf5acq[0].data.p = acq->data;
    
    /* Write it */
    h5status = H5Dwrite (dataset, datatype, memspace, filespace, H5P_DEFAULT, hdf5acq);
    
    /* Clean up */
    h5status = H5Tclose(datatype);
    h5status = H5Sclose(dataspace);
    h5status = H5Sclose(filespace);
    h5status = H5Sclose(memspace);
    h5status = H5Dclose(dataset);
    free(path);
    
    if (h5status < 0) {
        ISMRMRD_THROW(ISMRMRD_FILEERROR, "Failed to append acquisitions.");
        return ISMRMRD_FILEERROR;
    }

    return ISMRMRD_NOERROR;
};

int ismrmrd_read_acquisition(const ISMRMRD_Dataset *dset, unsigned long index, ISMRMRD_Acquisition *acq) {
    hid_t dataset, datatype, filespace, memspace;
    hsize_t dims[1];
    hsize_t offset[1];
    hsize_t dimsr[1] = {1};
    herr_t h5status;
    HDF5_Acquisition hdf5acq;
    
    /* The path to the acqusition data */    
    char *path = make_path(dset, "data");

    /* Check the path, extend or create if needed */
    if (link_exists(dset, path)) {
        /* open */
        dataset = H5Dopen(dset->fileid, path, H5P_DEFAULT);
        
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
        /* TODO Throw error */
        return ISMRMRD_FILEERROR;
    }
    
    return ISMRMRD_NOERROR;
};

/*****************************/
/* TODO Implement these ones */  
/*****************************/

int ismrmrd_append_image(const ISMRMRD_Dataset *dset, const char *varname,
                         const int block_mode, const ISMRMRD_Image *im) {
    return ISMRMRD_NOERROR;
};

int ismrmrd_read_image(const ISMRMRD_Dataset *dset, const char *varname,
                       const unsigned long index, ISMRMRD_Image *im) {
    return ISMRMRD_NOERROR;
};

int ismrmrd_get_number_of_images(const ISMRMRD_Dataset *dset, const char *varname) {
    return ISMRMRD_NOERROR;
};

int ismrmrd_append_array(const ISMRMRD_Dataset *dset, const char *varname,
                         const int block_mode, const ISMRMRD_NDArray *arr) {
    return ISMRMRD_NOERROR;
};

int ismrmrd_read_array(const ISMRMRD_Dataset *dataset, const char *varname,
                       const unsigned long index, ISMRMRD_NDArray *arr) {
    return ISMRMRD_NOERROR;
};

int ismrmrd_get_number_of_arrays(const ISMRMRD_Dataset *dset, const char *varname) {
    return ISMRMRD_NOERROR;
};

    
#ifdef __cplusplus
} /* extern "C" */
} /* ISMRMRD namespace */
#endif



#ifdef YOMAMA

t = this->type_container_.get_type<float>();
t = this->type_container_.get_type<double>();
t = this->type_container_.get_type<std::complex<float> >();
t = this->type_container_.get_type<std::complex<double> >();
t = this->type_container_.get_type<EncodingCounters>();
t = this->type_container_.get_type<AcquisitionHeader>();
t = this->type_container_.get_type<AcquisitionHeader_with_data>();
t = this->type_container_.get_type<ImageHeader>();
t = this->type_container_.get_type<ImageHeader_with_data<float> >();
t = this->type_container_.get_type<ImageHeader_with_data<double> >();
t = this->type_container_.get_type<ImageHeader_with_data<unsigned short> >();
t = this->type_container_.get_type<ImageHeader_with_data<ccomplex_t> >();
t = this->type_container_.get_type<ImageHeader_with_data<cdouble_complex_t> >();
t = this->type_container_.get_type<ImageHeader_with_data<std::complex<float> > >();
t = this->type_container_.get_type<ImageHeader_with_data<std::complex<double> > >();
t = this->type_container_.get_type<std::string>();

#endif
