#ifndef ICP_CALLBACK_H
#define ICP_CALLBACK_H

namespace ISMRMRD { namespace ICP
{

class Callback
{
  public:
  virtual      ~Callback() = default;
  virtual void receive (Callback* instance, Entity* entity) = 0;
};

}} // end of namespace
#endif //ICP_CALLBACK_H
