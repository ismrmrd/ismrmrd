#ifndef ISMRMRDSERIALIZATION_H
#define ISMRMRDSERIALIZATION_H

#include <exception>
#include <iostream>

#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/waveform.h"
#include "ismrmrd/xml.h"

/**
 * @file serialization.h
 *
 * @brief Serialization functions for ISMRMRD data structures
 *
 * This file contains functions for serializing and deserializing ISMRMRD data
 * The standalone function that serialize to and deserialize from a stream will
 * serrialize the data structures without adding "message id" identifiers.
 * The ProtocolSerializer and ProtocolDeserializer classes are used to to create streams
 * that include the message id in front of each message.
 *
 */

namespace ISMRMRD {

const uint16_t ISMRMRD_MESSAGE_UNPEEKED = 0;
const uint16_t ISMRMRD_MESSAGE_HEADER = 3;
const uint16_t ISMRMRD_MESSAGE_CLOSE = 4;
const uint16_t ISMRMRD_MESSAGE_ACQUISITION = 1008;
const uint16_t ISMRMRD_MESSAGE_IMAGE = 1022;
const uint16_t ISMRMRD_MESSAGE_WAVEFORM = 1026;

// serialize Acquisition to ostream
void EXPORTISMRMRD serialize(const Acquisition &acq, std::ostream &os);

// serialize Image<T> to ostream
template <typename T>
void EXPORTISMRMRD serialize(const Image<T> &img, std::ostream &os);

// serialize Waveform to ostream
void EXPORTISMRMRD serialize(const Waveform &wfm, std::ostream &os);

// deserialize Acquisition from istream
void EXPORTISMRMRD deserialize(Acquisition &acq, std::istream &is);

// deserialize Image<T> from istream
template <typename T>
void EXPORTISMRMRD deserialize(Image<T> &img, std::istream &is);

// deserialize Waveform from istream
void EXPORTISMRMRD deserialize(Waveform &wfm, std::istream &is);

class ProtocolStreamClosed : public std::exception {};

class EXPORTISMRMRD ProtocolSerializer {
public:
    ProtocolSerializer(std::ostream &os);
    void serialize(const IsmrmrdHeader &hdr);
    void serialize(const Acquisition &acq);
    template <typename T>
    void serialize(const Image<T> &img);
    void serialize(const Waveform &wfm);
    void close();

protected:
    void write_msg_id(uint16_t id);
    std::ostream &_os;
};

class EXPORTISMRMRD ProtocolDeserializer {
public:
    ProtocolDeserializer(std::istream &is);
    void deserialize(IsmrmrdHeader &hdr);
    void deserialize(Acquisition &acq);
    template <typename T>
    void deserialize(Image<T> &img);
    void deserialize(Waveform &wfm);

    // Peek at the next data type in the stream
    uint16_t peek();
    int peek_image_data_type();

protected:
    std::istream &_is;
    uint16_t _peeked;
    ImageHeader _peeked_image_header;
};

} // namespace ISMRMRD

#endif // ISMRMRDSERIALIZATION_H
