#ifndef ISMRMRDSERIALIZATION_H
#define ISMRMRDSERIALIZATION_H

#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>

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

// A wrapper, which we can use wrap any object that has a read(...) method
class ReadableStream {
public:
    void read(char *buffer, size_t count) {
        reader_impl->read(buffer, count);
    }

    template <class HAS_READ>
    ReadableStream(HAS_READ &has_read) {
        // make_unique is C++14
        reader_impl = std::unique_ptr<ReadableStreamT<HAS_READ> >(new ReadableStreamT<HAS_READ>(has_read));
    }

private:
    struct ReadableStreamImpl {
        virtual void read(char *buffer, size_t count) = 0;
    };

    template <class T>
    struct ReadableStreamT : public ReadableStreamImpl {
        ReadableStreamT(T &readable) : self{ readable } {}
        ~ReadableStreamT() {}
        void read(char *buffer, size_t count) {
            self.read(buffer, count);
        }
        T &self;
    };
    std::unique_ptr<ReadableStreamImpl> reader_impl;
};

// A wrapper, which we can use wrap any object that has a write(...) method
class WritableStream {
public:
    void write(const char *buffer, size_t count) {
        writer_impl->write(buffer, count);
    }

    template <class HAS_WRITE>
    WritableStream(HAS_WRITE &has_write) {
        // make_unique is C++14
        writer_impl = std::unique_ptr<WritableStreamT<HAS_WRITE> >(new WritableStreamT<HAS_WRITE>(has_write));
    }

private:
    struct WritableStreamImpl {
        virtual void write(const char *buffer, size_t count) = 0;
    };
    template <class T>
    struct WritableStreamT : public WritableStreamImpl {
        WritableStreamT(T &writable) : self{ writable } {}

        // Destructor
        ~WritableStreamT() {}
        void write(const char *buffer, size_t count) {
            self.write(buffer, count);
        }
        T &self;
    };

    std::unique_ptr<WritableStreamImpl> writer_impl;
};

// serialize Acquisition to ostream
void EXPORTISMRMRD serialize(const Acquisition &acq, WritableStream &ws);

// serialize Image<T> to ostream
template <typename T>
void EXPORTISMRMRD serialize(const Image<T> &img, WritableStream &ws);

// serialize Waveform to ostream
void EXPORTISMRMRD serialize(const Waveform &wfm, WritableStream &ws);

// deserialize Acquisition from istream
void EXPORTISMRMRD deserialize(Acquisition &acq, ReadableStream &rs);

// deserialize Image<T> from istream
template <typename T>
void EXPORTISMRMRD deserialize(Image<T> &img, ReadableStream &rs);

// deserialize Waveform from istream
void EXPORTISMRMRD deserialize(Waveform &wfm, ReadableStream &rs);

class ProtocolStreamClosed : public std::exception {};

class EXPORTISMRMRD ProtocolSerializer {
public:
    ProtocolSerializer(WritableStream &ws);
    void serialize(const IsmrmrdHeader &hdr);
    void serialize(const Acquisition &acq);
    template <typename T>
    void serialize(const Image<T> &img);
    void serialize(const Waveform &wfm);
    void close();

protected:
    void write_msg_id(uint16_t id);
    WritableStream &_ws;
};

class EXPORTISMRMRD ProtocolDeserializer {
public:
    ProtocolDeserializer(ReadableStream &rs);
    void deserialize(IsmrmrdHeader &hdr);
    void deserialize(Acquisition &acq);
    template <typename T>
    void deserialize(Image<T> &img);
    void deserialize(Waveform &wfm);

    // Peek at the next data type in the stream
    uint16_t peek();
    int peek_image_data_type();

protected:
    ReadableStream &_rs;
    uint16_t _peeked;
    ImageHeader _peeked_image_header;
};

} // namespace ISMRMRD

#endif // ISMRMRDSERIALIZATION_H
