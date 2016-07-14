#include <iostream>
#include "icpServerCallbacks.h"

using namespace ISMRMRD::ICP;

/*******************************************************************************
 ******************************************************************************/
ServerEntityHandler::ServerEntityHandler
(
  Server* server
)
: _server (server)
{
}
/******************************************************************************/

void ServerEntityHandler::receive
(
  Callback* base,
  ENTITY*      entity
)
{
  ServerEntityHandler* _this = static_cast<ServerEntityHandler*>(base);
  ETYPE etype = entity->getEntityType();

  switch (etype)
  {
    case ISMRMRD::ISMRMRD_HANDSHAKE:

      _this->_server->processHandshake (static_cast<HANDSHAKE*>(entity));
      break;

    case ISMRMRD::ISMRMRD_COMMAND:
      _this->_server->processCommand (static_cast<COMMAND*>(entity));
      break;

    case ISMRMRD::ISMRMRD_ERROR_REPORT:

      _this->_server->processError (static_cast<ERRREPORT*>(entity));
      break;

    default:

      std::cout << "Warning! Entity Handler received unexpected entity\n";
      break;
  }
}

/*******************************************************************************
 ******************************************************************************/
