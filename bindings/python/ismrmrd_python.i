%module ismrmrd

%{

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/dataset.h"
#include "numpy/arrayobject.h"

// for compatibility with Numpy version <= 1.6
#define NUMPY17_API 0x00000007
#if NPY_FEATURE_VERSION < NUMPY17_API
#define NPY_ARRAY_FARRAY NPY_FARRAY
#endif

#define SWIG_FILE_WITH_INIT

    /* exception helpers */
    static int swig_c_err_num = 0;
    static char swig_c_err_msg[256];

    const char *err_occurred()
    {
        if (swig_c_err_num) {
            swig_c_err_num = 0;
            return (const char*)swig_c_err_msg;
        }
        return NULL;
    }

    void set_err(const char *msg)
    {
        swig_c_err_num = 1;
        strncpy(swig_c_err_msg, msg, 256);
    }
%}

// Initialize NumPy
%init %{
    import_array();
%}

%include "carrays.i"

%array_class(uint16_t, ushortArray);
%array_class(int32_t, intArray);
%array_class(uint32_t, uintArray);
%array_class(uint64_t, ulongArray);
%array_class(float, floatArray);


%exception {
    const char *err;
    $action
    if ((err = err_occurred())) {
        PyErr_SetString(PyExc_RuntimeError, err);
        return NULL;
    }
}

// Process the main ISMRMRD header
%include "ismrmrd/export.h"
%include "ismrmrd/ismrmrd.h"

// These definitions are needed for the HDF5 stuff to work
// %define EXPORTISMRMRD %enddef

// Process the HDF5 ISMRMD headers
%include "ismrmrd/dataset.h"
