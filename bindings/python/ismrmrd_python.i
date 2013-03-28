%module ismrmrd

%{
#include "ismrmrd_hdf5.h"
%}

#ifdef SWIGPYTHON
%extend ISMRMRD::Acquisition {
    PyObject* getData()
    {
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

#endif


%include "../ismrmrd.i"
