#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/dataset.h"
#include "ismrmrd/xml.h"

#include <boost/program_options.hpp>
#include <boost/asio.hpp>

#include <thread>
#include <utility>
#include <iostream>

using boost::asio::ip::tcp;
namespace po = boost::program_options;

class Server {
public:
    Server(boost::asio::io_service& io_service, unsigned short port)//const tcp::endpoint& endpoint)
      : io_service_(io_service),
        port_(port)
    { }

    void start()
    {
        tcp::acceptor acceptor(io_service_, tcp::endpoint(tcp::v4(), port_));
        while (true) {
            tcp::socket sock(io_service_);
            acceptor.accept(sock);
            /* session(std::move(sock)); */
            std::thread(&Server::session, this, std::move(sock)).detach();
        }
    }

    void session(tcp::socket sock)
    {
        uint32_t xml_size = 0;
        std::string xml;

        boost::system::error_code error;

        // First, read the XML header's size in bytes, then the actual header
        boost::asio::read(sock, boost::asio::buffer(&xml_size, sizeof(xml_size)), error);
        xml.resize(xml_size);
        boost::asio::read(sock, boost::asio::buffer(&xml[0], xml_size), error);

        ISMRMRD::IsmrmrdHeader xmlHeader;
        ISMRMRD::deserialize(xml.c_str(), xmlHeader);

        std::map<int, std::vector<ISMRMRD::Acquisition<float> > > streams;

        while (true) {
            // Read identifier of next object
            uint32_t etype = 0;
            boost::asio::read(sock, boost::asio::buffer(&etype, sizeof(etype)), error);

            // TODO: handle more dataTypes
            if (etype != ISMRMRD::ISMRMRD_MRACQUISITION) {
                throw std::runtime_error("unsupported dataType ID received by server");
            }

            // Read the Acquisition header
            ISMRMRD::AcquisitionHeader head;
            boost::asio::read(sock, boost::asio::buffer(&head, sizeof(head)), error);

            // Create Acquisition and set its header to allocate memory for its data buffer
            ISMRMRD::Acquisition<float> acq;
            acq.setHead(head);

            // Read the acquisition data
            //size_t nbytes = acq.getNumberOfDataElements() * sizeof_storage_type(acq.getStorageType());
            //boost::asio::read(sock, boost::asio::buffer(acq.getDataPtr(), nbytes), error);

            // Read the acquisition trajectory
            std::vector<float> traj(head.number_of_samples * head.trajectory_dimensions);
            boost::asio::read(sock, boost::asio::buffer(traj), error);
            acq.setTraj(traj);

            if (streams.count (head.ent_head.stream) == 0)
            {
              std::vector<ISMRMRD::Acquisition<float> > stream;
              stream.push_back(acq);
              streams[head.ent_head.stream] = stream;
            }
            else
            {
              streams[head.ent_head.stream].push_back(acq);
            }
        }

        std::map<int, std::vector<ISMRMRD::Acquisition<float> > >::const_iterator it;
        for (it = streams.begin(); it != streams.end(); ++it) {
            std::cout << "Stream #" << it->first << " contains " << it->second.size() << " acquisitions\n";
        }


        //if (error == boost::asio::error::eof) {
            //return;
        //} else if (error) {
            //throw boost::system::system_error(error);
        //}


        /* boost::asio::write(sock, boost::asio::buffer(rev), error); */
        /* if (error) { */
        /*     throw boost::system::system_error(error); */
        /* } */
    }

private:
    boost::asio::io_service& io_service_;
    unsigned short port_;
    /* tcp::acceptor acceptor_; */
    /* tcp::socket socket_; */
};

/* class Client { */
/* public: */
/*     Client(boost::asio::io_service& io_service, const tcp::endpoint& endpoint) */
/*       : io_service_(io_service), */
/*         socket_(io_service) */
/*     { */
/*         std::cout << "Creating client...\n"; */
/*         do_connect(endpoint); */
/*     } */

