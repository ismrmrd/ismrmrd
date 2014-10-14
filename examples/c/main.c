#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/dataset.h"

void myerror(const char *file, int line, const char *func, int code, const char *msg) {
    char *msgtype = ismrmrd_strerror(code);
    fprintf(stderr, "Whoa! ERROR: %s in %s, line %d: %s\n", msgtype, file, line, msg);
    exit(-1);
}

int main(void)
{

    /* Declarations */
    int nacq_write, n, k, c;
    ISMRMRD_Dataset dataset1;
    ISMRMRD_Acquisition acq, acq2, acq3;
    ISMRMRD_Dataset dataset2;
    char *xmlstring;
    ISMRMRD_Image im;
    uint32_t index;
    uint64_t loc;
    uint32_t nacq_read;
    const char *filename = "myfile.h5";
    const char *groupname = "/dataset";
    const char *xmlhdr = "Yo! This is some text for the header.";
    const char *attr_string = "Yo! This is some text for the attribute string.";

    /* Set the error handler */
    ismrmrd_set_error_handler(myerror);

    /************/
    /* New File */
    /************/
    /* Create a data set */
    ismrmrd_init_dataset(&dataset1, filename, groupname);
    ismrmrd_open_dataset(&dataset1, true);

    /* Write an XML header */
    ismrmrd_write_header(&dataset1, xmlhdr);

    /* Append some acquisitions */
    /* must initialize an acquisition before you can use it */
    ismrmrd_init_acquisition(&acq);
    nacq_write = 5;
    for (n=0; n < nacq_write; n++) {
        /* must free an acquisition before you can reinitialize it */
        ismrmrd_init_acquisition(&acq);
        ismrmrd_init_acquisition(&acq);
        acq.head.number_of_samples = 128;
        acq.head.active_channels = 4;
        ismrmrd_make_consistent_acquisition(&acq);
        for (k=0; k<acq.head.number_of_samples; k++) {
            for (c=0; c<acq.head.active_channels; c++) {
#ifdef _MSC_VER
                /* Windows C compilers don't have a good complex type */
                acq.data[k*acq.head.active_channels + c].real = n;
                acq.data[k*acq.head.active_channels + c].imag = n;
#else
                acq.data[k*acq.head.active_channels + c] = n + I*n;
#endif
            }
        }
        if (n == 0) {
            ismrmrd_set_flag(&(acq.head.flags), ISMRMRD_ACQ_FIRST_IN_SLICE);
        }
        else if (n == nacq_write-1) {
            ismrmrd_set_flag(&(acq.head.flags), ISMRMRD_ACQ_LAST_IN_SLICE);
        }
        ismrmrd_append_acquisition(&dataset1, &acq);
    }
    
    /* Close the dataset */
    ismrmrd_close_dataset(&dataset1);

    /************/
    /* Old File */
    /************/
    /* Reopen the file as a different dataset */
    ismrmrd_init_dataset(&dataset2, filename, groupname);
    ismrmrd_open_dataset(&dataset2, false);

    /* Read the header */
    xmlstring = ismrmrd_read_header(&dataset2);
    printf("Header: %s\n", xmlstring);

    /* Get the number of acquisitions */
    nacq_read = ismrmrd_get_number_of_acquisitions(&dataset2);
    printf("Number of Acquisitions: %u\n", nacq_read);

    /* read the next to last one */
    ismrmrd_init_acquisition(&acq2);
    index = 0;
    if (nacq_read>1) {
        index = nacq_read - 1;
    }
    else {
        index = 0;
    }
    printf("Acquisition index: %u\n", index);
    ismrmrd_read_acquisition(&dataset2, index, &acq2);
    printf("Number of samples: %u\n", acq2.head.number_of_samples);
    printf("Flags: %llu\n", acq2.head.flags);
#ifdef _MSC_VER
    /* Windows C compilers don't have a good complex type */
    printf("Data 3: %f\t 2: %f\n", acq2.data[4].real, acq2.data[4].imag);
#else
    printf("Data[4]: %f, %f\n", creal(acq2.data[4]), cimag(acq2.data[4]));
#endif
    
    ismrmrd_init_acquisition(&acq3);
    ismrmrd_copy_acquisition(&acq3, &acq2);
    
    printf("Pointers 3: %p\t 2: %p\n", (void *) acq3.data, (void *) acq2.data);
#ifdef _MSC_VER
    /* Windows C compilers don't have a good complex type */
    printf("Data 3: %f\t 2: %f\n", acq3.data[4].real, acq2.data[4].real);
#else
    printf("Data 3: %f\t 2: %f\n", creal(acq3.data[4]), creal(acq2.data[4]));
#endif

    /* Create and store an image */
    ismrmrd_init_image(&im);
    im.head.data_type = ISMRMRD_FLOAT;
    im.head.matrix_size[0] = 256;
    im.head.matrix_size[1] = 256;
    im.head.matrix_size[2] = 4;
    im.head.channels = 8;
    /* Add an attribute string */
    im.head.attribute_string_len = strlen(attr_string);
    ismrmrd_make_consistent_image(&im);
    memcpy(im.attribute_string, attr_string, im.head.attribute_string_len);
    memset(im.data, 0, 256*256*4*8*sizeof(float));
    
    printf("Image Version: %d\n", im.head.version);
    printf("Image String: %s\n", im.attribute_string);
    ismrmrd_append_image(&dataset2, "testimages", 1, &im);
    for (loc=0; loc < 256*256*4*8; loc++) {
        ((float*)im.data)[loc] = 2.0;
    }
    ismrmrd_append_image(&dataset2, "testimages", 1, &im);
                         
    /* Clean up */
    /* This frees the internal memory of the acquisitions */
    ismrmrd_cleanup_acquisition(&acq);
    ismrmrd_cleanup_acquisition(&acq2);
    ismrmrd_cleanup_acquisition(&acq3);
    free(xmlstring);

    /* Close the dataset */
    ismrmrd_close_dataset(&dataset2);

    return 0;
}
