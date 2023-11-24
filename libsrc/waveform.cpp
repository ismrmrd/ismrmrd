//
// Created by dch on 27/02/18.
//

#include <cstring>
#include <cstdlib>
#include "ismrmrd/waveform.h"
#include <algorithm>
#include <ismrmrd/ismrmrd.h>

namespace ISMRMRD {

bool operator==(const ISMRMRD_WaveformHeader& left, const ISMRMRD_WaveformHeader& right){
    return left.version == right.version &&
        left.flags == right.flags &&
        left.measurement_uid == right.measurement_uid &&
        left.scan_counter == right.scan_counter &&
        left.time_stamp == right.time_stamp &&
        left.number_of_samples == right.number_of_samples &&
        left.channels == right.channels &&
        left.sample_time_us == right.sample_time_us &&
        left.waveform_id == right.waveform_id;
}

bool operator==(ISMRMRD_Waveform const &left, ISMRMRD_Waveform const &right)
{
    return (left.head == right.head) &&
           ((left.data == right.data) || ((left.data != NULL) && (right.data != NULL) && (memcmp(left.data, right.data, ismrmrd_size_of_waveform_data(&left)) == 0)));
}

}

ISMRMRD::Waveform::Waveform(uint16_t number_of_samples, uint16_t channels) {

    ismrmrd_init_waveform(this);
    this->head.channels = channels;
    this->head.number_of_samples = number_of_samples;
    this->head.waveform_id =0;
	this->data = (uint32_t*)malloc(this->head.channels*this->head.number_of_samples* sizeof(uint32_t));


}
size_t ISMRMRD::Waveform::size() const {
	return head.channels*head.number_of_samples;
}

uint32_t* ISMRMRD::Waveform::begin_data() {
	return data;
}

uint32_t* ISMRMRD::Waveform::end_data() {
	return data + this->size();
}

const uint32_t* ISMRMRD::Waveform::begin_data() const {
	return data;
}

const uint32_t* ISMRMRD::Waveform::end_data() const {
	return data + this->size();
}

ISMRMRD::Waveform::Waveform() {
    ismrmrd_init_waveform(this);
}

ISMRMRD::Waveform::Waveform(const Waveform &other) {


	size_t datasize = other.size();
	if (datasize == 0)
		this->data = NULL;
	else {
		this->data = (uint32_t *)malloc(datasize* sizeof(uint32_t));
		memcpy(this->data, other.data, other.size() * sizeof(uint32_t));
	}

	this->head = other.head;
}

#if !ISMRMRD_CPP03_SUPPORT
ISMRMRD::Waveform::Waveform(Waveform &&other) {
    this->data = other.data;
    other.data = NULL;
	this->head = other.head;

}
#endif

ISMRMRD::Waveform::~Waveform() {
	if (data != NULL) free(data);


}

#if !ISMRMRD_CPP03_SUPPORT
ISMRMRD::Waveform & ISMRMRD::Waveform::operator=(Waveform &&other) {
	if (data != NULL) free(data);
    this->data = other.data;
    other.data = nullptr;
	this->head = other.head;
    return *this;
}
#endif

ISMRMRD::Waveform & ISMRMRD::Waveform::operator=(const Waveform &other) {

	if (this->data != NULL) free(this->data);

	size_t datasize = other.size();
	if (datasize == 0)
		this->data = NULL;
	else {
		this->data = (uint32_t*) malloc(sizeof(uint32_t)*datasize);
		memcpy(this->data, other.data, other.size() * sizeof(uint32_t));
	}


	this->head = other.head;
    return *this;
}

bool ISMRMRD::WaveformHeader::isFlagSet(const uint64_t val) {
    return ismrmrd_is_flag_set(flags, val);
};

void ISMRMRD::WaveformHeader::setFlag(const uint64_t val) {
    ismrmrd_set_flag(&flags, val);
};

void ISMRMRD::WaveformHeader::clearFlag(const uint64_t val) {
    ismrmrd_clear_flag(&flags, val);
};

void ISMRMRD::WaveformHeader::clearAllFlags() {
    ismrmrd_clear_all_flags(&flags);
};
