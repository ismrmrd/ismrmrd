#include <iostream>
#include "ismrmrd_hdf5.h"
#include "ismrmrd.hxx"

template <T> random_fill () {
}

int main (int args, char** argv) {

    std::vector <unsigned> dims;
    dims.push_back (10);
    dims.push_back (8);
    dims.push_back (7);
    
    NDArrayContainer<float> ndcs (dims);

    return 0;

    
}
