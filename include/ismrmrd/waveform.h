//
// Created by dch on 26/02/18.
//

#ifndef ISMRMRD_WAVEFORM_H
#define ISMRMRD_WAVEFORM_H

#include "export.h"
#ifdef __cplusplus
#include <cstdint>
extern "C" {
#endif

typedef struct ISMRMRD_WaveformHeader
{
    uint16_t version;
    /**< First unsigned int indicates the version */
    uint64_t flags;
    /**< bit field with flags */
    uint32_t measurement_uid;
    /**< Unique ID for the measurement */
    uint32_t scan_counter;
    /**< Current acquisition number in the measurement */
    uint32_t time_stamp;
    /**< Time stamp of the waveform */
    uint16_t number_of_samples;
    /**< Number of samples acquired */
    uint16_t available_channels;
    /**< Available channels */
    float sample_time_us;
    /**< Time between samples in micro seconds */
    char type[16];                                       /**< Name of the waveform type*/
} ISMRMRD_WaveformHeader;

typedef struct ISMRMRD_Waveform
{
    ISMRMRD_WaveformHeader head;
    uint32_t *data;
} ISMRMRD_Waveform;


EXPORTISMRMRD int ismrmrd_make_consistent_waveform(ISMRMRD_Waveform* wav);
EXPORTISMRMRD int ismrmrd_size_of_waveform_data(ISMRMRD_Waveform* wav);

#ifdef __cplusplus
}
namespace ISMRMRD {
    struct Waveform : public ISMRMRD_Waveform {
        Waveform(uint16_t number_of_samples, uint16_t available_channels);
        ~Waveform();

    };
}
#endif
#endif //ISMRMRD_WAVEFORM_H_H
