#ifndef ICP_CLIENTCALLBACKS_H
#define ICP_CLIENTCALLBACKS_H

#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/xml.h"
#include "icpClient.h"
#include "icpCallback.h"

namespace ISMRMRD { namespace ICP
{
/*******************************************************************************
 ******************************************************************************/
class ClientEntityHandler : public Callback
{
  public:
          ClientEntityHandler (Client*);
          ~ClientEntityHandler () = default;
  void    receive (Callback*, ENTITY*);

  private:

  Client* _client;
};

/*******************************************************************************
 ******************************************************************************/
class ClientImageProcessor : public Callback
{
  public:
       ClientImageProcessor (Client*,
                             std::string fname,
                             std::string dname,
                             std::mutex& mtx);
       ~ClientImageProcessor () = default;
  void receive (Callback*, ENTITY*);

  private:

  template<typename S>
  void writeImage (ISMRMRD::Entity* entity)
  {
    ISMRMRD::Image<S>* img = static_cast<ISMRMRD::Image<S>*>(entity);
    {
      std::lock_guard<std::mutex> guard (_mtx);
      _dset.appendImage (*img, img->getStream());
    }
  }

  Client*             _client;
  ISMRMRD::Dataset       _dset;
  std::mutex&            _mtx;
};

/*******************************************************************************
 ******************************************************************************/

}} //end of namespace scope
#endif // ICP_CLIENTCALLBACKS_H */
