#ifndef ICP_CONNECTION_H
#define ICP_CONNECTION_H

#include <boost/asio.hpp>
#include <thread>
#include "icpSession.h"

namespace ISMRMRD { namespace ICP
{
/*******************************************************************************
 ******************************************************************************/
class Connection
{
public:

       Connection      (uint16_t port);
       Connection      (std::string host, uint16_t port);
  void start           ();
  void connect         ();
  void registerUserApp (START_USER_APP_FUNC func_ptr);

private:

  void catchSignal ();
  static void signalHandler (int sig_id);

  void                 asyncAccept ();
  void                 startUserApp (SOCKET_PTR);
  START_USER_APP_FUNC  runUserApp;

  static bool                    _running;
  static Connection*             _this;

  boost::asio::io_service        _io_service;
  std::shared_ptr<tcp::acceptor> _acceptor;
  std::string                    _host;
  unsigned short                 _port;
  bool                           _user_app_registered;
};

}} // end of namespace ISMRMRD::ICP
#endif // ICP_CONNECTION_H */
