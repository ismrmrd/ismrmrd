%module ismrmrd

%{
#include "ismrmrd_hdf5.h"
#include "numpy/arrayobject.h"

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

    // typedef for complex numbers
    typedef std::complex<float>  cxfloat;
    typedef std::complex<double> cxdouble;


    template <typename T> PyObject *make_image_array(boost::shared_ptr< ISMRMRD::NDArrayContainer<T> > cont, unsigned int numpy_type)
    {
        size_t all_dims = cont->dimensions_.size();
        size_t ndim = cont->ndims();

        npy_intp* dims = new npy_intp[ndim];
	for (int d = 0; d < all_dims; d++){
            int dimension = cont->dimensions_[d];
            if (dimension > 1) {
                dims[d] = dimension;
            }
        }
	PyObject *array = PyArray_New(&PyArray_Type, ndim, dims, numpy_type, NULL, NULL, 0, NPY_ARRAY_FARRAY, NULL);
	delete[] dims;

        return array;
    }

    template <typename T> PyObject* readTArray(ISMRMRD::IsmrmrdDataset *dset, const char* varname, unsigned long index, unsigned int numpy_type)
    {
        boost::shared_ptr< ISMRMRD::NDArrayContainer<T> > container = dset->readArray<T>(varname,index);
        PyObject *array = make_image_array<T>(container, numpy_type);

        T *raw = (T*)PyArray_DATA(array);
        npy_intp raw_size = PyArray_NBYTES(array);

        memcpy(raw, &container->data_[0], raw_size);

        return array;
    }

%}

// Initialize NumPy
%init %{
    import_array();
%}

%include "stdint.i"
%include "std_string.i"
%include "std_vector.i"
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

%ignore *::operator=;
%ignore *::operator[];
%ignore ISMRMRD::AcquisitionHeader_with_data;
%ignore ISMRMRD::ImageHeader_with_data;
%ignore ISMRMRD::Image;
%ignore ISMRMRD::NDArrayContainer;


%extend ISMRMRD::Acquisition {
    /* Returns 1-D Numpy Array */
    PyObject* getData()
    {
        npy_intp dims[] = { $self->getData().size() };
        PyObject *array = PyArray_SimpleNew(1, dims, NPY_FLOAT);

        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        npy_intp raw_size = PyArray_NBYTES(array);

        std::valarray<float> data = $self->getData();
        memcpy(raw, &data[0], dims[0] * data_size);

        return array;
    }

    void setData(PyObject *array)
    {
        if (!PyArray_Check(array) || !PyArray_ISFLOAT(array)) {
            set_err("Argument to setData is not a numpy float array\n");
            return;
        } else if (!PyArray_ISBEHAVED_RO(array)) {
            set_err("Argument to setData must be aligned\n");
            return;
        } else if (!PyArray_ISONESEGMENT(array)) {
            set_err("Data is not one segment\n");
            return;
        }

        int ndim = PyArray_NDIM(array);
        int itemsize = PyArray_ITEMSIZE(array);
        npy_intp nbytes = PyArray_NBYTES(array);
        npy_intp nelements = PyArray_SIZE(array);
        npy_intp* dims = PyArray_DIMS(array);
        npy_intp* strides = PyArray_STRIDES(array);
        void *raw = PyArray_DATA(array);

        std::valarray<float> data(0.0, nelements);
        memcpy(&(data[0]), raw, nbytes);
        $self->setData(data);
    }
}

%ignore ISMRMRD::Acquisition::getData;
%ignore ISMRMRD::Acquisition::setData;


// IsmrmrdDataset

