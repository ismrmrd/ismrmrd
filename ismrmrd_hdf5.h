#prama once
#ifndef ISMRMRD_HDF5_H
#define ISMRMRD_HDF5_H

#include "ismrmrd_export.h"
#include "ismrmrd.h"

#include <boost/shared_ptr.hpp>


namespace ISMRMRD
{

  EXPORTISMRMRD int H5AppendAcquisition(Acquisition* a, const char* filename, const char* varname);
  EXPORTISMRMRD boost::shared_ptr<Acquisition> H5ReadAcquisition(const char* filename, const char* varname, unsigned long index = 0);



} //end of ISMRMRD namespace

#endif //ISMRMRD_HDF5_H
