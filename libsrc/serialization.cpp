#include <sstream>
#include <string>

#include "ismrmrd/serialization.h"
#include "ismrmrd/xml.h"

namespace ISMRMRD {

void serialize(const Acquisition &acq, WritableStreamView &ws) {
    AcquisitionHeader ahead = acq.getHead();
    ws.write(reinterpret_cast<const char *>(&ahead), sizeof(AcquisitionHeader));
    ws.write(reinterpret_cast<const char *>(acq.getTrajPtr()), ahead.trajectory_dimensions * ahead.number_of_samples * sizeof(float));
    ws.write(reinterpret_cast<const char *>(acq.getDataPtr()), ahead.number_of_samples * ahead.active_channels * 2 * sizeof(float));
    if (ws.bad()) {
        throw std::runtime_error("Error writing acquisition to stream");
    }
}

template <typename T>
void serialize(const Image<T> &img, WritableStreamView &ws) {
    ImageHeader ihead = img.getHead();
    if (ismrmrd_sizeof_data_type(ihead.data_type) != sizeof(T)) {
        throw std::runtime_error("Image data type does not match template type");
    }
    ws.write(reinterpret_cast<const char *>(&ihead), sizeof(ImageHeader));
    uint64_t attr_length = img.getAttributeStringLength();
    ws.write(reinterpret_cast<char *>(&attr_length), sizeof(uint64_t));
    if (attr_length) {
        ws.write(img.getAttributeString(), ihead.attribute_string_len);
    }
    ws.write(reinterpret_cast<const char *>(img.getDataPtr()), img.getDataSize());
    if (ws.bad()) {
        throw std::runtime_error("Error writing image to stream");
    }
}

void serialize(const Waveform &wfm, WritableStreamView &ws) {
    ws.write(reinterpret_cast<const char *>(&wfm.head), sizeof(ISMRMRD_WaveformHeader));
    ws.write(reinterpret_cast<const char *>(wfm.begin_data()), wfm.head.number_of_samples * wfm.head.channels * sizeof(uint32_t));
    if (ws.bad()) {
        throw std::runtime_error("Error writing waveform to stream");
    }
}

void serialize(const ConfigFile &cfg, WritableStreamView &ws) {
    ws.write(cfg.config, sizeof(cfg.config));
    if (ws.bad()) {
        throw std::runtime_error("Error writing fixed length char array to stream");
    }
}

void serialize(const std::string &str, WritableStreamView &ws) {
    uint32_t len = static_cast<uint32_t>(str.length());
    ws.write(reinterpret_cast<char *>(&len), sizeof(uint32_t));
    ws.write(str.c_str(), len);
    if (ws.bad()) {
        throw std::runtime_error("Error writing string to stream");
    }
}

void deserialize(Acquisition &acq, ReadableStreamView &rs) {
    AcquisitionHeader ahead;
    rs.read(reinterpret_cast<char *>(&ahead), sizeof(AcquisitionHeader));
    acq.setHead(ahead);
    rs.read(reinterpret_cast<char *>(acq.getTrajPtr()), ahead.trajectory_dimensions * ahead.number_of_samples * sizeof(float));
    rs.read(reinterpret_cast<char *>(acq.getDataPtr()), ahead.number_of_samples * ahead.active_channels * 2 * sizeof(float));
    if (rs.eof()) {
        throw std::runtime_error("Error reading acquisition");
    }
}

// Helper function that deserializes attributes and pixels
template <typename T>
void deserialize_attr_and_pixels(Image<T> &img, ReadableStreamView &rs) {
    uint64_t attr_length;
    rs.read(reinterpret_cast<char *>(&attr_length), sizeof(uint64_t));
    if (attr_length) {
        std::vector<char> attr(attr_length + 1);
        rs.read(&attr[0], attr_length);
        attr[attr_length] = '\0';
        img.setAttributeString(&attr[0]);
    }
    rs.read(reinterpret_cast<char *>(img.getDataPtr()), img.getDataSize());
    if (rs.eof()) {
        throw std::runtime_error("Error reading image");
    }
}

template <typename T>
void deserialize(Image<T> &img, ReadableStreamView &rs) {
    ImageHeader ihead;
    rs.read(reinterpret_cast<char *>(&ihead), sizeof(ImageHeader));
    if (rs.eof()) {
        throw std::runtime_error("Error reading image header");
    }
    if (ismrmrd_sizeof_data_type(ihead.data_type) != sizeof(T)) {
        throw std::runtime_error("Image data type does not match template type");
    }
    img.setHead(ihead);
    deserialize_attr_and_pixels(img, rs);
}

void deserialize(Waveform &wfm, ReadableStreamView &rs) {
#if __cplusplus > 199711L
    static_assert(std::is_same<decltype(wfm.head), ISMRMRD_WaveformHeader>::value, "Waveform header type mismatch");
#endif
    rs.read(reinterpret_cast<char *>(&wfm.head), sizeof(ISMRMRD_WaveformHeader));
    ismrmrd_make_consistent_waveform(&wfm);
    rs.read(reinterpret_cast<char *>(wfm.begin_data()), wfm.head.number_of_samples * wfm.head.channels * sizeof(uint32_t));
    if (rs.eof()) {
        throw std::runtime_error("Error reading waveform");
    }
}

void deserialize(ConfigFile &cfg, ReadableStreamView &rs) {
    rs.read(cfg.config, sizeof(cfg.config));
    if (rs.eof()) {
        throw std::runtime_error("Error reading fixed length char array");
    }
}

void deserialize(std::string &str, ReadableStreamView &rs) {
    uint32_t len;
    rs.read(reinterpret_cast<char *>(&len), sizeof(uint32_t));
    std::vector<char> buf(len);
    rs.read(&buf[0], len);
    if (rs.eof()) {
        throw std::runtime_error("Error reading string");
    }
    // Past implementations would serialize strings with the null terminator added at the end.
    // We remove that here.
    if (buf[len - 1] == '\0') {
        len--;
    }
    str.assign(&buf[0], len);
}

ProtocolSerializer::ProtocolSerializer(WritableStreamView &ws) : _ws(ws) {}

void ProtocolSerializer::write_msg_id(uint16_t id) {
    _ws.write(reinterpret_cast<const char *>(&id), sizeof(uint16_t));
}

void ProtocolSerializer::serialize(const ConfigFile &cf) {
    write_msg_id(ISMRMRD_MESSAGE_CONFIG_FILE);
    ISMRMRD::serialize(cf, _ws);
}

void ProtocolSerializer::serialize(const ConfigText &ct) {
    write_msg_id(ISMRMRD_MESSAGE_CONFIG_TEXT);
    ISMRMRD::serialize(ct.config_text, _ws);
}

void ProtocolSerializer::serialize(const TextMessage &tm) {
    write_msg_id(ISMRMRD_MESSAGE_TEXT);
    ISMRMRD::serialize(tm.message, _ws);
}

void ProtocolSerializer::serialize(const IsmrmrdHeader &hdr) {
    std::stringstream str;
    ISMRMRD::serialize(hdr, str);
    auto as_str = str.str();
    uint32_t size = static_cast<uint32_t>(as_str.size());
    write_msg_id(ISMRMRD_MESSAGE_HEADER);
    _ws.write(reinterpret_cast<const char *>(&size), sizeof(uint32_t));
    _ws.write(as_str.c_str(), as_str.size());
    if (_ws.bad()) {
        throw std::runtime_error("Error writing header to stream");
    }
}

void ProtocolSerializer::serialize(const Acquisition &acq) {
    write_msg_id(ISMRMRD_MESSAGE_ACQUISITION);
    ISMRMRD::serialize(acq, _ws);
}

template <typename T>
void ProtocolSerializer::serialize(const Image<T> &img) {
    write_msg_id(ISMRMRD_MESSAGE_IMAGE);
    ISMRMRD::serialize(img, _ws);
}

void ProtocolSerializer::serialize(const Waveform &wfm) {
    write_msg_id(ISMRMRD_MESSAGE_WAVEFORM);
    ISMRMRD::serialize(wfm, _ws);
}

void ProtocolSerializer::close() {
    write_msg_id(ISMRMRD_MESSAGE_CLOSE);
}

ProtocolDeserializer::ProtocolDeserializer(ReadableStreamView &rs) : _rs(rs), _peeked(ISMRMRD_MESSAGE_UNPEEKED) {}

uint16_t ProtocolDeserializer::peek() {
    if (_peeked == ISMRMRD_MESSAGE_UNPEEKED) {
        _rs.read(reinterpret_cast<char *>(&_peeked), sizeof(uint16_t));
        if (_peeked == ISMRMRD_MESSAGE_IMAGE) {
            _rs.read(reinterpret_cast<char *>(&_peeked_image_header), sizeof(ImageHeader));
        }
        if (_rs.eof()) {
            throw std::runtime_error("Error reading message ID");
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

void ProtocolDeserializer::deserialize(ConfigFile &cf) {
    if (peek() != ISMRMRD_MESSAGE_CONFIG_FILE) {
        throw std::runtime_error("Expected config file message");
    }
    ISMRMRD::deserialize(cf, _rs);
    _peeked = ISMRMRD_MESSAGE_UNPEEKED;
}

void ProtocolDeserializer::deserialize(ConfigText &ct) {
    if (peek() != ISMRMRD_MESSAGE_CONFIG_TEXT) {
        throw std::runtime_error("Expected config text message");
    }
    ISMRMRD::deserialize(ct.config_text, _rs);
    _peeked = ISMRMRD_MESSAGE_UNPEEKED;
}

void ProtocolDeserializer::deserialize(TextMessage &tm) {
    if (peek() != ISMRMRD_MESSAGE_TEXT) {
        throw std::runtime_error("Expected text message");
    }
    ISMRMRD::deserialize(tm.message, _rs);
    _peeked = ISMRMRD_MESSAGE_UNPEEKED;
}

void ProtocolDeserializer::deserialize(IsmrmrdHeader &hdr) {
    if (peek() == ISMRMRD_MESSAGE_CLOSE) {
        throw ProtocolStreamClosed();
    }
    if (peek() != ISMRMRD_MESSAGE_HEADER) {
        throw std::runtime_error("Expected ISMRMRD_MESSAGE_HEADER");
    }
    uint32_t size;
    _rs.read(reinterpret_cast<char *>(&size), sizeof(uint32_t));
    std::string str(size, '\0');
    _rs.read(&str[0], size);
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
    ISMRMRD::deserialize(acq, _rs);
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
    deserialize_attr_and_pixels(img, _rs);
    _peeked = ISMRMRD_MESSAGE_UNPEEKED;
}

void ProtocolDeserializer::deserialize(Waveform &wfm) {
    if (peek() == ISMRMRD_MESSAGE_CLOSE) {
        throw ProtocolStreamClosed();
    }
    if (peek() != ISMRMRD_MESSAGE_WAVEFORM) {
        throw std::runtime_error("Expected ISMRMRD_MESSAGE_WAVEFORM");
    }
    ISMRMRD::deserialize(wfm, _rs);
    _peeked = ISMRMRD_MESSAGE_UNPEEKED;
}

// template instantiations
template EXPORTISMRMRD void serialize(const Image<unsigned short> &img, WritableStreamView &ws);
template EXPORTISMRMRD void serialize(const Image<unsigned int> &img, WritableStreamView &ws);
template EXPORTISMRMRD void serialize(const Image<short> &img, WritableStreamView &ws);
template EXPORTISMRMRD void serialize(const Image<int> &img, WritableStreamView &ws);
template EXPORTISMRMRD void serialize(const Image<float> &img, WritableStreamView &ws);
template EXPORTISMRMRD void serialize(const Image<double> &img, WritableStreamView &ws);
template EXPORTISMRMRD void serialize(const Image<std::complex<float> > &img, WritableStreamView &ws);
template EXPORTISMRMRD void serialize(const Image<std::complex<double> > &img, WritableStreamView &ws);
template EXPORTISMRMRD void deserialize(Image<unsigned short> &img, ReadableStreamView &rs);
template EXPORTISMRMRD void deserialize(Image<unsigned int> &img, ReadableStreamView &rs);
template EXPORTISMRMRD void deserialize(Image<short> &img, ReadableStreamView &rs);
template EXPORTISMRMRD void deserialize(Image<int> &img, ReadableStreamView &rs);
template EXPORTISMRMRD void deserialize(Image<float> &img, ReadableStreamView &rs);
template EXPORTISMRMRD void deserialize(Image<double> &img, ReadableStreamView &rs);
template EXPORTISMRMRD void deserialize(Image<std::complex<float> > &img, ReadableStreamView &rs);
template EXPORTISMRMRD void deserialize(Image<std::complex<double> > &img, ReadableStreamView &rs);
template EXPORTISMRMRD void ProtocolSerializer::serialize(const Image<unsigned short> &img);
template EXPORTISMRMRD void ProtocolSerializer::serialize(const Image<unsigned int> &img);
template EXPORTISMRMRD void ProtocolSerializer::serialize(const Image<short> &img);
template EXPORTISMRMRD void ProtocolSerializer::serialize(const Image<int> &img);
template EXPORTISMRMRD void ProtocolSerializer::serialize(const Image<float> &img);
template EXPORTISMRMRD void ProtocolSerializer::serialize(const Image<double> &img);
template EXPORTISMRMRD void ProtocolSerializer::serialize(const Image<std::complex<float> > &img);
template EXPORTISMRMRD void ProtocolSerializer::serialize(const Image<std::complex<double> > &img);
template EXPORTISMRMRD void ProtocolDeserializer::deserialize(Image<unsigned short> &img);
template EXPORTISMRMRD void ProtocolDeserializer::deserialize(Image<unsigned int> &img);
template EXPORTISMRMRD void ProtocolDeserializer::deserialize(Image<short> &img);
template EXPORTISMRMRD void ProtocolDeserializer::deserialize(Image<int> &img);
template EXPORTISMRMRD void ProtocolDeserializer::deserialize(Image<float> &img);
template EXPORTISMRMRD void ProtocolDeserializer::deserialize(Image<double> &img);
template EXPORTISMRMRD void ProtocolDeserializer::deserialize(Image<std::complex<float> > &img);
template EXPORTISMRMRD void ProtocolDeserializer::deserialize(Image<std::complex<double> > &img);

} // namespace ISMRMRD
