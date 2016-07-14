// client.cpp
#include "icpClient.h"
#include "icpClientCallbacks.h"

using namespace ISMRMRD::ICP;

static std::mutex gmtx;

/*****************************************************************************
 ****************************************************************************/
int32_t Client::getStream
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

/*****************************************************************************
 ****************************************************************************/
void Client::processHandshake
(
  HANDSHAKE* msg
)
{
  std::cout << "Received handshake response for \"" << _client_name
            << "\": name = \"" << msg->getClientName()
            << "\", status = " << msg->getConnectionStatus() << "\n";

  // Here the manifest received from server could be verified against the
  // client's manifest and some configuration and/or processing decisions
  // could be made. This example just replaces original manifest with the
  // one received from server.
  _manifest = msg->getManifest ();

  _callbacks.emplace_back
    (new ClientImageProcessor (this, _out_fname, _out_dset, gmtx));
  using namespace std::placeholders;
  auto fp = std::bind (&Callback::receive, _callbacks.back(), _1, _2);

  int32_t temp;
  std::vector<uint32_t> streams;

  if ((temp = getStream (ISMRMRD_IMAGE, ISMRMRD_USHORT)) >= 0)
  {
    streams.push_back (std::abs(temp));
  }
  if ((temp = getStream (ISMRMRD_IMAGE, ISMRMRD_SHORT)) >= 0)
  {
    streams.push_back (std::abs (temp));
  }
  if ((temp = getStream (ISMRMRD_IMAGE, ISMRMRD_UINT)) >= 0)
  {
    streams.push_back (std::abs (temp));
  }
  if ((temp = getStream (ISMRMRD_IMAGE, ISMRMRD_INT)) >= 0)
  {
    streams.push_back (std::abs (temp));
  }
  if ((temp = getStream (ISMRMRD_IMAGE, ISMRMRD_FLOAT)) >= 0)
  {
    streams.push_back (std::abs(temp));
  }
  if ((temp = getStream (ISMRMRD_IMAGE, ISMRMRD_DOUBLE)) >= 0)
  {
    streams.push_back (std::abs (temp));
  }
  if ((temp = getStream (ISMRMRD_IMAGE, ISMRMRD_CXFLOAT)) >= 0)
  {
    streams.push_back (std::abs (temp));
  }
  if ((temp = getStream (ISMRMRD_IMAGE, ISMRMRD_CXDOUBLE)) >= 0)
  {
    streams.push_back (std::abs (temp));
  }

  if (streams.size() > 0)
  {
    streams.push_back (ISMRMRD_HEADER_STREAM);
    if (!_session->registerHandler ((CB) fp, _callbacks.back(), streams))
    {
      std::cout << "Warning, failed to register Image Handler streams\n";
      sendCommand (ISMRMRD_COMMAND_CLOSE_CONNECTION, 0);
      _session->shutdown();
    }
    else
    {
      _input_thread = std::thread (&Client::beginInput, this, std::ref (gmtx));

      //std::thread (&Client::beginInput,
                   //this,
                   //_in_fname,
                   //_in_dset,
                   //&(*_session),
                   //std::ref (gmtx)).detach();
    }
  }
  else
  {
    _callbacks.pop_back();
    std::cout << "Warning, no stream numbers found for Image Handler\n";
    sendCommand (ISMRMRD_COMMAND_CLOSE_CONNECTION, 0);
    _session->shutdown();
  }
}

/*****************************************************************************
 ****************************************************************************/
void Client::processCommand
(
  COMMAND* msg
)
{
  switch (msg->getCommandType())
  {
    case ISMRMRD_COMMAND_DONE_FROM_SERVER:

      std::cout << "Received DONE from server\n";
      _server_done = true;
      if (_task_done)
      {
        std::cout << "Sending CLOSE_CONNECTION command\n";
        sendCommand (ISMRMRD_COMMAND_CLOSE_CONNECTION, 0);
        std::cout << _client_name << " shutting down\n";
        _session->shutdown();
      }
      break;

    default:
      std::cout << "Received unexpected command\n";
      break;
  }
}

/*****************************************************************************
 ****************************************************************************/
void Client::processError
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

/*****************************************************************************
 ****************************************************************************/
