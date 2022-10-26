#ifndef ISMRMRDSERIALIZATION_H
#define ISMRMRDSERIALIZATION_H

#include <cstdint>
#include <exception>
#include <iostream>

#include "ismrmrd/export.h"
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

enum ISMRMRD_MESSAGE_ID {
    ISMRMRD_MESSAGE_UNPEEKED = 0,
    ISMRMRD_MESSAGE_CONFIG_FILE = 1,
    ISMRMRD_MESSAGE_CONFIG_TEXT = 2,
    ISMRMRD_MESSAGE_HEADER = 3,
    ISMRMRD_MESSAGE_CLOSE = 4,
    ISMRMRD_MESSAGE_TEXT = 5,
    ISMRMRD_MESSAGE_ACQUISITION = 1008,
    ISMRMRD_MESSAGE_IMAGE = 1022,
    ISMRMRD_MESSAGE_WAVEFORM = 1026
};

// A wrapper interface, which we can implement, e.g., for std::istream
class ReadableStreamView {
public:
    virtual void read(char *buffer, size_t count) = 0;

    virtual bool eof() = 0;
};

// A wrapper interface, which we can implement, e.g., for std::ostream
class WritableStreamView {
public:
    virtual void write(const char *buffer, size_t count) = 0;

    virtual bool bad() = 0;
};

// We define a few wrapper structs here to make the serialization code a bit
// more readable.
struct ConfigFile {
    char config[1024];
};

struct ConfigText {
    std::string config_text;
};

struct TextMessage {
    std::string message;
};

// serialize Acquisition to ostream
EXPORTISMRMRD void serialize(const Acquisition &acq, WritableStreamView &ws);

// serialize Image<T> to ostream
template <typename T>
EXPORTISMRMRD void serialize(const Image<T> &img, WritableStreamView &ws);

// serialize Waveform to ostream
EXPORTISMRMRD void serialize(const Waveform &wfm, WritableStreamView &ws);

// serialize const length (1024) char array to ostream. Used for CONFIG FILE
EXPORTISMRMRD void serialize(const ConfigFile &cfg, WritableStreamView &ws);

// serialize a string
EXPORTISMRMRD void serialize(const std::string &str, WritableStreamView &ws);

// deserialize Acquisition from istream
EXPORTISMRMRD void deserialize(Acquisition &acq, ReadableStreamView &rs);

// deserialize Image<T> from istream
template <typename T>
EXPORTISMRMRD void deserialize(Image<T> &img, ReadableStreamView &rs);

// deserialize Waveform from istream
EXPORTISMRMRD void deserialize(Waveform &wfm, ReadableStreamView &rs);

// deserialize const length (1024) char array from istream. Used for CONFIG FILE
EXPORTISMRMRD void deserialize(ConfigFile &cfg, ReadableStreamView &rs);

// deserialize a string
EXPORTISMRMRD void deserialize(std::string &str, ReadableStreamView &rs);

class ProtocolStreamClosed : public std::exception {};

class EXPORTISMRMRD ProtocolSerializer {
public:
    ProtocolSerializer(WritableStreamView &ws);
    void serialize(const ConfigFile &cf);
    void serialize(const ConfigText &ct);
    void serialize(const TextMessage &tm);
    void serialize(const IsmrmrdHeader &hdr);
    void serialize(const Acquisition &acq);
    template <typename T>
    void serialize(const Image<T> &img);
    void serialize(const Waveform &wfm);
    void close();

protected:
    void write_msg_id(uint16_t id);
    WritableStreamView &_ws;
};

class EXPORTISMRMRD ProtocolDeserializer {
public:
    ProtocolDeserializer(ReadableStreamView &rs);
    void deserialize(ConfigFile &cf);
    void deserialize(ConfigText &ct);
    void deserialize(TextMessage &tm);
    void deserialize(IsmrmrdHeader &hdr);
    void deserialize(Acquisition &acq);
    template <typename T>
    void deserialize(Image<T> &img);
    void deserialize(Waveform &wfm);

    // Peek at the next data type in the stream
    uint16_t peek();
    int peek_image_data_type();

protected:
    ReadableStreamView &_rs;
    uint16_t _peeked;
    ImageHeader _peeked_image_header;
};

} // namespace ISMRMRD

#endif // ISMRMRDSERIALIZATION_H
