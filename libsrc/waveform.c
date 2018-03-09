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
#include <ismrmrd/version.h>

#endif /* __cplusplus */

#ifdef __cplusplus
namespace ISMRMRD {
extern "C" {
#endif

int ismrmrd_make_consistent_waveform(ISMRMRD_Waveform *wav) {
    size_t data_size;

    data_size = ismrmrd_size_of_waveform_data(wav);

    if (data_size > 0) {
        uint32_t *newPtr = (uint32_t *) (realloc(wav->data, data_size));
        if (newPtr == NULL) {
            return ISMRMRD_PUSH_ERR(ISMRMRD_MEMORYERROR,
                                    "Failed to realloc acquisition data array");
        }
        wav->data = newPtr;
    }

    return ISMRMRD_NOERROR;
}

int ismrmrd_size_of_waveform_data(const ISMRMRD_Waveform *wav) {
    return wav->head.channels * wav->head.number_of_samples * sizeof(uint32_t);
}


ISMRMRD_Waveform *ismrmrd_create_waveform() {
    ISMRMRD_Waveform *wav = (ISMRMRD_Waveform *) malloc(sizeof(ISMRMRD_Waveform));
    if (wav == NULL) {
        ISMRMRD_PUSH_ERR(ISMRMRD_MEMORYERROR, "Failed to malloc new ISMRMRD_Waveform.");
        return NULL;
    }
    if (ismrmrd_init_waveform(wav) != ISMRMRD_NOERROR) {
        ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Failed to initialize waveform.");
        return NULL;
    }
    return wav;
}

int ismrmrd_init_waveformheader(ISMRMRD_WaveformHeader *hdr) {
    memset(hdr, 0, sizeof(ISMRMRD_WaveformHeader));
    hdr->channels = 1;
    hdr->version = ISMRMRD_VERSION_MAJOR;
    return ISMRMRD_NOERROR;

}

int ismrmrd_init_waveform(ISMRMRD_Waveform *wav) {

    ismrmrd_init_waveformheader(&wav->head);
    wav->data = NULL;
    return ISMRMRD_NOERROR;

}

int ismrmrd_copy_waveform(ISMRMRD_Waveform *dest, const ISMRMRD_Waveform *src) {

    if (src == NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Source pointer should not NULL.");
    }
    if (dest == NULL) {
        return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Destination pointer should not NULL.");
    }

    memcpy(&dest->head, &src->head, sizeof(ISMRMRD_WaveformHeader));
    ismrmrd_make_consistent_waveform(dest);

    memcpy(dest->data, src->data, ismrmrd_size_of_waveform_data(src));

    return ISMRMRD_NOERROR;

}

int ismrmrd_free_waveform(ISMRMRD_Waveform* wav) {
	if (wav == NULL) {
		return ISMRMRD_PUSH_ERR(ISMRMRD_RUNTIMEERROR, "Pointer should not be NULL.");
	}
	free(wav->data);
	free(wav);
	return ISMRMRD_NOERROR;

}

#ifdef __cplusplus
}
}
#endif