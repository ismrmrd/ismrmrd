#ifndef ICPMTQUEUE_H
#define ICPMTQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

#include <iostream>

namespace ISMRMRD { namespace ICP
{

/*******************************************************************************
 ******************************************************************************/
template <typename T>
class MTQueue
{
  public:

  MTQueue ()  = default;
  ~MTQueue () = default;

  /****************************************************************************/
  void push (const T& data)
  {
    std::unique_lock<std::mutex> lock (_mtx);
    _que.push (data);
    lock.unlock ();
    _cvar.notify_one ();
  }

  /****************************************************************************/
  bool pop (T& data)
  {
    std::unique_lock<std::mutex> lock (_mtx);
  
    while (_que.empty () && !_stop)
    {
      _cvar.wait (lock);
    }
  
    if (_stop)
    {
      _stop = false;
      return false;
    }

    data = _que.front ();
    _que.pop ();
  
    return true;
  }

  /****************************************************************************/
  void stop ()
  {
    if (!_que.empty())
    {
      usleep ((unsigned int)5000);
    }

    _stop = true;
    _cvar.notify_one ();
  }

  /****************************************************************************/
  MTQueue (const MTQueue&)             = delete;
  MTQueue& operator = (const MTQueue&) = delete;

  private:

  bool                     _stop;
  std::queue<T>            _que;
  std::mutex               _mtx;
  std::condition_variable  _cvar;
};

}} // end of namespace ISMRMRD::ICP
#endif //ICPMTQUEUE_H
