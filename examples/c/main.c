#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ismrmrd.h"
#include "ismrmrd_dataset.h"

void myerror(const char *file, int line, const char *func, int err, char *msg) {
    char *msgtype = ismrmrd_strerror(err);
    fprintf(stderr, "Whoa! ERROR: %s in %s, line %d: %s\n", msgtype, file, line, msg);
    exit(-1);
}

int main(void)
{
    int status;

    /* Set the error handler */
    ismrmrd_set_error_handler(myerror);

    /************/
    /* New File */
    /************/
    /* Create a data set */
    ISMRMRD_Dataset dataset1;
    const char *filename = "myfile.h5";
    const char *groupname = "/dataset";
    ismrmrd_init_dataset(&dataset1, filename, groupname);
    status = ismrmrd_open_dataset(&dataset1, true);

    /* Write an XML header */
    const char *xmlhdr = "Yo! This is some text for the header.";
    status = ismrmrd_write_header(&dataset1, xmlhdr);

    /* Append some acquisitions */
    ISMRMRD_Acquisition acq;
    int nacq_write = 5;
    for (int n=0; n < nacq_write; n++) {
        ismrmrd_init_acquisition(&acq);
        acq.head.number_of_samples = 128;
        acq.head.active_channels = 4;
        ismrmrd_make_consistent_acquisition(&acq);
        for (int k=0; k<acq.head.number_of_samples; k++) {
            for (int c=0; c<acq.head.active_channels; c++) {
                acq.data[k*acq.head.active_channels + c] = n + I*n;
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
    status = ismrmrd_close_dataset(&dataset1);

    /************/
    /* Old File */
    /************/
    /* Reopen the file as a different dataset */
    ISMRMRD_Dataset dataset2;
    ismrmrd_init_dataset(&dataset2, filename, groupname);
    status = ismrmrd_open_dataset(&dataset2, false);

    /* Read the header */
    char *xmlstring = ismrmrd_read_header(&dataset2);
    printf("Header: %s\n", xmlstring);

    /* Get the number of acquisitions */
    unsigned long nacq_read = ismrmrd_get_number_of_acquisitions(&dataset2);
    printf("Number of Acquisitions: %lu\n", nacq_read);

    /* read the next to last one */
    ISMRMRD_Acquisition acq2;
    unsigned long index = 0;
    if (nacq_read>1) {
        index = nacq_read - 1;
    }
    else {
        index = 0;
    }
    printf("Acquisition index: %lu\n", index);
    ismrmrd_read_acquisition(&dataset2, index, &acq2);
    printf("Number of samples: %hu\n", acq2.head.number_of_samples);
    printf("Flags: %llu\n", acq2.head.flags);
    printf("Data[4]: %f, %f\n", creal(acq2.data[4]), cimag(acq2.data[4]));
    
    /* Clean up */
    free(xmlstring);

    /* Close the dataset */
    status = ismrmrd_close_dataset(&dataset2);

    //   Image im;
    //initImage(&im);
    //printf("Image Version: %d\n", im.head.version);

    //NDArray arr;
    //initNDArray(&arr);
    //printf("Array ndim: %d\n", arr.ndim);
    //printf("Array dim[0]: %d\n", arr.dims[0]);

    return 0;
}
