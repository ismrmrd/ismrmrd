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
    ISMRMRD_Dataset dataset, dataset2;
    const char *filename = "myfile.h5";
    const char *groupname = "/dataset";
    const char *xmlhdr = "This is some text for the header.";

    /* Set the error handler */
    ismrmrd_set_error_handler(myerror);

    /* Create a data set */
    ismrmrd_init_dataset(&dataset, filename, groupname);
    status = ismrmrd_open_dataset(&dataset, true);

    status = ismrmrd_write_header(&dataset, xmlhdr);

    /* Close the dataset */
    status = ismrmrd_close_dataset(&dataset);

    /* Reopen the file as a different dataset */
    ismrmrd_init_dataset(&dataset2, filename, groupname);
    status = ismrmrd_open_dataset(&dataset2, false);

    /* Read the header */
    char *xmlstring = ismrmrd_read_header(&dataset2);
    printf("Header: %s\n", xmlstring);

    /* Get the number of acquisitions */
    printf("Number of Acquisitions: %lu\n", ismrmrd_get_number_of_acquisitions(&dataset2));

    /* Clean up */
    free(xmlstring);

    /* Close the dataset */
    status = ismrmrd_close_dataset(&dataset2);

    ISMRMRD_Acquisition acq;
    ismrmrd_init_acquisition(&acq);
    acq.head.number_of_samples = 128;
    acq.head.active_channels = 4;
    ismrmrd_make_consistent_acquisition(&acq);
    printf("Acq Version: %d\n", acq.head.version);

    ismrmrd_set_flag(&(acq.head.flags), ISMRMRD_ACQ_FIRST_IN_SLICE);
    printf("Flags: %llu\n", acq.head.flags);
    printf("ACQ_FIRST_IN_SLICE: %d\n", ismrmrd_is_flag_set(acq.head.flags, ISMRMRD_ACQ_FIRST_IN_SLICE));
    ismrmrd_clear_flag(&(acq.head.flags), ISMRMRD_ACQ_FIRST_IN_SLICE);
    printf("Flags: %llu\n", acq.head.flags);
    printf("ACQ_FIRST_IN_SLICE: %d\n", ismrmrd_is_flag_set(acq.head.flags, ISMRMRD_ACQ_FIRST_IN_SLICE));

    //   Image im;
    //initImage(&im);
    //printf("Image Version: %d\n", im.head.version);

    //NDArray arr;
    //initNDArray(&arr);
    //printf("Array ndim: %d\n", arr.ndim);
    //printf("Array dim[0]: %d\n", arr.dims[0]);

    return 0;
}
