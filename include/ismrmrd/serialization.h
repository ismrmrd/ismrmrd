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
    ISMRMRD_MESSAGE_HEADER = 3,
    ISMRMRD_MESSAGE_CLOSE = 4,
    ISMRMRD_MESSAGE_ACQUISITION = 1008,
    ISMRMRD_MESSAGE_IMAGE = 1022,
    ISMRMRD_MESSAGE_WAVEFORM = 1026
};

// A wrapper, which we can use wrap any object that has a read(...) method
class ReadableStreamView {
public:
    void read(char *buffer, size_t count) {
        reader_impl->read(buffer, count);
    }

    bool eof() {
        return reader_impl->eof();
    }

    template <class HAS_READ>
    ReadableStreamView(HAS_READ &has_read) {
        reader_impl = new ReadableStreamViewT<HAS_READ>(has_read);
    }

    virtual ~ReadableStreamView() {
        delete reader_impl;
    }

private:
    struct ReadableStreamViewImpl {
        virtual ~ReadableStreamViewImpl() {}
        virtual void read(char *buffer, size_t count) = 0;
        virtual bool eof() = 0;
    };

    template <class T>
    struct ReadableStreamViewT : public ReadableStreamViewImpl {
        ReadableStreamViewT(T &readable) : self{ readable } {}
        virtual ~ReadableStreamViewT() {}
        void read(char *buffer, size_t count) {
            self.read(buffer, count);
        }
        bool eof() {
            return self.eof();
        }
        T &self;
    };
    ReadableStreamViewImpl *reader_impl;
};

// A wrapper, which we can use wrap any object that has a write(...) method
class WritableStreamView {
public:
    void write(const char *buffer, size_t count) {
        writer_impl->write(buffer, count);
    }

    bool bad() {
        return writer_impl->bad();
    }

    template <class HAS_WRITE>
    WritableStreamView(HAS_WRITE &has_write) {
        writer_impl = new WritableStreamViewT<HAS_WRITE>(has_write);
    }

    virtual ~WritableStreamView() {
        delete writer_impl;
    }

private:
    struct WritableStreamViewImpl {
        virtual ~WritableStreamViewImpl() {}
        virtual void write(const char *buffer, size_t count) = 0;
        virtual bool bad() = 0;
    };

    template <class T>
    struct WritableStreamViewT : public WritableStreamViewImpl {
        WritableStreamViewT(T &writable) : self{ writable } {}

        virtual ~WritableStreamViewT() {}
        void write(const char *buffer, size_t count) {
            self.write(buffer, count);
        }
        bool bad() {
            return self.bad();
        }
        T &self;
    };

    WritableStreamViewImpl *writer_impl;
};

// serialize Acquisition to ostream
EXPORTISMRMRD void serialize(const Acquisition &acq, WritableStreamView &ws);

// serialize Image<T> to ostream
template <typename T>
EXPORTISMRMRD void serialize(const Image<T> &img, WritableStreamView &ws);

// serialize Waveform to ostream
EXPORTISMRMRD void serialize(const Waveform &wfm, WritableStreamView &ws);

// deserialize Acquisition from istream
EXPORTISMRMRD void deserialize(Acquisition &acq, ReadableStreamView &rs);

// deserialize Image<T> from istream
template <typename T>
EXPORTISMRMRD void deserialize(Image<T> &img, ReadableStreamView &rs);

// deserialize Waveform from istream
EXPORTISMRMRD void deserialize(Waveform &wfm, ReadableStreamView &rs);

class ProtocolStreamClosed : public std::exception {};

class EXPORTISMRMRD ProtocolSerializer {
public:
    ProtocolSerializer(WritableStreamView &ws);
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
