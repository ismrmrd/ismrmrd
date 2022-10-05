#include "ismrmrd/dataset.h"
#include "ismrmrd/serialization.h"
#include <iostream>

template <typename T>
std::string create_image_series_name(const ISMRMRD::Image<T> &img) {
    std::stringstream ss;
    ss << "image_" << img.getHead().image_series_index;
    return ss.str();
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    ISMRMRD::Dataset d(argv[1], "dataset", true);
    ISMRMRD::ProtocolDeserializer deserializer(std::cin);

    ISMRMRD::IsmrmrdHeader hdr;
    deserializer.deserialize(hdr);

    // We will convert the XML header to a string and write it to the HDF5 file
    std::stringstream xmlstream;
    ISMRMRD::serialize(hdr, xmlstream);
    d.writeHeader(xmlstream.str());

    while (deserializer.peek() != ISMRMRD::ISMRMRD_MESSAGE_CLOSE) {
        if (deserializer.peek() == ISMRMRD::ISMRMRD_MESSAGE_ACQUISITION) {
            ISMRMRD::Acquisition acq;
            deserializer.deserialize(acq);
            d.appendAcquisition(acq);
        } else if (deserializer.peek() == ISMRMRD::ISMRMRD_MESSAGE_IMAGE) {
            if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_USHORT) {
                ISMRMRD::Image<unsigned short> img;
                deserializer.deserialize(img);
                d.appendImage(create_image_series_name(img), img);
                break;
            } else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_SHORT) {
                ISMRMRD::Image<short> img;
                deserializer.deserialize(img);
                d.appendImage(create_image_series_name(img), img);
                break;
            } else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_UINT) {
                ISMRMRD::Image<unsigned int> img;
                deserializer.deserialize(img);
                d.appendImage(create_image_series_name(img), img);
                break;
            } else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_INT) {
                ISMRMRD::Image<int> img;
                deserializer.deserialize(img);
                d.appendImage(create_image_series_name(img), img);
                break;
            } else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_FLOAT) {
                ISMRMRD::Image<float> img;
                deserializer.deserialize(img);
                d.appendImage(create_image_series_name(img), img);
                break;
            } else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_DOUBLE) {
                ISMRMRD::Image<double> img;
                deserializer.deserialize(img);
                d.appendImage(create_image_series_name(img), img);
                break;
            } else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_CXFLOAT) {
                ISMRMRD::Image<std::complex<float> > img;
                deserializer.deserialize(img);
                d.appendImage(create_image_series_name(img), img);
                break;
            } else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_CXDOUBLE) {
                ISMRMRD::Image<std::complex<double> > img;
                deserializer.deserialize(img);
                d.appendImage(create_image_series_name(img), img);
                break;
            } else {
                throw std::runtime_error("Unknown image type");
                break;
            }
        } else if (deserializer.peek() == ISMRMRD::ISMRMRD_MESSAGE_WAVEFORM) {
            ISMRMRD::Waveform wfm;
            deserializer.deserialize(wfm);
            d.appendWaveform(wfm);
        } else {
            std::cerr << "Unknown message type: " << deserializer.peek() << std::endl;
            return 1;
        }
    }
    return 0;
}
