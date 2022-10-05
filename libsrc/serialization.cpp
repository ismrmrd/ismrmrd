#include "ismrmrd/serialization.h"

namespace ISMRMRD {

const uint16_t ISMRMRD_MESSAGE_ACQUISITION = 1008;
const uint16_t ISMRMRD_MESSAGE_CLOSE = 4;
const uint16_t ISMRMRD_MESSAGE_IMAGE = 1022;
const uint16_t ISMRMRD_MESSAGE_WAVEFORM = 1026;

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

ProtocolSerializer::ProtocolSerializer(std::ostream &os) : os(os) {}

void ProtocolSerializer::write_msg_id(uint16_t id) {
    os.write(reinterpret_cast<const char *>(&id), sizeof(uint16_t));
}

void ProtocolSerializer::serialize(const Acquisition &acq) {
    write_msg_id(ISMRMRD_MESSAGE_ACQUISITION);
    ISMRMRD::serialize(acq, os);
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
