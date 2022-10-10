#ifndef ISMRMRDSERIALIZATION_H
#define ISMRMRDSERIALIZATION_H

#include <exception>
#include <functional>
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

// In the following the HAS_WRITE type is a type like ostream,
// which implements ::write(const char* buffer, size_t n)
// Similarly HAS_READ is a type that implements ::read(char* buf, size_t n)

// serialize Acquisition to ostream
template <typename HAS_WRITE>
void serialize(const Acquisition &acq, HAS_WRITE &os) {
    AcquisitionHeader ahead = acq.getHead();
    os.write(reinterpret_cast<const char *>(&ahead), sizeof(AcquisitionHeader));
    os.write(reinterpret_cast<const char *>(acq.getTrajPtr()), ahead.trajectory_dimensions * ahead.number_of_samples * sizeof(float));
    os.write(reinterpret_cast<const char *>(acq.getDataPtr()), ahead.number_of_samples * ahead.active_channels * 2 * sizeof(float));
}

// serialize Image<T> to ostream
template <typename T, typename HAS_WRITE>
void EXPORTISMRMRD serialize(const Image<T> &img, HAS_WRITE &os) {
    ImageHeader ihead = img.getHead();
    if (ismrmrd_sizeof_data_type(ihead.data_type) != sizeof(T)) {
        throw std::runtime_error("Image data type does not match template type");
    }
    os.write(reinterpret_cast<const char *>(&ihead), sizeof(ImageHeader));
    uint64_t attr_length = img.getAttributeStringLength();
    os.write(reinterpret_cast<char *>(&attr_length), sizeof(uint64_t));
    if (attr_length) {
        os.write(img.getAttributeString(), ihead.attribute_string_len);
    }
    os.write(reinterpret_cast<const char *>(img.getDataPtr()), img.getDataSize());
}

// serialize Waveform to ostream
template <typename HAS_WRITE>
void serialize(const Waveform &wfm, HAS_WRITE &os) {
    os.write(reinterpret_cast<const char *>(&wfm.head), sizeof(ISMRMRD_WaveformHeader));
    os.write(reinterpret_cast<const char *>(wfm.begin_data()), wfm.head.number_of_samples * wfm.head.channels * sizeof(uint32_t));
}

// deserialize Acquisition from istream
template <typename HAS_READ>
void deserialize(Acquisition &acq, HAS_READ &is) {
    AcquisitionHeader ahead;
    is.read(reinterpret_cast<char *>(&ahead), sizeof(AcquisitionHeader));
    acq.setHead(ahead);
    is.read(reinterpret_cast<char *>(acq.getTrajPtr()), ahead.trajectory_dimensions * ahead.number_of_samples * sizeof(float));
    is.read(reinterpret_cast<char *>(acq.getDataPtr()), ahead.number_of_samples * ahead.active_channels * 2 * sizeof(float));
}

// deserialize Image<T> from istream
// Helper function that deserializes attributes and pixels
template <typename T, typename HAS_READ>
void deserialize_attr_and_pixels(Image<T> &img, HAS_READ &is) {
    uint64_t attr_length;
    is.read(reinterpret_cast<char *>(&attr_length), sizeof(uint64_t));
    if (attr_length) {
        char *attr = new char[attr_length];
        is.read(attr, attr_length);
        img.setAttributeString(attr);
        delete[] attr;
    }
    is.read(reinterpret_cast<char *>(img.getDataPtr()), img.getDataSize());
}

template <typename T, typename HAS_READ>
void EXPORTISMRMRD deserialize(Image<T> &img, HAS_READ &is) {
    ImageHeader ihead;
    is.read(reinterpret_cast<char *>(&ihead), sizeof(ImageHeader));
    if (ismrmrd_sizeof_data_type(ihead.data_type) != sizeof(T)) {
        throw std::runtime_error("Image data type does not match template type");
    }
    img.setHead(ihead);
    deserialize_attr_and_pixels(img, is);
}

// deserialize Waveform from istream
template <typename HAS_READ>
void deserialize(Waveform &wfm, HAS_READ &is) {
    is.read(reinterpret_cast<char *>(&wfm.head), sizeof(ISMRMRD_WaveformHeader));
    ismrmrd_make_consistent_waveform(&wfm);
    is.read(reinterpret_cast<char *>(wfm.begin_data()), wfm.head.number_of_samples * wfm.head.channels * sizeof(uint32_t));
}

class ProtocolStreamClosed : public std::exception {};

template <typename HAS_WRITE>
class ProtocolSerializer {
public:
    ProtocolSerializer(HAS_WRITE &os) : _os(os) {}

