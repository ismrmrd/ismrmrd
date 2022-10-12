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

    bool eof() {
        return reader_impl->eof();
    }

    template <class HAS_READ>
    ReadableStream(HAS_READ &has_read) {
        reader_impl = new ReadableStreamT<HAS_READ>(has_read);
    }

    virtual ~ReadableStream() {
        delete reader_impl;
    }

private:
    struct ReadableStreamImpl {
        virtual ~ReadableStreamImpl() {}
        virtual void read(char *buffer, size_t count) = 0;
        virtual bool eof() = 0;
    };

    template <class T>
    struct ReadableStreamT : public ReadableStreamImpl {
        ReadableStreamT(T &readable) : self{ readable } {}
        virtual ~ReadableStreamT() {}
        void read(char *buffer, size_t count) {
            self.read(buffer, count);
        }
        bool eof() {
            return self.eof();
        }
        T &self;
    };
    ReadableStreamImpl *reader_impl;
};

// A wrapper, which we can use wrap any object that has a write(...) method
class WritableStream {
public:
    void write(const char *buffer, size_t count) {
        writer_impl->write(buffer, count);
    }

    bool bad() {
        return writer_impl->bad();
    }

    template <class HAS_WRITE>
    WritableStream(HAS_WRITE &has_write) {
        writer_impl = new WritableStreamT<HAS_WRITE>(has_write);
    }

    virtual ~WritableStream() {
        delete writer_impl;
    }

private:
    struct WritableStreamImpl {
        virtual ~WritableStreamImpl() {}
        virtual void write(const char *buffer, size_t count) = 0;
        virtual bool bad() = 0;
    };

    template <class T>
    struct WritableStreamT : public WritableStreamImpl {
        WritableStreamT(T &writable) : self{ writable } {}

        virtual ~WritableStreamT() {}
        void write(const char *buffer, size_t count) {
            self.write(buffer, count);
        }
        bool bad() {
            return self.bad();
        }
        T &self;
    };

    WritableStreamImpl *writer_impl;
};

// serialize Acquisition to ostream
EXPORTISMRMRD void serialize(const Acquisition &acq, WritableStream &ws);

// serialize Image<T> to ostream
template <typename T>
EXPORTISMRMRD void serialize(const Image<T> &img, WritableStream &ws);

// serialize Waveform to ostream
EXPORTISMRMRD void serialize(const Waveform &wfm, WritableStream &ws);

// deserialize Acquisition from istream
EXPORTISMRMRD void deserialize(Acquisition &acq, ReadableStream &rs);

// deserialize Image<T> from istream
template <typename T>
EXPORTISMRMRD void deserialize(Image<T> &img, ReadableStream &rs);

// deserialize Waveform from istream
EXPORTISMRMRD void deserialize(Waveform &wfm, ReadableStream &rs);

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
