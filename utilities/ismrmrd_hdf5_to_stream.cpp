#include "ismrmrd/dataset.h"
#include "ismrmrd/serialization.h"
#include <iostream>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    ISMRMRD::Dataset d(argv[1], "dataset", false);
    ISMRMRD::ProtocolSerializer serializer(std::cout);

    std::string xml;
    d.readHeader(xml);
    ISMRMRD::IsmrmrdHeader hdr;
    ISMRMRD::deserialize(xml.c_str(), hdr);

    serializer.serialize(hdr);
    for (unsigned int i = 0; i < d.getNumberOfAcquisitions(); i++) {
        ISMRMRD::Acquisition acq;
        d.readAcquisition(i, acq);
        serializer.serialize(acq);
    }
    serializer.close();

    return 0;
}