void Client::taskDone
(
)
{
  _task_done = true;
  if (_server_done)
  {
    std::cout << "Sending CLOSE_CONNECTION command\n";
    sendCommand (ISMRMRD_COMMAND_CLOSE_CONNECTION, 0);
    std::cout << _client_name << " shutting down\n";
    _session->shutdown();
  }
}

/*****************************************************************************
 ****************************************************************************/
void Client::sendCommand
(
  CommandType cmd_type,
  uint32_t    cmd_id
)
{
  Command msg;
  msg.setCommandType (cmd_type);
  msg.setCommandId (cmd_id);
  _session->send (&msg);

  return;
}

/*****************************************************************************
 ****************************************************************************/
void Client::sendError
(
  ErrorType    type,
  std::string  descr
)
{
  ErrorReport msg;
  msg.setErrorType (type);
  msg.setErrorDescription (descr);
  _session->send (&msg);

  return;
}

/*******************************************************************************
 ******************************************************************************/
void Client::sendHandshake
(
)
{
  Handshake msg;
  msg.setTimestamp ((uint64_t)std::time(nullptr));
  msg.setConnectionStatus (CONNECTION_REQUEST);
  msg.setClientName (_client_name);
  msg.addManifestEntry (5000,
                        ISMRMRD_MRACQUISITION,
                        ISMRMRD_SHORT);//,
                        //std::string ("Image Reconstruction"));

  msg.addManifestEntry (5001,
                        ISMRMRD_MRACQUISITION,
                        ISMRMRD_INT);//,
                        //std::string ("Image Reconstruction"));

  msg.addManifestEntry (5002,
                        ISMRMRD_MRACQUISITION,
                        ISMRMRD_FLOAT);//,
                        //std::string ("Image Reconstruction"));

  msg.addManifestEntry (5003,
                        ISMRMRD_MRACQUISITION,
                        ISMRMRD_DOUBLE);//,
                        //std::string ("Image Reconstruction"));

  msg.addManifestEntry (ISMRMRD_HEADER_STREAM,
                        ISMRMRD_HEADER,
                        ISMRMRD_STORAGE_NONE);//,
                        //std::string ("ISMRMRD Header"));

  msg.addManifestEntry (6000,
                        ISMRMRD_IMAGE,
                        ISMRMRD_SHORT);//,
                        //std::string ("Image Reconstruction"));

  msg.addManifestEntry (6001,
                        ISMRMRD_IMAGE,
                        ISMRMRD_USHORT);//,
                        //std::string ("Image Reconstruction"));

  msg.addManifestEntry (6002,
                        ISMRMRD_IMAGE,
                        ISMRMRD_INT);//,
                        //std::string ("Image Reconstruction"));

  msg.addManifestEntry (6003,
                        ISMRMRD_IMAGE,
                        ISMRMRD_UINT);//,
                        //std::string ("Image Reconstruction"));

  msg.addManifestEntry (6004,
                        ISMRMRD_IMAGE,
                        ISMRMRD_FLOAT);//,
                        //std::string ("Image Reconstruction"));

  msg.addManifestEntry (6005,
                        ISMRMRD_IMAGE,
                        ISMRMRD_DOUBLE);//,
                        //std::string ("Image Reconstruction"));

  msg.addManifestEntry (6006,
                        ISMRMRD_IMAGE,
                        ISMRMRD_CXFLOAT);//,
                        //std::string ("Image Reconstruction"));

  msg.addManifestEntry (6007,
                        ISMRMRD_IMAGE,
                        ISMRMRD_CXDOUBLE);//,
                        //std::string ("Image Reconstruction"));

  msg.addManifestEntry (10000,
                        ISMRMRD_BLOB,
                        ISMRMRD_STORAGE_NONE);//,
                        //std::string ("Device 1 Data"));

  _manifest = msg.getManifest();
  _session->send (&msg);
}

/*******************************************************************************
 ******************************************************************************/
