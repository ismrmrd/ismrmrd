#include "icpServer.h"
#include <functional>
#include "icpServerCallbacks.h"

using namespace ISMRMRD::ICP;
using namespace std::placeholders;

/*******************************************************************************
 ******************************************************************************/
void Server::processHandshake
(
  HANDSHAKE* msg
)
{
  _manifest = msg->getManifest();
  // Here server's capabilities may be compared with the received manifest.
  // For this demo all entities not related to image reconstruction will be
  // filtered out.

  std::vector<std::string> unsupported;
  for (auto it = _manifest.begin(); it != _manifest.end(); ++it)
  {
    if (it->second.entity_type != ISMRMRD_MRACQUISITION &&
        it->second.entity_type != ISMRMRD_HEADER        &&
        it->second.entity_type != ISMRMRD_IMAGE)
    {
      unsupported.push_back (it->first);
    }
  }

  // Remove unsupported entries
  for (int ii = 0; ii < unsupported.size(); ++ii)
  {
    _manifest.erase (unsupported[ii]);
  }

  clientAccepted (msg, true);
}

/*******************************************************************************
 ******************************************************************************/
void Server::processCommand
(
  COMMAND* msg
)
{
  switch (msg->getCommandType())
  {
    case ISMRMRD_COMMAND_CONFIGURATION:

      configure (msg);
      break;

    case ISMRMRD_COMMAND_STOP_FROM_CLIENT:

      std::cout << "Received STOP command\n";
      _client_done = true;
      if (_task_done)
      {
        sendCommand (ISMRMRD_COMMAND_DONE_FROM_SERVER, 0);
      }
      break;

    case ISMRMRD_COMMAND_CLOSE_CONNECTION:

      std::cout << "Received CLOSE command\n";
      _session->shutdown();
      break;

    default:

      std::cout << "Received unexpected command\n";
      break;
  }
}

/*******************************************************************************
 ******************************************************************************/
void Server::processError
(
  ERRREPORT* msg
)
{
  std::cout << "Error Type: " << msg->getErrorType()
            << ", Error Command: " << msg->getErrorCommandType()
            << ", Error Command ID: " << msg->getErrorCommandId()
            << ", Error Entity: " << msg->getErrorEntityType()
            << ",\nError Description: " << msg->getErrorDescription() << "\n";
}

/*******************************************************************************
 ******************************************************************************/
void Server::taskDone
(
)
{
  _task_done = true;

  if (_client_done) sendCommand (ISMRMRD_COMMAND_DONE_FROM_SERVER, 0);
}

/*****************************************************************************
 ****************************************************************************/
int32_t Server::getStream
(
  ETYPE etype,
  STYPE stype
)
{
  std::string key = std::to_string (etype) +
                    std::string ("_") +
                    std::to_string (stype);
  if (_manifest.find (key) == _manifest.end())
  {
    std::cout << "Warning! Stream number not available for " << key << "\n";
    return -1;
  }

  return _manifest.at (key).stream;
}

/*******************************************************************************
 ******************************************************************************/