    void serialize(const IsmrmrdHeader &hdr) {
        std::stringstream str;
        ISMRMRD::serialize(hdr, str);
        auto as_str = str.str();
        uint32_t size = as_str.size();
        write_msg_id(ISMRMRD_MESSAGE_HEADER);
        _os.write(reinterpret_cast<const char *>(&size), sizeof(uint32_t));
        _os.write(as_str.c_str(), as_str.size());
    }

    void serialize(const Acquisition &acq) {
        write_msg_id(ISMRMRD_MESSAGE_ACQUISITION);
        ISMRMRD::serialize(acq, _os);
    }

    template <typename T>
    void serialize(const Image<T> &img) {
        write_msg_id(ISMRMRD_MESSAGE_IMAGE);
        ISMRMRD::serialize(img, _os);
    }

    void serialize(const Waveform &wfm) {
        write_msg_id(ISMRMRD_MESSAGE_WAVEFORM);
        ISMRMRD::serialize(wfm, _os);
    }

    void close() {
        write_msg_id(ISMRMRD_MESSAGE_CLOSE);
    }

protected:
    void write_msg_id(uint16_t id) {
        _os.write(reinterpret_cast<const char *>(&id), sizeof(uint16_t));
    }

    HAS_WRITE &_os;
};

template <typename HAS_READ>
class EXPORTISMRMRD ProtocolDeserializer {
public:
    ProtocolDeserializer(HAS_READ &is) : _is(is), _peeked(ISMRMRD_MESSAGE_UNPEEKED){};

    void deserialize(IsmrmrdHeader &hdr) {
        if (peek() == ISMRMRD_MESSAGE_CLOSE) {
            throw ProtocolStreamClosed();
        }
        if (peek() != ISMRMRD_MESSAGE_HEADER) {
            throw std::runtime_error("Expected ISMRMRD_MESSAGE_HEADER");
        }
        uint32_t size;
        _is.read(reinterpret_cast<char *>(&size), sizeof(uint32_t));
        std::string str(size, '\0');
        _is.read(&str[0], size);
        ISMRMRD::deserialize(str.c_str(), hdr);
        _peeked = ISMRMRD_MESSAGE_UNPEEKED;
    }

    void deserialize(Acquisition &acq) {
        if (peek() == ISMRMRD_MESSAGE_CLOSE) {
            throw ProtocolStreamClosed();
        }
        if (peek() != ISMRMRD_MESSAGE_ACQUISITION) {
            throw std::runtime_error("Expected ISMRMRD_MESSAGE_ACQUISITION");
        }
        ISMRMRD::deserialize(acq, _is);
        _peeked = ISMRMRD_MESSAGE_UNPEEKED;
    }

    template <typename T>
    void deserialize(Image<T> &img) {
        if (peek() == ISMRMRD_MESSAGE_CLOSE) {
            throw ProtocolStreamClosed();
        }
        if (peek() != ISMRMRD_MESSAGE_IMAGE) {
            throw std::runtime_error("Expected ISMRMRD_MESSAGE_IMAGE");
        }
        img.setHead(_peeked_image_header);
        deserialize_attr_and_pixels(img, _is);
        _peeked = ISMRMRD_MESSAGE_UNPEEKED;
    }

    void deserialize(Waveform &wfm) {
        if (peek() == ISMRMRD_MESSAGE_CLOSE) {
            throw ProtocolStreamClosed();
        }
        if (peek() != ISMRMRD_MESSAGE_WAVEFORM) {
            throw std::runtime_error("Expected ISMRMRD_MESSAGE_WAVEFORM");
        }
        ISMRMRD::deserialize(wfm, _is);
        _peeked = ISMRMRD_MESSAGE_UNPEEKED;
    }

    // Peek at the next data type in the stream
    uint16_t peek() {
        if (_peeked == ISMRMRD_MESSAGE_UNPEEKED) {
            _is.read(reinterpret_cast<char *>(&_peeked), sizeof(uint16_t));
            if (_peeked == ISMRMRD_MESSAGE_IMAGE) {
                _is.read(reinterpret_cast<char *>(&_peeked_image_header), sizeof(ImageHeader));
            }
        }
        return _peeked;
    }

    int peek_image_data_type() {
        if (_peeked == ISMRMRD_MESSAGE_IMAGE) {
            return _peeked_image_header.data_type;
        } else {
            throw std::runtime_error("Cannot peak image data type if not peeking an image");
        }
    }

protected:
    HAS_READ &_is;
    uint16_t _peeked;
    ImageHeader _peeked_image_header;
};

} // namespace ISMRMRD

#endif // ISMRMRDSERIALIZATION_H