void Client::beginInput
(
  std::mutex& mtx
)
{
  std::cout << __func__ << "  starting\n";

  std::string xml_head;
  Dataset dset (_in_fname, _in_dset);

  {
    std::lock_guard<std::mutex> guard (mtx);
    xml_head = dset.readHeader();
  }

  IsmrmrdHeader xmlHeader;
  deserialize (xml_head.c_str(), xmlHeader);

  ETYPE etype = (ETYPE)xmlHeader.streams[0].entityType;
  STYPE stype = (STYPE)xmlHeader.streams[0].storageType;


  Command msg;
  msg.setCommandType (ISMRMRD_COMMAND_CONFIGURATION);
  msg.setCommandId (0);
  
  if (etype == ISMRMRD_MRACQUISITION)
  {
    if (stype == ISMRMRD_SHORT)
    {
      msg.setConfigType (CONFIGURATION_RECON_SHORT);
    }
    else if (stype == ISMRMRD_INT)
    {
      msg.setConfigType (CONFIGURATION_RECON_INT);
    }
    else if (stype == ISMRMRD_FLOAT ||
             stype == ISMRMRD_CXFLOAT)
    {
      msg.setConfigType (CONFIGURATION_RECON_FLOAT);
    }
    else if (stype == ISMRMRD_DOUBLE ||
             stype == ISMRMRD_CXDOUBLE)
    {
      msg.setConfigType (CONFIGURATION_RECON_DOUBLE);
    }
    else
    {
      std::cout << "Warning, unexpected acquisition storage type in file\n";
      msg.setConfigType (CONFIGURATION_NONE);
    }
  }
  else
  {
      std::cout << "Warning, unexpected entity type in file\n";
      msg.setConfigType (CONFIGURATION_NONE);
  }

  _session->send (&msg);
  _session->send (&xmlHeader);

  if (stype == ISMRMRD_SHORT)
  {
    sendAcquisitions <int16_t> (dset, mtx);
  }
  else if (stype == ISMRMRD_INT)
  {
    sendAcquisitions <int32_t> (dset, mtx);
  }
  // TODO: FLOAT and CXFLOAT are treated the same here
  else if (stype == ISMRMRD_FLOAT ||
           stype == ISMRMRD_CXFLOAT)
  {
    sendAcquisitions <float> (dset, mtx);
  }
  // TODO: DOUBLE and CXDOUBLE are treated the same here 
  else if (stype == ISMRMRD_DOUBLE ||
           stype == ISMRMRD_CXDOUBLE)
  {
    sendAcquisitions <double> (dset, mtx);
  }
  else
  {
    std::cout << "Acq storage type = " << stype << "\n";
    throw std::runtime_error ("Unexpected MR Acquisition storage type");
  }
  
  Command cmd;
  cmd.setCommandId (0);
  cmd.setCommandType (ISMRMRD_COMMAND_STOP_FROM_CLIENT);
  _session->send (&cmd);

  std::cout << "Input completed\n";
}

/*******************************************************************************
 ******************************************************************************/
template <typename S>
void Client::sendAcquisitions
(
  Dataset&    dset,
  std::mutex& mtx
)
{
  uint32_t num_acq = dset.getNumberOfAcquisitions (0);
  for (int ii = 0; ii < num_acq; ii++)
  {
    //usleep (500); // For testing only
    Acquisition<S> acq;
    {
      std::lock_guard<std::mutex> guard (mtx);
      acq = dset.readAcquisition<S> (ii, 0); //TODO lookup stream number
    }
    acq.setStream (getStream (ISMRMRD_MRACQUISITION, acq.getStorageType()));
    _session->send (&acq);
  }
}

/*******************************************************************************
 ******************************************************************************/
Client::Client
(
  SESSION          session,
  std::string      client_name,
  std::string      in_fname,
  std::string      out_fname,
  std::string      in_dataset,
  std::string      out_dataset
)
: _session         (std::move (session)),
  _client_name     (client_name),
  _in_fname        (in_fname),
  _out_fname       (out_fname),
  _in_dset         (in_dataset),
  _out_dset        (out_dataset)
{

  std::cout << "\""<< _client_name << "\"  starting\n";

  using namespace std::placeholders;

  _callbacks.emplace_back (new ClientEntityHandler (this));
  auto fp = std::bind (&Callback::receive, _callbacks.back(), _1, _2);
  _session->registerHandler ((CB) fp, _callbacks.back(), HANDSHAKE_STREAM);
  _session->registerHandler ((CB) fp, _callbacks.back(), ERROR_REPORT_STREAM);
  _session->registerHandler ((CB) fp, _callbacks.back(), COMMAND_STREAM);

  sendHandshake ();
  _session->run ();
}

/*******************************************************************************
 ******************************************************************************/
Client::~Client
(
)
{
  if (_input_thread.joinable()) _input_thread.join();
  for (auto cb : _callbacks) if (cb) delete (cb);
}

/*******************************************************************************
 ******************************************************************************/
