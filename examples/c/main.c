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


    printf("%zu %zu %zu %zu %zu %zu %zu %zu %zu \n", 
        HOFFSET(ISMRMRD_WaveformHeader, version),
        HOFFSET(ISMRMRD_WaveformHeader, flags),
        HOFFSET(ISMRMRD_WaveformHeader, measurement_uid),
        HOFFSET(ISMRMRD_WaveformHeader, scan_counter),
        HOFFSET(ISMRMRD_WaveformHeader, time_stamp),
        HOFFSET(ISMRMRD_WaveformHeader, number_of_samples),
        HOFFSET(ISMRMRD_WaveformHeader, channels),
        HOFFSET(ISMRMRD_WaveformHeader, sample_time_us),
        HOFFSET(ISMRMRD_WaveformHeader, waveform_id));

    printf("Size %zu \n",sizeof(ISMRMRD_WaveformHeader));

    /* Declarations */
    int nacq_write, n, k, c;
    ISMRMRD_Dataset dataset1;
    ISMRMRD_Acquisition acq, acq2, acq3;
    ISMRMRD_Dataset dataset2;
    char *xmlstring;
    ISMRMRD_Image im, im2;
    uint32_t index;
    uint64_t loc;
    uint32_t nacq_read;
    uint32_t numim;
    const char *filename = "myfile.h5";
    const char *groupname = "/dataset";
    const char *xmlhdr = "Yo! This is some text for the header.";
    const char *attr_string = "Yo! This is some text for the attribute string.";
    ISMRMRD_NDArray arr, arr2;

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
    nacq_write = 5;
    for (n=0; n < nacq_write; n++) {
        /* must initialize an acquisition before you can use it */
        ismrmrd_init_acquisition(&acq);
        acq.head.number_of_samples = 128;
        acq.head.active_channels = 4;
        for (k=0; k<acq.head.active_channels; k++) {
            ismrmrd_set_channel_on(acq.head.channel_mask, k);
        }
        ismrmrd_make_consistent_acquisition(&acq);
        for (k=0; k<acq.head.number_of_samples; k++) {
            for (c=0; c<acq.head.active_channels; c++) {
#ifdef _MSC_VER
                /* Windows C compilers don't have a good complex type */
                acq.data[k*acq.head.active_channels + c].real = (float)n;
                acq.data[k*acq.head.active_channels + c].imag = (float)n;
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
        ismrmrd_cleanup_acquisition(&acq);
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
    free(xmlstring);

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
    printf("Flags: %llu\n", (unsigned long long)acq2.head.flags);
    printf("Channel Mask[0]: %llu\n", (unsigned long long)acq2.head.channel_mask[0]);
    printf("Channel Mask[1]: %llu\n", (unsigned long long)acq2.head.channel_mask[1]);
    printf("Channel 3 is %d\n", ismrmrd_is_channel_on(acq2.head.channel_mask, 3));
    printf("Channel 5 is %d\n", ismrmrd_is_channel_on(acq2.head.channel_mask, 5));
    
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
    ismrmrd_cleanup_acquisition(&acq2);
    ismrmrd_cleanup_acquisition(&acq3);

    /* Create and store an image */
    ismrmrd_init_image(&im);
    im.head.data_type = ISMRMRD_FLOAT;
    im.head.matrix_size[0] = 256;
    im.head.matrix_size[1] = 256;
    im.head.matrix_size[2] = 4;
    im.head.channels = 8;
    /* Add an attribute string */
    im.head.attribute_string_len = (uint32_t)strlen(attr_string);
    ismrmrd_make_consistent_image(&im);
    memcpy(im.attribute_string, attr_string, im.head.attribute_string_len);
    memset(im.data, 0, 256*256*4*8*sizeof(float));
    
    printf("Image Version: %d\n", im.head.version);
    printf("Image String: %s\n", im.attribute_string);
    ismrmrd_append_image(&dataset2, "testimages", &im);
    for (loc=0; loc < 256*256*4*8; loc++) {
        ((float*)im.data)[loc] = 2.0;
    }
    ismrmrd_append_image(&dataset2, "testimages", &im);
    ismrmrd_cleanup_image(&im);

    numim = ismrmrd_get_number_of_images(&dataset2, "testimages");
    printf("Number of images stored = %d\n", numim);
    
    ismrmrd_init_image(&im2);
    ismrmrd_read_image(&dataset2, "testimages", 1, &im2);
    printf("Image 1 attribute string = %s\n", im2.attribute_string);
    printf("Image 1 at position 10 has value = %f\n", ((float*)im2.data)[10]);
    ismrmrd_cleanup_image(&im2);

    /* Create and store an array */
    ismrmrd_init_ndarray(&arr);
    arr.data_type = ISMRMRD_FLOAT;
    arr.ndim = 3;
    arr.dims[0] = 256;
    arr.dims[1] = 128;
    arr.dims[2] = 4;
    ismrmrd_make_consistent_ndarray(&arr);
    for (loc=0; loc < 256*128*4; loc++) {
        ((float*)arr.data)[loc] = 2.0;
    }
    ismrmrd_append_array(&dataset2, "testarray", &arr);
    printf("Number of arrays stored = %d\n", ismrmrd_get_number_of_arrays(&dataset2, "testarray"));
    ismrmrd_cleanup_ndarray(&arr);

    /* Read it back in */
    ismrmrd_init_ndarray(&arr2);
    ismrmrd_read_array(&dataset2, "testarray", 0, &arr2);
    printf("Array 2 at position 10 has value = %f\n", ((float*)arr2.data)[10]);
    ismrmrd_cleanup_ndarray(&arr2);
    
    /* Close the dataset */
    ismrmrd_close_dataset(&dataset2);

    return 0;
}
