#ifdef WIN32 
#include <windows.h>
#else 
#include <sys/time.h>
#endif

#include <iostream>
#include <string>

#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/dataset.h"


class Timer
{
public:

  Timer() { Timer("Timer"); }

  Timer(const char* name) : name_(name) {
    pre();
#ifdef WIN32
    QueryPerformanceFrequency(&frequency_);
    QueryPerformanceCounter(&start_);
#else
    gettimeofday(&start_, NULL);
#endif
  }

  virtual ~Timer() {
    double time_in_us = 0.0;
    post();
#ifdef WIN32
    QueryPerformanceCounter(&end_);
    time_in_us = (end_.QuadPart * (1.0e6/ frequency_.QuadPart)) - start_.QuadPart * (1.0e6 / frequency_.QuadPart);
#else
    gettimeofday(&end_, NULL);
    time_in_us = ((end_.tv_sec * 1e6) + end_.tv_usec) - ((start_.tv_sec * 1e6) + start_.tv_usec);
#endif
    std::cout << name_ << ": " << time_in_us/1000.0 << " ms" << std::endl; std::cout.flush();
  }

  virtual void pre() { }
  virtual void post() { }

protected:

#ifdef WIN32
  LARGE_INTEGER frequency_;
  LARGE_INTEGER start_;
  LARGE_INTEGER end_;
#else
  timeval start_;
  timeval end_;
#endif

  std::string name_;
};


int main(int argc, char** argv)
{
  std::cout << "File reader timing test" << std::endl;

  if (argc != 2) {
    std::cout << "Usage: " << std::endl;
    std::cout << "  " << argv[0] << " <FILENAME>" << std::endl;
  } 

  std::cout << "Opening file " << argv[1] << std::endl;


  {
    Timer t("READ TIMER");
    ISMRMRD::Dataset d(argv[1],"dataset", false);
    uint32_t number_of_acquisitions = d.getNumberOfAcquisitions();
    ISMRMRD::Acquisition acq;
    for (uint32_t i = 0; i < number_of_acquisitions; i++) {
        d.readAcquisition(i, acq);
        //We'll just throw the data away here. 
    }
  }
  
  return 0;
}
