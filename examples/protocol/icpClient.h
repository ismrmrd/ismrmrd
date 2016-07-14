#ifndef ICP_CLIENT_H
#define ICP_CLIENT_H

#include "icpSession.h"
#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/dataset.h"
#include "ismrmrd/xml.h"

namespace ISMRMRD { namespace ICP
{

/*******************************************************************************
 ******************************************************************************/
class Client
{
  public:

  Client  (SESSION session,
           std::string client_name,
           std::string in_fname,
           std::string out_fname,
           std::string in_dataset,
           std::string out_dataset);
  ~Client ();

  void processHandshake (HANDSHAKE* msg);
  void processCommand   (COMMAND*   msg);
  void processError     (ERRREPORT* msg);
  void taskDone         ();

  private:

  void    beginInput (std::mutex&);
  void    sendHandshake ();
  int32_t getStream (ETYPE, STYPE);
  void    sendCommand (CommandType cmd_type, uint32_t cmd_id);
  void    sendError (ErrorType type, std::string descr);

  template <typename S>
  void    sendAcquisitions (Dataset& dset, std::mutex& mtx);

  SESSION                         _session;
  std::thread                     _input_thread;
  std::string                     _client_name;
  std::string                     _in_fname;
  std::string                     _out_fname;
  std::string                     _in_dset;
  std::string                     _out_dset;
  bool                            _server_done;
  bool                            _task_done;
  std::map<std::string, Manifest> _manifest;
  std::vector<Callback*>          _callbacks;
};

}} // end of namespace scope
#endif // ICP_CLIENT_H */
