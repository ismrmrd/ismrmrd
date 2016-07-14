#include "icpClient.h"
#include "icpSession.h"
#include "icpConnection.h"
#include <boost/program_options.hpp>

using namespace ISMRMRD::ICP;

namespace po = boost::program_options;

std::string       client_name = "Client 1";
std::string       host        = "127.0.0.1";
std::string       in_fname    = "testdata.h5";
std::string       in_dset     = "dataset";
std::string       out_fname   = "FileOut.h5";
std::string       out_dset    = "dataset";
unsigned short    port        = 50050;

/*******************************************************************************
 ******************************************************************************/
void startClient (SESSION session)
{
  Client* client = new Client (std::move (session),
                                     client_name,
                                     in_fname,
                                     out_fname,
                                     in_dset,
                                     out_dset);
  delete (client);
  std::cout << client_name << " done _____________________________________\n\n";
  return;
}

/*******************************************************************************
 ******************************************************************************/
int main
(
  int argc,
  char* argv[]
)
{
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h",
     "produce help message")
    ("client name,c",
     po::value<std::string>(&client_name)->default_value(client_name),
     "Client Name")
    ("host,H",
     po::value<std::string>(&host)->default_value(host),
     "Server IP address")
    ("port,p",
     po::value<unsigned short>(&port)->default_value(port),
     "TCP port for server")
    ("fin,i",
     po::value<std::string>(&in_fname)->default_value(in_fname),
     "HDF5 Input file")
    ("in_group,I",
     po::value<std::string>(&in_dset)->default_value(in_dset),
     "Input group name")
    ("fout,o",
     po::value<std::string>(&out_fname)->default_value(out_fname),
     "Output file name")
    ("out_group,O",
     po::value<std::string>(&out_dset)->default_value(out_dset),
     "Output group name")
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
    std::cout << desc << "\n";
    return 0;
  }

  std::cout << "Using client name <" << client_name << ">, host IP address <"
            << host << ">, and port <" << port << ">" << std::endl;
  std::cout << "Re-start with: Client -h to see all options\n" << std::endl;

  Connection client_conn (host, port);
  client_conn.registerUserApp ((START_USER_APP_FUNC) &startClient);
  client_conn.connect();

  return 0;
}
