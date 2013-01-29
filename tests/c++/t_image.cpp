#include <iostream>
#include "ismrmrd_hdf5.h"
#include "ismrmrd.hxx"



int main (int args, char** argv) {

    using namespace ISMRMRD;
    
    std::vector <unsigned> dims;
    dims.push_back (32);
    dims.push_back (64);
    
    Image<std::complex<float> > img_s (dims);

    return 0;

    
}
