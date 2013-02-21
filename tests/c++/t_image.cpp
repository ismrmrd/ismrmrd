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
    
    Image<std::complex<float> > img_s (dims[0], dims[1], dims[2], dims[3]);

    if (img_s.getNumberOfElements() != img_s.getData().size()) // dimensions and data size match?
        return 1;
    
    /* This test should no longer be needed.
    size_t img_s_gne = img_s.getNumberOfElements();
    size_t img_s_e   = img_s.elements();

    if (img_s_gne != img_s_e) // header and data match?
        return 1;
    */
    
    return 0;
    
}
