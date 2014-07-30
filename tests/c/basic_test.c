#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ismrmrd.h"
#include "ismrmrd_dataset.h"

int main(void)
{

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

   const char *filename = "myfile.h5";
   const char *groupname = "/G1/V";

   ISMRMRD_Dataset dataset;
   ismrmrd_init_dataset(&dataset, filename, groupname);
   printf("Filename: %s\n", dataset.filename);
   printf("Groupname: %s\n", dataset.groupname);
   
   int status;
   status = ismrmrd_open_dataset(&dataset, true);
   printf("Status from open: %d\n", status);
   printf("File_id: %d\n", dataset.fileid);

   const char *xmlhdr="Hello world.";
   status = ismrmrd_write_header(&dataset, xmlhdr);
   printf("XMLString: %s\nLength: %lu\n", xmlhdr, strlen(xmlhdr));

   char *xmlstring = ismrmrd_read_header(&dataset);
   printf("XMLString OUT: %s\n", xmlstring);
   free(xmlstring);

   status = ismrmrd_close_dataset(&dataset);
   printf("File_id: %d\n", dataset.fileid);
   printf("File close status: %d\n", status);

   return 0;
}
