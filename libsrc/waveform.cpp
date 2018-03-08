//
// Created by dch on 27/02/18.
//

#include <cstring>
#include "ismrmrd/waveform.h"
#include <algorithm>

ISMRMRD::Waveform::Waveform(uint16_t number_of_samples, uint16_t available_channels) {

    ismrmrd_init_waveform(this);
    this->head.available_channels = available_channels;
    this->head.number_of_samples = number_of_samples;
    this->head.waveform_id =0;
	this->data = new uint32_t[this->head.available_channels*this->head.number_of_samples];


}
size_t ISMRMRD::Waveform::size() const {
	return head.available_channels*head.number_of_samples;
}

uint32_t* ISMRMRD::Waveform::begin_data() {
	return data;
}

uint32_t* ISMRMRD::Waveform::end_data() {
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
		this->data = new uint32_t[datasize];
		memcpy(this->data, other.data, other.size() * sizeof(uint32_t));
	}

	this->head = other.head;
}

ISMRMRD::Waveform::Waveform(Waveform &&other) {
    this->data = other.data;
    other.data = NULL;
	this->head = other.head;

}

ISMRMRD::Waveform::~Waveform() {
	if (data != NULL) delete[] data;
    

}

ISMRMRD::Waveform & ISMRMRD::Waveform::operator=(Waveform &&other) {
	
	if (data != NULL) delete[] data;
    this->data = other.data;
    other.data = nullptr;
	this->head = other.head;
    return *this;
}

ISMRMRD::Waveform & ISMRMRD::Waveform::operator=(const Waveform &other) {
	
	if (this->data != NULL) delete[] this->data;
	
	size_t datasize = other.size();
	if (datasize == 0)
		this->data = NULL;
	else {
		this->data = new uint32_t[datasize];
		memcpy(this->data, other.data, other.size() * sizeof(uint32_t));
	}


	this->head = other.head;
    return *this;
}


