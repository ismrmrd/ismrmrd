#include <stdio.h>
#include <ismrmrd.h>

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

   return 0;
}
