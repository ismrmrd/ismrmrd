//
// Created by dch on 27/02/18.
//

#include <cstring>
#include "ismrmrd/waveform.h"


ISMRMRD::Waveform::Waveform(uint16_t number_of_samples, uint16_t available_channels) {

    ismrmrd_init_waveform(this);
    this->data = new uint32_t[number_of_samples*available_channels];
    this->head.available_channels = available_channels;
    this->head.number_of_samples = number_of_samples;
    this->head.waveform_id =0;


}

ISMRMRD::Waveform::Waveform() {
    ismrmrd_init_waveform(this);
}

ISMRMRD::Waveform::Waveform(const Waveform &other) {
    ismrmrd_copy_waveform(this,&other);
}

ISMRMRD::Waveform::Waveform(Waveform &&other) {
    memcpy(&this->head,&other.head,sizeof(ISMRMRD_WaveformHeader));
    this->data = other.data;
    other.data = nullptr;


}

ISMRMRD::Waveform::~Waveform() {
    delete[] this->data;

}

ISMRMRD::Waveform & ISMRMRD::Waveform::operator=(Waveform &&other) {
    memcpy(&this->head,&other.head,sizeof(ISMRMRD_WaveformHeader));
    this->data = other.data;
    other.data = nullptr;
    return *this;
}

ISMRMRD::Waveform & ISMRMRD::Waveform::operator=(const Waveform &other) {
    ismrmrd_copy_waveform(this,&other);
    return *this;
}


