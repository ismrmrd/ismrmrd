#include "icpServer.h"
#include "icpConnection.h"
#include "icpSession.h"

using namespace ISMRMRD::ICP;

/*******************************************************************************
 ******************************************************************************/
void startServer
(
  SESSION session
)
{
  Server* server = new Server (std::move (session));
  delete (server);
  std::cout << "Server done ==============================================\n\n";
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
  unsigned int port;

  if (argc != 2)
  {
    port = 50050;
  }
  else
  {
    port = std::atoi (argv[1]);
  }

  std::cout << "ISMRMRD Server app starts with port number " << port << '\n';
  std::cout << "To connect to a different port, restart: Server <port>\n\n";

  Connection server_conn (port);
  server_conn.registerUserApp ((START_USER_APP_FUNC) &startServer);
  server_conn.start();

  return 0;
}
