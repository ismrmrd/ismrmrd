//
// Created by dch on 28/02/18.
//
#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/waveform.h"
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
int ismrmrd_make_consistent_waveform(ISMRMRD_Waveform* wav){
    size_t data_size;

    data_size = ismrmrd_size_of_waveform_data(wav);

    if (data_size > 0){
        uint32_t* newPtr = (uint32_t *)(realloc(wav->data,data_size));
        if (newPtr == NULL) {
            return ISMRMRD_PUSH_ERR(ISMRMRD_MEMORYERROR,
                          "Failed to realloc acquisition data array");
        }
        wav->data = newPtr;
    }

    return ISMRMRD_NOERROR;
}

int ismrmrd_size_of_waveform_data(ISMRMRD_Waveform* wav){
    return wav->head.available_channels*wav->head.number_of_samples*sizeof(uint32_t);
}

