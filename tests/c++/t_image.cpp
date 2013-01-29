#include <iostream>
#include "ismrmrd_hdf5.h"
#include "ismrmrd.hxx"

int main (int args, char** argv) {

    using namespace ISMRMRD;
    
    std::vector <unsigned> dims;
    dims.push_back (32); 
    dims.push_back (64);
    dims.push_back (1);
    dims.push_back (16); // channels
    
    Image<std::complex<float> > img_s (dims);

    if (!img_s.is_consistent()) // dimensions and data size match?
        return 1;
    
    size_t img_s_gne = img_s.getNumberOfElements();
    size_t img_s_e   = img_s.elements();

    if (img_s_gne != img_s_e) // header and data match?
        return 1;
    
    return 0;
    
}