%extend ISMRMRD::IsmrmrdDataset {

    // XML
    std::string readHeader() {
        boost::shared_ptr<std::string> hdr = $self->readHeader();
        return *(hdr.get()); 
    }

    // Acquisition
    ISMRMRD::Acquisition* readAcquisition(unsigned long index = 0) {
        ISMRMRD::Acquisition* acq = new ISMRMRD::Acquisition(*$self->readAcquisition(index).get());
        return acq;
    }

    // Image Header
    ISMRMRD::ImageHeader* readImageHeader(const char* varname, unsigned long index = 0) {
        ISMRMRD::ImageHeader* imghdr = new ISMRMRD::ImageHeader(*$self->readImageHeader(varname,index).get());
        return imghdr;
    }

    // Image Data
    PyObject* readArray(ISMRMRD::ImageHeader *hdr, const char* varname, unsigned long index = 0)
    {
        PyObject *array = NULL;
        if (!hdr) {
            return NULL;
        }

        switch (hdr->image_data_type) {
        case ISMRMRD::DATA_UNSIGNED_SHORT: {
            return readTArray<unsigned short>($self, varname, index, NPY_USHORT);
            break;
        }
        case ISMRMRD::DATA_FLOAT: {
            return readTArray<float>($self, varname, index, NPY_FLOAT);
            break;
        }
        case ISMRMRD::DATA_DOUBLE: {
            return readTArray<double>($self, varname, index, NPY_DOUBLE);
            break;
        }
        case ISMRMRD::DATA_COMPLEX_FLOAT: {
            return readTArray<cxfloat>($self, varname, index, NPY_CFLOAT);
            break;
        }
        case ISMRMRD::DATA_COMPLEX_DOUBLE: {
            return readTArray<cxdouble>($self, varname, index, NPY_CDOUBLE);
            break;
        }
        default:
            array = NULL;
        }
        return array;
    }

    int appendArray(PyObject *array, const char* varname)
    {
        if (!PyArray_Check(array)) {
            set_err("Array arg to appendArray is not a numpy array\n");
            return -1;
        } else if (!PyArray_ISBEHAVED_RO(array)) {
            set_err("Array arg to appendArray must be aligned and in machine byte-order\n");
            return -1;
        }

        int ndim = PyArray_NDIM(array);
        npy_intp* dims = PyArray_DIMS(array);

        std::vector<unsigned int> dimensions;
        for (int d = 0; d < ndim; d++) {
            dimensions.push_back(dims[d]);
        }

        int ret = 0;
        switch (PyArray_TYPE(array)) {
            case NPY_USHORT: {
                npy_ushort *raw = (npy_ushort*)PyArray_DATA(array);
                ret = $self->appendArray(dimensions, raw, varname);
                break;
            }
            case NPY_FLOAT: {
                npy_float *raw = (npy_float*)PyArray_DATA(array);
                ret = $self->appendArray(dimensions, raw, varname);
                break;
            }
            case NPY_DOUBLE: {
                npy_double *raw = (npy_double*)PyArray_DATA(array);
                ret = $self->appendArray(dimensions, raw, varname);
                break;
            }
            case NPY_CFLOAT: {
                npy_cfloat *raw = (npy_cfloat*)PyArray_DATA(array);
                ret = $self->appendArray(dimensions, (std::complex<float>*)raw, varname);
                break;
            }
            case NPY_CDOUBLE: {
                npy_cdouble *raw = (npy_cdouble*)PyArray_DATA(array);
                ret = $self->appendArray(dimensions, (std::complex<float>*)raw, varname);
                break;
            }
            default:
                set_err("Invalid array dtype\n");
                return -1;
        }
        return ret;
    }

}

%ignore readHeader;
%ignore readAcquisition;
%ignore readImageHeader;
%ignore readImage;
%ignore readArray;
%ignore appendImage;
%ignore appendArray;

// Process the main ISMRMRD header
%include "ismrmrd.h"

// These definitions are needed for the HDF5 stuff to work
%define EXPORTISMRMRD %enddef
namespace H5 { }

// Process the HDF5 ISMRMD headers
%include "ismrmrd_hdf5.h"
%include "ismrmrd_hdf5_datatypes.h"
