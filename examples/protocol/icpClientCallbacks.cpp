#include <iostream>
#include "icpClientCallbacks.h"

using namespace ISMRMRD::ICP;
/*******************************************************************************
 ******************************************************************************/
ClientEntityHandler::ClientEntityHandler
(
  Client* client
)
: _client (client)
{
}
/******************************************************************************/

void ClientEntityHandler::receive
(
  Callback*     base,
  Entity* entity
)
{
  ClientEntityHandler* _this = static_cast<ClientEntityHandler*>(base);
  ETYPE etype = entity->getEntityType();

  switch (etype)
  {
    case ISMRMRD_HANDSHAKE:

      _this->_client->processHandshake (static_cast<HANDSHAKE*>(entity));
      break;

    case ISMRMRD_COMMAND:
      _this->_client->processCommand (static_cast<COMMAND*>(entity));
      break;

    case ISMRMRD_ERROR_REPORT:

      _this->_client->processError (static_cast<ERRREPORT*>(entity));
      break;

    default:

      std::cout << "Warning! Entity Handler received unexpected entity\n";
      break;
  }
}

/*******************************************************************************
 ******************************************************************************/
ClientImageProcessor::ClientImageProcessor
(
  Client* client,
  std::string fname,
  std::string dname,
  std::mutex& mtx
)
: _client (client),
  _dset (fname.c_str(), dname.c_str()),
  _mtx (mtx)
{
}
/******************************************************************************/

void ClientImageProcessor::receive
(
  Callback*          base,
  Entity*      entity
)
{
  ClientImageProcessor* _this = static_cast<ClientImageProcessor*>(base);
  ETYPE etype = entity->getEntityType();
  STYPE stype = entity->getStorageType();

  if (etype == ISMRMRD_HEADER)
  {
    std::cout << "NOT Writing Ismrmrd Header to file\n";
//    IsmrmrdHeaderWrapper* wrp =
//      static_cast<IsmrmrdHeaderWrapper*>(entity);

//    std::string tmp = wrp->getString ();
//    std::cout << "Before dset.writeHeader\n";
//    {
//      std::lock_guard<std::mutex> guard (_mtx);
//      _this->_dset.writeHeader (tmp);
//    }
//    std::cout << "After dset.writeHeader\n";
  }
  else if (etype == ISMRMRD_IMAGE)
  {
    std::cout << "Writing image to file\n";

    if (stype == ISMRMRD_SHORT)
    {
      _this->writeImage<int16_t> (entity);
    }
    else if (stype == ISMRMRD_USHORT)
    {
      _this->writeImage<uint16_t> (entity);
    }
    else if (stype == ISMRMRD_INT)
    {
      _this->writeImage<int32_t> (entity);
    }
    else if (stype == ISMRMRD_UINT)
    {
      _this->writeImage<uint32_t> (entity);
    }
    else if (stype == ISMRMRD_FLOAT)
    {
      _this->writeImage<float> (entity);
    }
    else if (stype == ISMRMRD_DOUBLE)
    {
      _this->writeImage<double> (entity);
    }
    else if (stype == ISMRMRD_CXFLOAT)
    {
      _this->writeImage<std::complex<float>> (entity);
    }
    else if (stype == ISMRMRD_CXDOUBLE)
    {
      _this->writeImage<std::complex<double>> (entity);
    }
    else
    {
      std::cout << "Error! Received unexpected image storage type\n";
    }

    std::cout << "Image processing done" << std::endl;

    _this->_client->taskDone();
  }
  else
  {
    std::cout << "Error! Received unexpected entity type\n";
  }
}

/*******************************************************************************
 ******************************************************************************/
