#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif
#include <iostream>

#include <memory>
#include <fstream>
#include <sstream>
#include "ismrmrd/xml.h"
#include "ismrmrd/serialization.h"
#include "ismrmrd/serialization_iostream.h"


// This is how the Gadgetron reads the ISMRMRD Header from its input stream
ISMRMRD::IsmrmrdHeader consume_ismrmrd_header(std::istream& input_stream)
{
    ISMRMRD::IsmrmrdHeader hdr;
    uint16_t id = ISMRMRD::ISMRMRD_MESSAGE_CLOSE;
    input_stream.read(reinterpret_cast<char*>(&id), sizeof(id));

    switch(id)
    {
        case ISMRMRD::ISMRMRD_MESSAGE_HEADER:
        {
            uint32_t hdr_size = 0;
            input_stream.read(reinterpret_cast<char*>(&hdr_size), sizeof(uint32_t));

            if(hdr_size > 0)
            {
                std::vector<char> data(hdr_size);

                input_stream.read(data.data(), hdr_size);
                ISMRMRD::deserialize(std::string(data.data(), data.size()).c_str(), hdr);
            }
            else
            {
                throw std::runtime_error("Expected size > 0, got: " + std::to_string(hdr_size));
            }

            break;
        }
        default:
        {
            throw std::runtime_error("Expected HEADER enumeration, got: " + std::to_string(id));
        }
    }

    return hdr;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <output_file>" << std::endl;
        return -1;
    }

#ifdef _WIN32
    // Change std::cin/std::cout to binary mode
    _setmode(_fileno(stdout), _O_BINARY);
    _setmode(_fileno(stdin), _O_BINARY);
#endif

    ISMRMRD::IsmrmrdHeader header;
    header.experimentalConditions.H1resonanceFrequency_Hz = 63500000;
    header.encoding.resize(1);

    {
        // Test ProtocolSerializer to file
        std::ofstream outfile(argv[1], std::ios::out | std::ios::binary);
        if (!outfile.good()) {
            std::cerr << "Could not open output file: " << argv[2] << std::endl;
            return -1;
        }
        ISMRMRD::OStreamView ws(outfile);
        ISMRMRD::ProtocolSerializer serializer(ws);
        serializer.serialize(header);
        outfile.flush();
        std::cerr << "ProtocolSerializer wrote " << outfile.tellp() << " bytes to file " << argv[1] << "." << std::endl;
        serializer.close();
        outfile.close();

        std::ifstream infile(argv[1], std::ios::in | std::ios::binary);
        if (!infile.good()) {
            std::cerr << "Could not open input file: " << argv[1] << std::endl;
            return -1;
        }
        ISMRMRD::IsmrmrdHeader deserialized_header = consume_ismrmrd_header(infile);
        if (deserialized_header != header)
        {
            std::cerr << "Deserialized header does not match original header!" << std::endl;
            return -1;
        }
    }

    {
        // Test ProtocolSerializer to stdout
        ISMRMRD::OStreamView ws(std::cout);
        ISMRMRD::ProtocolSerializer serializer(ws);
        serializer.serialize(header);
        // NOTE: Using `tellp` on `cout` is not correct, but works in this scenario for a quick test.
        std::cerr << "ProtocolSerializer wrote " << std::cout.tellp() << " bytes to stdout." << std::endl;
        serializer.close();
    }

    {
        // Test direct serialization to file
        std::ofstream xml_out("processed.xml", std::ios::out | std::ios::binary);
        if (!xml_out.good()) {
            std::cerr << "Could not open output file: processed.xml" << std::endl;
            return -1;
        }
        ISMRMRD::serialize(header, xml_out);
        xml_out.flush();
        std::cerr << "Directly serialized header as " << xml_out.tellp() << " bytes to file 'processed.xml'." << std::endl;
        xml_out.close();

        std::ifstream xml_in("processed.xml", std::ios::in | std::ios::binary);
        if (!xml_in.good()) {
            std::cerr << "Could not open input file: processed.xml" << std::endl;
            return -1;
        }
        std::string xml((std::istreambuf_iterator<char>(xml_in)), std::istreambuf_iterator<char>());

        ISMRMRD::IsmrmrdHeader header_in;
        ISMRMRD::deserialize(xml.c_str(), header_in);
        if (header_in != header)
        {
            std::cerr << "Deserialized header does not match original header!" << std::endl;
            return -1;
        }
    }

    {
        // Test direct serialization to stringstream
        std::stringstream sstr;
        ISMRMRD::serialize(header, sstr);
        std::string as_str = sstr.str();
        std::cerr << "Directly serialized header as " << sstr.tellp() << " (" << as_str.size() << ") bytes to stringstream." << std::endl;
        ISMRMRD::deserialize(as_str.c_str(), header);
    }

    return 0;
}
