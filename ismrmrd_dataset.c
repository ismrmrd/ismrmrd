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

static int create_link(ISMRMRD_Dataset *dset, const char *link_path) {

    if (link_exists(dset, link_path)) {
        return ISMRMRD_NOERROR;
    }
    else {
        hid_t lcpl_id;
        lcpl_id = H5Pcreate(H5P_LINK_CREATE);
        H5Pset_create_intermediate_group(lcpl_id, 1);
        H5Gcreate2(dset->fileid, link_path, lcpl_id, H5P_DEFAULT, H5P_DEFAULT);
        // TODO does this thing return error
        return ISMRMRD_NOERROR;
    }

}

/********************/
/* Public functions */
/********************/
void ismrmrd_init_dataset(ISMRMRD_Dataset *dset) {
    dset->filename = NULL;
    dset->groupname = NULL;
    dset->fileid = 0;
}

int ismrmrd_open_dataset(ISMRMRD_Dataset *dset, const bool create_if_needed) {
    // TODO add a mode for clobbering the dataset if it exists.

    hid_t       fileid, filetype, memtype, space, daset;
    herr_t      status;

    /* Turn of HDF5 Errors */
    /* TODO, this is bad.  Maybe have it compile time dependent */
    /* or add to our error log */
    H5Eset_auto1(NULL, NULL);
    
    /* Check if the file exists and is an HDF5 File */
    status = H5Fis_hdf5(dset->filename);
    
    if (status > 0) {
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
    else if (status == 0) {
       /* Zero value for exists and is NOT HDF5 */
       //TODO raise error
       return ISMRMRD_FILEERROR;
    }
    else {
        /* Negative value for does NOT exist or other error */
        if (create_if_needed == false) {
            // TODO raise error
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

    /* Open the dataset exists, create if needed */
    /* insure that /groupname exists */
    int val = create_link(dset, dset->groupname);

    return val;
};

int ismrmrd_close_dataset(ISMRMRD_Dataset *dset) {

    herr_t      status;

    /* Check for a valid fileid before trying to close the file */
    if (dset->fileid > 0) {
        status = H5Fclose (dset->fileid);
        dset->fileid = 0;
    }
    return ISMRMRD_NOERROR;
};


/*****************************/
/* TODO Implement these ones */  
/*****************************/
int ismrmrd_write_xml_header(const ISMRMRD_Dataset *dset, const char *xml) {
    return ISMRMRD_NOERROR;
};

int ismrmrd_read_xml_header(const ISMRMRD_Dataset *dset, char *xml) {
    return ISMRMRD_NOERROR;
};

int ismrmrd_append_acquisition(const ISMRMRD_Dataset *dset, const ISMRMRD_Acquisition *a) {
    return ISMRMRD_NOERROR;
};

int ismrmrd_read_acquisition(const ISMRMRD_Dataset *dset, unsigned long index, ISMRMRD_Acquisition *a) {
    return ISMRMRD_NOERROR;
};

unsigned long ismrmrd_get_number_of_acquisitions(const ISMRMRD_Dataset *dset) {
    return ISMRMRD_NOERROR;
};

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
xml_header_path_ = groupname_ + std::string("/xml");
data_path_ = groupname_ + std::string("/data");

hid_t t;

t = this->type_container_.get_type<float>();
std::cout << "Type for float: " << t << std::endl;

t = this->type_container_.get_type<double>();
std::cout << "Type for double: " << t << std::endl;

t = this->type_container_.get_type<std::complex<float> >();
std::cout << "Type for complex float: " << t << std::endl;

t = this->type_container_.get_type<std::complex<double> >();
std::cout << "Type for complex double: " << t << std::endl;

t = this->type_container_.get_type<EncodingCounters>();
std::cout << "Type for EncodingCounters: " << t << std::endl;

t = this->type_container_.get_type<AcquisitionHeader>();
std::cout << "Type for AcquisitionHeader: " << t << std::endl;

t = this->type_container_.get_type<AcquisitionHeader_with_data>();
std::cout << "Type for AcquisitionHeader_with_data: " << t << std::endl;

t = this->type_container_.get_type<ImageHeader>();
std::cout << "Type for ImageHeader: " << t << std::endl;

t = this->type_container_.get_type<ImageHeader_with_data<float> >();
std::cout << "Type for ImageHeader_with_data<float>: " << t << std::endl;

t = this->type_container_.get_type<ImageHeader_with_data<double> >();
std::cout << "Type for ImageHeader_with_data<double>: " << t << std::endl;

t = this->type_container_.get_type<ImageHeader_with_data<unsigned short> >();
std::cout << "Type for ImageHeader_with_data<unsigned short>: " << t << std::endl;

t = this->type_container_.get_type<ImageHeader_with_data<ccomplex_t> >();
std::cout << "Type for ImageHeader_with_data<ccomplex_t>: " << t << std::endl;

t = this->type_container_.get_type<ImageHeader_with_data<cdouble_complex_t> >();
std::cout << "Type for ImageHeader_with_data<cdouble_complex_t>: " << t << std::endl;

t = this->type_container_.get_type<ImageHeader_with_data<std::complex<float> > >();
std::cout << "Type for ImageHeader_with_data< std::complex<float> >: " << t << std::endl;

t = this->type_container_.get_type<ImageHeader_with_data<std::complex<double> > >();
std::cout << "Type for ImageHeader_with_data< std::complex<double> >: " << t << std::endl;

t = this->type_container_.get_type<std::string>();
std::cout << "Type for std::string: " << t << std::endl;

#endif
