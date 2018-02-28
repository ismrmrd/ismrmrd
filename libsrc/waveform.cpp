//
// Created by dch on 27/02/18.
//

#include "ismrmrd/waveform.h"


ISMRMRD::Waveform::Waveform(uint16_t number_of_samples, uint16_t available_channels) {

    this->data = new uint32_t[number_of_samples*available_channels];


}

ISMRMRD::Waveform::~Waveform() {
    delete[] this->data;

}



