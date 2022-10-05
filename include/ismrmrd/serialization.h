#ifndef ISMRMRDSERIALIZATION_H
#define ISMRMRDSERIALIZATION_H

#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/waveform.h"
#include "ismrmrd/xml.h"

namespace ISMRMRD {
// serialize Acquisition to ostream
void EXPORTISMRMRD serialize(const Acquisition &acq, std::ostream &os);

// serialize Image<T> to ostream
template <typename T>
void EXPORTISMRMRD serialize(const Image<T> &img, std::ostream &os);

// serialize Waveform to ostream
void EXPORTISMRMRD serialize(const Waveform &wfm, std::ostream &os);

// deserialize Acquisition from istream
void EXPORTISMRMRD deserialize(Acquisition &acq, std::istream &is);

// deserialize Image<T> from istream
template <typename T>
void EXPORTISMRMRD deserialize(Image<T> &img, std::istream &is);

// deserialize Waveform from istream
void EXPORTISMRMRD deserialize(Waveform &wfm, std::istream &is);

class EXPORTISMRMRD ProtocolSerializer {
public:
    ProtocolSerializer(std::ostream &os);
    void serialize(const IsmrmrdHeader &hdr);
    void serialize(const Acquisition &acq);
    template <typename T>
    void serialize(const Image<T> &img);
    void serialize(const Waveform &wfm);

protected:
    void write_msg_id(uint16_t id);
    std::ostream &os;
};

class EXPORTISMRMRD ProtocolDeserializer {
public:
    ProtocolDeserializer(std::istream &is);
    void deserialize(IsmrmrdHeader &hdr);
    void deserialize(Acquisition &acq);
    template <typename T>
    void deserialize(Image<T> &img);
    void deserialize(Waveform &wfm);

    // Peek at the next data type in the stream
    uint16_t peek();

protected:
    std::istream &is;
    uint16_t peeked;
};

} // namespace ISMRMRD

#endif // ISMRMRDSERIALIZATION_H