void Server::configure
(
  COMMAND* msg
)
{
  if (msg->getConfigType() == CONFIGURATION_RECON_SHORT)
  {
    std::cout << "Received config request for image recon short\n";

    _callbacks.emplace_back (new ServerImageRecon<int16_t> (this));
    auto fp = std::bind (&Callback::receive, _callbacks.back(), _1, _2);
    _session->registerHandler ((CB)fp, _callbacks.back(), ISMRMRD_HEADER_STREAM);
    _session->registerHandler ((CB)fp, _callbacks.back(),
                               getStream (ISMRMRD_MRACQUISITION, ISMRMRD_SHORT));
  }
  else if (msg->getConfigType() == CONFIGURATION_RECON_INT)
  {
    std::cout << "Received config request for image recon int\n";

    _callbacks.emplace_back (new ServerImageRecon<int32_t> (this));
    auto fp = std::bind (&Callback::receive, _callbacks.back(), _1, _2);
    _session->registerHandler ((CB)fp, _callbacks.back(), ISMRMRD_HEADER_STREAM);
    _session->registerHandler ((CB)fp, _callbacks.back(),
                               getStream (ISMRMRD_MRACQUISITION, ISMRMRD_INT));
  }
  else if (msg->getConfigType() == CONFIGURATION_RECON_FLOAT)
  {
    std::cout << "Received config request for image recon float\n";

    _callbacks.emplace_back (new ServerImageRecon<float> (this));
    auto fp = std::bind (&Callback::receive, _callbacks.back(), _1, _2);
    _session->registerHandler ((CB)fp, _callbacks.back(), ISMRMRD_HEADER_STREAM);
    _session->registerHandler ((CB)fp, _callbacks.back(),
                               getStream (ISMRMRD_MRACQUISITION, ISMRMRD_FLOAT));
  }
  else if (msg->getConfigType() == CONFIGURATION_RECON_DOUBLE)
  {
    std::cout << "Received config request for image recon double\n";

    _callbacks.emplace_back (new ServerImageRecon<double> (this));
    auto fp = std::bind (&Callback::receive, _callbacks.back(), _1, _2);
    _session->registerHandler ((CB)fp, _callbacks.back(), ISMRMRD_HEADER_STREAM);
    _session->registerHandler ((CB)fp, _callbacks.back(),
                              getStream (ISMRMRD_MRACQUISITION, ISMRMRD_DOUBLE));
  }
  else
  {
    std::cout <<"Warning: Requested configuration option not implemented\n";
  } 
}

/*****************************************************************************
 ****************************************************************************/
void Server::sendCommand
(
  CommandType cmd_type,
  uint32_t             cmd_id
)
{
  Command msg;
  msg.setCommandType (cmd_type);
  msg.setCommandId (cmd_id);
  _session->send (&msg);
}

/*****************************************************************************
 ****************************************************************************/
void Server::sendEntity
(
  ENTITY*  ent
)
{
  _session->send (ent);
}

/*****************************************************************************
 ****************************************************************************/
void Server::clientAccepted
(
  Handshake* hand,
  bool       accepted
)
{
  Handshake msg;

  msg.setTimestamp (hand->getTimestamp());
  msg.setConnectionStatus ((accepted) ?
    CONNECTION_ACCEPTED : CONNECTION_DENIED_UNKNOWN_USER);
  msg.setClientName (hand->getClientName());

  if (_manifest.size() > 0)
  {
    for (auto it = _manifest.begin(); it != _manifest.end(); it++)
    {
      msg.addManifestEntry (it->second.stream,
                            it->second.entity_type,
                            it->second.storage_type);//,
                            //std::string (it->second.description.begin(),
                                         //it->second.description.end()));
    }
  }

  _session->send (&msg);

  if (!accepted)
  {
    sendCommand (ISMRMRD_COMMAND_DONE_FROM_SERVER, 0);
    std::cout << "Client not accepted, server DONE\n";
    _session->shutdown();
  }
}

/*****************************************************************************
 ****************************************************************************/
void Server::sendError
(
  ErrorType type,
  std::string        descr
)
{
  ErrorReport msg;
  msg.setErrorType (type);
  msg.setErrorDescription (descr);
  _session->send (&msg);
}

/*****************************************************************************
 ****************************************************************************/
Server::Server
(
  SESSION  session
)
: _session (std::move (session))
{

  _callbacks.emplace_back (new ServerEntityHandler (this));
  auto fp = std::bind (&Callback::receive, _callbacks.back(), _1, _2);

  _session->registerHandler ((CB) fp, _callbacks.back(), HANDSHAKE_STREAM);
  _session->registerHandler ((CB) fp, _callbacks.back(), ERROR_REPORT_STREAM);
  _session->registerHandler ((CB) fp, _callbacks.back(), COMMAND_STREAM);

  _session->run ();
}

/*******************************************************************************
 ******************************************************************************/
Server::~Server
(
)
{
  for (auto cb : _callbacks) if (cb) delete (cb);
}
