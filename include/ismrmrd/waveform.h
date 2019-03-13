//
// Created by dch on 26/02/18.
//

#ifndef ISMRMRD_WAVEFORM_H
#define ISMRMRD_WAVEFORM_H

#include "export.h"
#ifdef __cplusplus
#include <cstdint>
namespace ISMRMRD {
extern "C" {
#else
#include <stdint.h>
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
    /**< Number of the acquisition after this waveform */
    uint32_t time_stamp;
    /**< Time stamp of the waveform */
    uint16_t number_of_samples;
    /**< Number of samples acquired */
    uint16_t channels;
    /**< Available channels */
    float sample_time_us;
    /**< Time between samples in micro seconds */
	uint16_t waveform_id;
	/**< Id matching the types specified in the xml header */
} ISMRMRD_WaveformHeader;

typedef struct ISMRMRD_Waveform
{
    ISMRMRD_WaveformHeader head;
    uint32_t *data;
} ISMRMRD_Waveform;


EXPORTISMRMRD int ismrmrd_make_consistent_waveform(ISMRMRD_Waveform* wav);
EXPORTISMRMRD int ismrmrd_size_of_waveform_data(const ISMRMRD_Waveform* wav);
EXPORTISMRMRD ISMRMRD_Waveform* ismrmrd_create_waveform();
EXPORTISMRMRD int ismrmrd_free_waveform(ISMRMRD_Waveform*);
EXPORTISMRMRD int ismrmrd_init_waveform(ISMRMRD_Waveform*);
EXPORTISMRMRD int ismrmrd_init_waveformheader(ISMRMRD_WaveformHeader* header);
EXPORTISMRMRD int ismrmrd_copy_waveform(ISMRMRD_Waveform* dest, const ISMRMRD_Waveform* src);

#ifdef __cplusplus
}
    struct EXPORTISMRMRD WaveformHeader : public ISMRMRD_WaveformHeader {

        // Flag methods
        bool isFlagSet(const uint64_t val);
        void setFlag(const uint64_t val);
        void clearFlag(const uint64_t val);
        void clearAllFlags();

    };
    struct EXPORTISMRMRD Waveform : public ISMRMRD_Waveform {
        Waveform();
        Waveform(const Waveform &other);
        Waveform(Waveform&& other);
        Waveform(uint16_t number_of_samples, uint16_t available_channels);
        ~Waveform();
        Waveform & operator=(const Waveform &other);
        Waveform & operator=(Waveform &&other);

		uint32_t* begin_data();
		uint32_t* end_data();
        const uint32_t* begin_data() const;
		const uint32_t* end_data() const;
		size_t size() const;
    };
}


#endif
#endif //ISMRMRD_WAVEFORM_H_H
