%module ismrmrd
%{
#define SWIG_FILE_WITH_INIT
#include "ismrmrd.h"
%}

%include "stdint.i"

//%include "carrays.i"
//%array_class(float, floatArray);

%ignore ISMRMRD::Acquisition::getData;

%include "ismrmrd.h"

%extend ISMRMRD::Acquisition {

    float* getData() {
        return (float*)(&($self->getData()[0]));
    }
    
}

namespace ISMRMRD {
    %template(Image_ushort) Image<unsigned short int>;
    %template(Image_float)  Image<float>;
    %template(Image_double)  Image<double>;
    %template(Image_cplx_float)  Image< std::complex<float> >;
    %template(Image_cplx_double)  Image< std::complex<double> >;
}