/*     void send_dataset(const ISMRMRD::Dataset& dataset) */
/*     { */
/*         io_service_.post( */
/*     } */

/*     void close() */
/*     { */
/*         std::cout << "Closing client...\n"; */
/*         io_service_.post([this]() { socket_.close(); }); */
/*     } */

/* private: */
/*     void do_connect(tcp::endpoint endpoint) */
/*     { */
/*         std::cout << "Client do_connect...\n"; */
/*         socket_.async_connect(endpoint, */
/*             [this](boost::system::error_code ec) */
/*             { */
/*                 if (!ec) { */
/*                     std::cout << "connected to server\n"; */
/*                 } */
/*             }); */
/*     } */

/*     boost::asio::io_service& io_service_; */
/*     tcp::socket socket_; */
/* }; */

void start_server(unsigned short port)
{
    boost::asio::io_service io_service;
    Server server(io_service, port);
    server.start();
}

int main(int argc, char* argv[])
{
    unsigned short port = 9999;
    std::string in_fname, out_fname("out.h5");
    std::string in_hdf5_group("/dataset"), out_hdf5_group("/dataset");

    po::options_description desc("Allowed options");
    desc.add_options()
      ("help,h", "produce help message")
      ("port,p", po::value<unsigned short>(&port)->default_value(port), "TCP port for server")
      ("filename,f", po::value<std::string>(&in_fname), "Input file")
      ("outfile,o", po::value<std::string>(&out_fname)->default_value(out_fname), "Output file")
      ("in-group,g", po::value<std::string>(&in_hdf5_group)->default_value(in_hdf5_group), "Input group name")
      ("out-group,G", po::value<std::string>(&out_hdf5_group)->default_value(out_hdf5_group), "Output group name")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }

    if (!vm.count("filename")) {
        std::cerr << "You must supply an input filename\n";
        return 1;
    }

    // start the "server" in the background and detach so it dies when the program exits
    std::thread(start_server, port).detach();

    sleep(1);

    // Open dataset
    ISMRMRD::Dataset dataset(in_fname.c_str(), in_hdf5_group.c_str());

    // Read and deserialize XML header
    std::string xml = dataset.readHeader();
    ISMRMRD::IsmrmrdHeader xmlHeader;
    ISMRMRD::deserialize(xml.c_str(), xmlHeader);

    // Establish connection to server
    boost::asio::io_service io_service;
    tcp::socket sock(io_service);
    tcp::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"), port);
    sock.connect(endpoint);

    // First, send the XML header
    uint32_t xml_size = xml.size();
    boost::asio::write(sock, boost::asio::buffer(&xml_size, sizeof(xml_size)));
    boost::asio::write(sock, boost::asio::buffer(xml));

    for (size_t i = 0; i < dataset.getNumberOfAcquisitions(); ++i) {
        ISMRMRD::Acquisition<float> acq = dataset.readAcquisition<float>(i);

        // send the acquisition type (acquisition or waveform)
        // TODO: do we care if its an MR acquisition or just waveform data?
        uint32_t etype = ISMRMRD::ISMRMRD_MRACQUISITION;
        boost::asio::write(sock, boost::asio::buffer(&etype, sizeof(etype)));

        // send the acquisition header
        ISMRMRD::AcquisitionHeader head = acq.getHead();
        boost::asio::write(sock, boost::asio::buffer(&head, sizeof(head)));

        // send the acquisition data
        //boost::asio::write(sock, boost::asio::buffer(acq.getDataPtr(), acq.getNumberOfDataElements() * sizeof_storage_type(acq.getStorageType())));

        // send the acquisition trajectory
        std::vector<float> traj = acq.getTraj();
        if (traj.size() > 0) {
            boost::asio::write(sock, boost::asio::buffer(traj));
        }
    }


    return 0;
}
