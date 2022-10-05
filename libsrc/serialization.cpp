#include <sstream>
#include <string>

#include "ismrmrd/serialization.h"

namespace ISMRMRD {

void serialize(const Acquisition &acq, std::ostream &os) {
    AcquisitionHeader ahead = acq.getHead();
    os.write(reinterpret_cast<const char *>(&ahead), sizeof(AcquisitionHeader));
    os.write(reinterpret_cast<const char *>(acq.getTrajPtr()), ahead.trajectory_dimensions * ahead.number_of_samples * sizeof(float));
    os.write(reinterpret_cast<const char *>(acq.getDataPtr()), ahead.number_of_samples * ahead.active_channels * 2 * sizeof(float));
}

template <typename T>
void serialize(const Image<T> &img, std::ostream &os) {
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
    os.write(reinterpret_cast<const char *>(img.getDataPtr()), ihead.matrix_size[0] * ihead.matrix_size[1] * ihead.matrix_size[2] * ihead.channels * img.getDataSize());
}

void serialize(const Waveform &wfm, std::ostream &os) {
    os.write(reinterpret_cast<const char *>(&wfm.head), sizeof(ISMRMRD_WaveformHeader));
    os.write(reinterpret_cast<const char *>(wfm.begin_data()), wfm.head.number_of_samples * wfm.head.channels * sizeof(uint32_t));
}

void deserialize(Acquisition &acq, std::istream &is) {
    AcquisitionHeader ahead;
    is.read(reinterpret_cast<char *>(&ahead), sizeof(AcquisitionHeader));
    acq.setHead(ahead);
    is.read(reinterpret_cast<char *>(acq.getTrajPtr()), ahead.trajectory_dimensions * ahead.number_of_samples * sizeof(float));
    is.read(reinterpret_cast<char *>(acq.getDataPtr()), ahead.number_of_samples * ahead.active_channels * 2 * sizeof(float));
}

template <typename T>
void deserialize(Image<T> &img, std::istream &is) {
    ImageHeader ihead;
    is.read(reinterpret_cast<char *>(&ihead), sizeof(ImageHeader));
    if (ismrmrd_sizeof_data_type(ihead.data_type) != sizeof(T)) {
        throw std::runtime_error("Image data type does not match template type");
    }
    img.setHead(ihead);
    uint64_t attr_length;
    is.read(reinterpret_cast<char *>(&attr_length), sizeof(uint64_t));
    if (attr_length) {
        char *attr = new char[attr_length];
        is.read(attr, attr_length);
        img.setAttributeString(attr);
        delete[] attr;
    }
    is.read(reinterpret_cast<char *>(img.getDataPtr()), ihead.matrix_size[0] * ihead.matrix_size[1] * ihead.matrix_size[2] * ihead.channels * img.getDataSize());
}

void deserialize(Waveform &wfm, std::istream &is) {
    is.read(reinterpret_cast<char *>(&wfm.head), sizeof(ISMRMRD_WaveformHeader));
    is.read(reinterpret_cast<char *>(wfm.begin_data()), wfm.head.number_of_samples * wfm.head.channels * sizeof(uint32_t));
}

ProtocolSerializer::ProtocolSerializer(std::ostream &os) : _os(os) {}

void ProtocolSerializer::write_msg_id(uint16_t id) {
    _os.write(reinterpret_cast<const char *>(&id), sizeof(uint16_t));
}

void ProtocolSerializer::serialize(const IsmrmrdHeader &hdr) {
    std::stringstream str;
    ISMRMRD::serialize(hdr, str);
    auto as_str = str.str();
    uint32_t size = as_str.size();
    write_msg_id(ISMRMRD_MESSAGE_HEADER);
    _os.write(reinterpret_cast<const char *>(&size), sizeof(uint32_t));
    _os.write(as_str.c_str(), as_str.size());
}

void ProtocolSerializer::serialize(const Acquisition &acq) {
    write_msg_id(ISMRMRD_MESSAGE_ACQUISITION);
    ISMRMRD::serialize(acq, _os);
}

template <typename T>
void ProtocolSerializer::serialize(const Image<T> &img) {
    write_msg_id(ISMRMRD_MESSAGE_IMAGE);
    ISMRMRD::serialize(img, _os);
}

void ProtocolSerializer::serialize(const Waveform &wfm) {
    write_msg_id(ISMRMRD_MESSAGE_WAVEFORM);
    ISMRMRD::serialize(wfm, _os);
}

void ProtocolSerializer::close() {
    write_msg_id(ISMRMRD_MESSAGE_CLOSE);
}

ProtocolDeserializer::ProtocolDeserializer(std::istream &is) : _is(is), _peeked(ISMRMRD_MESSAGE_UNPEEKED) {}

uint16_t ProtocolDeserializer::peek() {
    if (_peeked == ISMRMRD_MESSAGE_UNPEEKED) {
        _is.read(reinterpret_cast<char *>(&_peeked), sizeof(uint16_t));
        if (_peeked == ISMRMRD_MESSAGE_IMAGE) {
            _is.read(reinterpret_cast<char *>(&_peeked_image_header), sizeof(ImageHeader));
        }
    }
    return _peeked;
}

int ProtocolDeserializer::peek_image_data_type() {
    if (_peeked == ISMRMRD_MESSAGE_IMAGE) {
        return _peeked_image_header.data_type;
    } else {
        throw std::runtime_error("Cannot peak image data type if not peeking an image");
    }
}

void ProtocolDeserializer::deserialize(IsmrmrdHeader &hdr) {
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

void ProtocolDeserializer::deserialize(Acquisition &acq) {
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
void ProtocolDeserializer::deserialize(Image<T> &img) {
    if (peek() == ISMRMRD_MESSAGE_CLOSE) {
        throw ProtocolStreamClosed();
    }
    if (peek() != ISMRMRD_MESSAGE_IMAGE) {
        throw std::runtime_error("Expected ISMRMRD_MESSAGE_IMAGE");
    }
    img.setHead(_peeked_image_header);
    uint64_t attr_length;
    _is.read(reinterpret_cast<char *>(&attr_length), sizeof(uint64_t));
    if (attr_length) {
        char *attr = new char[attr_length];
        _is.read(attr, attr_length);
        img.setAttributeString(attr);
        delete[] attr;
    }
    _is.read(reinterpret_cast<char *>(img.getDataPtr()), _peeked_image_header.matrix_size[0] * _peeked_image_header.matrix_size[1] * _peeked_image_header.matrix_size[2] * _peeked_image_header.channels * img.getDataSize());
    _peeked = ISMRMRD_MESSAGE_UNPEEKED;
}

void ProtocolDeserializer::deserialize(Waveform &wfm) {
    if (peek() == ISMRMRD_MESSAGE_CLOSE) {
        throw ProtocolStreamClosed();
    }
    if (peek() != ISMRMRD_MESSAGE_WAVEFORM) {
        throw std::runtime_error("Expected ISMRMRD_MESSAGE_WAVEFORM");
    }
    ISMRMRD::deserialize(wfm, _is);
    _peeked = ISMRMRD_MESSAGE_UNPEEKED;
}

} // namespace ISMRMRD

// template instanciations
template void ISMRMRD::serialize(const Image<unsigned short> &img, std::ostream &os);
template void ISMRMRD::serialize(const Image<unsigned int> &img, std::ostream &os);
template void ISMRMRD::serialize(const Image<short> &img, std::ostream &os);
template void ISMRMRD::serialize(const Image<int> &img, std::ostream &os);
template void ISMRMRD::serialize(const Image<float> &img, std::ostream &os);
template void ISMRMRD::serialize(const Image<double> &img, std::ostream &os);
template void ISMRMRD::serialize(const Image<std::complex<float> > &img, std::ostream &os);
template void ISMRMRD::serialize(const Image<std::complex<double> > &img, std::ostream &os);
template void ISMRMRD::deserialize(Image<unsigned short> &img, std::istream &is);
template void ISMRMRD::deserialize(Image<unsigned int> &img, std::istream &is);
template void ISMRMRD::deserialize(Image<short> &img, std::istream &is);
template void ISMRMRD::deserialize(Image<int> &img, std::istream &is);
template void ISMRMRD::deserialize(Image<float> &img, std::istream &is);
template void ISMRMRD::deserialize(Image<double> &img, std::istream &is);
template void ISMRMRD::deserialize(Image<std::complex<float> > &img, std::istream &is);
template void ISMRMRD::deserialize(Image<std::complex<double> > &img, std::istream &is);
template void ISMRMRD::ProtocolSerializer::serialize(const Image<unsigned short> &img);
template void ISMRMRD::ProtocolSerializer::serialize(const Image<unsigned int> &img);
template void ISMRMRD::ProtocolSerializer::serialize(const Image<short> &img);
template void ISMRMRD::ProtocolSerializer::serialize(const Image<int> &img);
template void ISMRMRD::ProtocolSerializer::serialize(const Image<float> &img);
template void ISMRMRD::ProtocolSerializer::serialize(const Image<double> &img);
template void ISMRMRD::ProtocolSerializer::serialize(const Image<std::complex<float> > &img);
template void ISMRMRD::ProtocolSerializer::serialize(const Image<std::complex<double> > &img);
template void ISMRMRD::ProtocolDeserializer::deserialize(Image<unsigned short> &img);
template void ISMRMRD::ProtocolDeserializer::deserialize(Image<unsigned int> &img);
template void ISMRMRD::ProtocolDeserializer::deserialize(Image<short> &img);
template void ISMRMRD::ProtocolDeserializer::deserialize(Image<int> &img);
template void ISMRMRD::ProtocolDeserializer::deserialize(Image<float> &img);
template void ISMRMRD::ProtocolDeserializer::deserialize(Image<double> &img);
template void ISMRMRD::ProtocolDeserializer::deserialize(Image<std::complex<float> > &img);
template void ISMRMRD::ProtocolDeserializer::deserialize(Image<std::complex<double> > &img);
