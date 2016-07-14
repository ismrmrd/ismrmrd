#ifndef ICP_SERVER_H
#define ICP_SERVER_H

#include "icpSession.h"
#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/xml.h"

namespace ISMRMRD { namespace ICP
{

/*******************************************************************************
 ******************************************************************************/
class Server
{
  public:

  Server       (SESSION);
  ~Server      ();

  void    processHandshake (HANDSHAKE* msg);
  void    processCommand   (COMMAND*   msg);
  void    processError     (ERRREPORT* msg);
  void    sendEntity       (ENTITY*);
  void    sendError        (ErrorType type, std::string descr);
  int32_t getStream        (ETYPE etype, STYPE stype);
  void    taskDone         ();

  private:

  void    clientAccepted  (Handshake* msg, bool accepted);
  void    configure       (COMMAND* cmd);
  void    sendCommand     (CommandType cmd, uint32_t cmd_id);

  SESSION                         _session;
  bool                            _client_done;
  bool                            _task_done;
  std::vector<Callback*>          _callbacks;
  std::map<std::string, Manifest> _manifest;
};

}} // end of namespace scope
#endif // ICP_SERVER_H
