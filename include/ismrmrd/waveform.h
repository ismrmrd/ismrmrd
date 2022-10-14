//
// Created by dch on 26/02/18.
//

// clang-format off

#ifndef ISMRMRD_WAVEFORM_H
#define ISMRMRD_WAVEFORM_H

#if __cplusplus > 199711L
#include <type_traits>
#endif

#include "export.h"
#ifdef __cplusplus
#include <cstdint>
#include <cstddef>
namespace ISMRMRD {
extern "C" {
#else
#include <stdint.h>
#endif

// Note: This header does not use 2-byte packing like the rest of the ISMRMRD
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


#if __cplusplus > 199711L // Static assert requires C++11
// Check size and offsets of WaveformHeader
static_assert(sizeof(ISMRMRD_WaveformHeader) == 40, "ISMRMRD_WaveformHeader is not 40 bytes");
static_assert(offsetof(ISMRMRD_WaveformHeader, version) == 0, "ISMRMRD WaveformHeader version offset is not correct");
static_assert(offsetof(ISMRMRD_WaveformHeader, flags) == 8, "ISMRMRD WaveformHeader flags offset is not correct");
static_assert(offsetof(ISMRMRD_WaveformHeader, measurement_uid) == 16, "ISMRMRD WaveformHeader measurement_uid offset is not correct");
static_assert(offsetof(ISMRMRD_WaveformHeader, scan_counter) == 20, "ISMRMRD WaveformHeader scan_counter offset is not correct");
static_assert(offsetof(ISMRMRD_WaveformHeader, time_stamp) == 24, "ISMRMRD WaveformHeader time_stamp offset is not correct");
static_assert(offsetof(ISMRMRD_WaveformHeader, number_of_samples) == 28, "ISMRMRD WaveformHeader number_of_samples offset is not correct");
static_assert(offsetof(ISMRMRD_WaveformHeader, channels) == 30, "ISMRMRD WaveformHeader channels offset is not correct");
static_assert(offsetof(ISMRMRD_WaveformHeader, sample_time_us) == 32, "ISMRMRD WaveformHeader sample_time_us offset is not correct");
static_assert(offsetof(ISMRMRD_WaveformHeader, waveform_id) == 36, "ISMRMRD WaveformHeader waveform_id offset is not correct");
#endif

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

    #if __cplusplus > 199711L // Static assert and is_standard_layout requires C++11
    // check size of WaveformHeader
    static_assert(sizeof(WaveformHeader) == sizeof(ISMRMRD_WaveformHeader), "WaveformHeader is not the same size as ISMRMRD_WaveformHeader");
    static_assert(std::is_standard_layout<WaveformHeader>::value, "WaveformHeader is not a standard layout type");
    #endif

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
