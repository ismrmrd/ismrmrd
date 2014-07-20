#include <stdio.h>
#include <ismrmrd.h>

#ifdef __cplusplus
using namespace ISMRMRD;
#endif

int main(void)
{

   Acquisition acq;
   InitAcquisition(&acq);
   acq.head.number_of_samples = 128;
   acq.head.active_channels = 4;
   MakeConsistentAcquisition(&acq);
   printf("Acq Version: %d\n", acq.head.version);

   flags_set(&(acq.head.flags), ACQ_FIRST_IN_SLICE);
   printf("Flags: %llu\n", acq.head.flags);
   printf("ACQ_FIRST_IN_SLICE: %d\n", flags_is_set(acq.head.flags, ACQ_FIRST_IN_SLICE));
   flags_clear(&(acq.head.flags), ACQ_FIRST_IN_SLICE);
   printf("Flags: %llu\n", acq.head.flags);
   printf("ACQ_FIRST_IN_SLICE: %d\n", flags_is_set(acq.head.flags, ACQ_FIRST_IN_SLICE));


   Image im;
   InitImage(&im);
   printf("Image Version: %d\n", im.head.version);

   NDArray arr;
   InitNDArray(&arr);
   printf("Array ndim: %d\n", arr.ndim);
   printf("Array dim[0]: %d\n", arr.dims[0]);  

   return 0;
}
