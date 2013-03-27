%module ismrmrd
%{
#define SWIG_FILE_WITH_INIT
#include "ismrmrd.h"
%}

%include "stdint.i"

%extend ISMRMRD::Acquisition {
    PyObject* getData() {
        PyObject *list = PyList_New(0);

        std::valarray<float> data = $self->getData();

        int i;
        for (i = 0; i < data.size(); i++) {
            PyObject *o = PyFloat_FromDouble((double)data[i]);
            PyList_Append(list, o);
            Py_DECREF(o);
        }
        return list;
    }

}

%ignore ISMRMRD::Acquisition::getData;

%include "ismrmrd.h"

namespace ISMRMRD {
    %template(Image_ushort) Image<unsigned short int>;
    %template(Image_float)  Image<float>;
    %template(Image_double)  Image<double>;
    %template(Image_cplx_float)  Image< std::complex<float> >;
    %template(Image_cplx_double)  Image< std::complex<double> >;
}

