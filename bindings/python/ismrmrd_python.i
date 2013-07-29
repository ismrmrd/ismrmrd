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

%}

%init %{
import_array();
%}

%include "stdint.i"
%include "std_string.i"
%include "std_vector.i"
%include "carrays.i"

%array_class(unsigned short, ushortArray);
%array_class(uint16_t, ushortArray);
%array_class(unsigned int, uintArray);
%array_class(uint32_t, uintArray);
%array_class(float, floatArray);

%exception {
    const char *err;
    $action
    if ((err = err_occurred())) {
        PyErr_SetString(PyExc_RuntimeError, err);
        return NULL;
    }
}

%extend ISMRMRD::Acquisition {
    PyObject* getData()
    {
        npy_intp dims[] = { $self->getData().size() };
        PyObject *array = PyArray_SimpleNew(1, dims, NPY_FLOAT);

        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        npy_intp raw_size = PyArray_NBYTES(array);

        //printf("# elements: %d\n", $self->getData().size());
        //printf("data size:  %d\n", data_size);
        //printf("total size: %d\n", raw_size);

        std::valarray<float> data = $self->getData();
        int i;
        for (i = 0; i < dims[0]; i++) {
            npy_float *addr = (npy_float*)(raw + (i * data_size));
            *addr = data[i];

            /* slower method */
            //PyObject *o = PyFloat_FromDouble((double)data[i]);
            //PyArray_SETITEM(array, addr, o);
            //Py_DECREF(o);
        }

        return array;
    }

    void setData(PyObject *array)
    {
        if (!PyArray_Check(array) || !PyArray_ISFLOAT(array)) {
            set_err("Argument to setData is not a numpy float array\n");
            return;
        } 

        int ndim = PyArray_NDIM(array);
        npy_intp *dims = PyArray_DIMS(array);
        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        if (ndim != 1) {
            set_err("Argument to setData must be a 1-D array\n");
            return;
        }

        int i;
        for (i = 0; i < dims[0]; i++) {
            npy_float *addr = (npy_float*)(raw + i * data_size);
            (*$self)[i] = (float)*addr;
        }
    }

}
%ignore ISMRMRD::Acquisition::getData;
%ignore ISMRMRD::Acquisition::setData;


//
// The Image class is overloaded so we need to do some renaming
//
%rename(Image_ushort_getData)   Image<unsigned short int>::getData;
%rename(Image_ushort_setData)   Image<unsigned short int>::setData;
%rename(Image_float_getData)    Image<float>::getData;
%rename(Image_float_setData)    Image<float>::setData;
%rename(Image_double_getData)   Image<double>::getData;
%rename(Image_double_setData)   Image<double>::setData;
%rename(Image_cxfloat_getData)  Image<cxfloat>::getData;
%rename(Image_cxfloat_setData)  Image<cxfloat>::setData;
%rename(Image_cxdouble_getData) Image<cxdouble>::getData;
%rename(Image_cxdouble_setData) Image<cxdouble>::setData;

%extend ISMRMRD::Image<unsigned short int>
{
    PyObject *getData()
    {
        npy_intp dims[] = { $self->getData().size() };
        PyObject *array = PyArray_SimpleNew(1, dims, NPY_USHORT);

        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        npy_intp raw_size = PyArray_NBYTES(array);

        int i;
        for (i = 0; i < $self->getNumberOfElements(); i++) {
            npy_ushort *addr = (npy_ushort*)(raw + (i * data_size));
            *addr = (*self)[i];
        }

        return array;
    }

    void setData(PyObject *array)
    {
        if (!PyArray_Check(array) || !PyArray_ISINTEGER(array)) {
            set_err("Argument to setData is not a numpy integer array\n");
            return;
        } 

        int ndim = PyArray_NDIM(array);
        npy_intp *dims = PyArray_DIMS(array);
        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        if (ndim != 1) {
            set_err("Argument to setData must be a 1-D array\n");
            return;
        }

        int i;
        for (i = 0; i < dims[0]; i++) {
            npy_ushort *addr = (npy_ushort*)(raw + i * data_size);
            (*$self)[i] = (unsigned short)*addr;
        }
    }
}
%ignore ISMRMRD::Image<unsigned short int>::getData;
%ignore ISMRMRD::Image<unsigned short int>::setData;

%extend ISMRMRD::Image<float>
{
    PyObject *getData()
    {
        npy_intp dims[] = { $self->getData().size() };
        PyObject *array = PyArray_SimpleNew(1, dims, NPY_FLOAT);

        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        npy_intp raw_size = PyArray_NBYTES(array);

        int i;
        for (i = 0; i < $self->getNumberOfElements(); i++) {
            npy_float *addr = (npy_float*)(raw + (i * data_size));
            *addr = (*self)[i];
        }

        return array;
    }

    void setData(PyObject *array)
    {
        if (!PyArray_Check(array) || !PyArray_ISFLOAT(array)) {
            set_err("Argument to setData is not a numpy float array\n");
            return;
        } 

        int ndim = PyArray_NDIM(array);
        npy_intp *dims = PyArray_DIMS(array);
        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        if (ndim != 1) {
            set_err("Argument to setData must be a 1-D array\n");
            return;
        }

        int i;
        for (i = 0; i < dims[0]; i++) {
            npy_float *addr = (npy_float*)(raw + i * data_size);
            (*$self)[i] = (float)*addr;
        }
    }
}
%ignore ISMRMRD::Image<float>::getData;
%ignore ISMRMRD::Image<float>::setData;

%extend ISMRMRD::Image<double>
{
    PyObject *getData()
    {
        npy_intp dims[] = { $self->getData().size() };
        PyObject *array = PyArray_SimpleNew(1, dims, NPY_DOUBLE);

        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        npy_intp raw_size = PyArray_NBYTES(array);

        int i;
        for (i = 0; i < $self->getNumberOfElements(); i++) {
            npy_double *addr = (npy_double*)(raw + (i * data_size));
            *addr = (*self)[i];
        }

        return array;
    }

    void setData(PyObject *array)
    {
        if (!PyArray_Check(array) || !PyArray_ISFLOAT(array)) {
            set_err("Argument to setData is not a numpy double array\n");
            return;
        } 

        int ndim = PyArray_NDIM(array);
        npy_intp *dims = PyArray_DIMS(array);
        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        if (ndim != 1) {
            set_err("Argument to setData must be a 1-D array\n");
            return;
        }

        int i;
        for (i = 0; i < dims[0]; i++) {
            npy_double *addr = (npy_double*)(raw + i * data_size);
            (*$self)[i] = (double)*addr;
        }
    }
}
%ignore ISMRMRD::Image<double>::getData;
%ignore ISMRMRD::Image<double>::setData;

%extend ISMRMRD::Image<cxfloat>
{
    PyObject *getData()
    {
        npy_intp dims[] = { $self->getData().size() };
        PyObject *array = PyArray_SimpleNew(1, dims, NPY_CFLOAT);

        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        npy_intp raw_size = PyArray_NBYTES(array);

        int i;
        for (i = 0; i < $self->getNumberOfElements(); i++) {
            npy_cfloat *addr = (npy_cfloat*)(raw + (i * data_size));
            addr->real = (*self)[i].real();
            addr->imag = (*self)[i].imag();
        }

        return array;
    }

    void setData(PyObject *array)
    {
        if (!PyArray_Check(array) || !PyArray_ISCOMPLEX(array)) {
            set_err("Argument to setData is not a numpy complex array\n");
            return;
        } 

        int ndim = PyArray_NDIM(array);
        npy_intp *dims = PyArray_DIMS(array);
        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        if (ndim != 1) {
            set_err("Argument to setData must be a 1-D array\n");
            return;
        }

        int i;
        for (i = 0; i < dims[0]; i++) {
            npy_cfloat *addr = (npy_cfloat*)(raw + i * data_size);
            (*$self)[i] = cxfloat(addr->real, addr->imag);
        }
    }
}
%ignore ISMRMRD::Image<cxfloat>::getData;
%ignore ISMRMRD::Image<cxfloat>::setData;

%extend ISMRMRD::Image<cxdouble>
{
    PyObject *getData()
    {
        npy_intp dims[] = { $self->getData().size() };
        PyObject *array = PyArray_SimpleNew(1, dims, NPY_CDOUBLE);

        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        npy_intp raw_size = PyArray_NBYTES(array);

        int i;
        for (i = 0; i < $self->getNumberOfElements(); i++) {
            npy_cdouble *addr = (npy_cdouble*)(raw + (i * data_size));
            addr->real = (*self)[i].real();
            addr->imag = (*self)[i].imag();
        }

        return array;
    }

    void setData(PyObject *array)
    {
        if (!PyArray_Check(array) || !PyArray_ISCOMPLEX(array)) {
            set_err("Argument to setData is not a numpy complex array\n");
            return;
        } 

        int ndim = PyArray_NDIM(array);
        npy_intp *dims = PyArray_DIMS(array);
        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        if (ndim != 1) {
            set_err("Argument to setData must be a 1-D array\n");
            return;
        }

        int i;
        for (i = 0; i < dims[0]; i++) {
            npy_cdouble *addr = (npy_cdouble*)(raw + i * data_size);
            (*$self)[i] = cxdouble(addr->real, addr->imag);
        }
    }
}
%ignore ISMRMRD::Image<cxdouble>::getData;
%ignore ISMRMRD::Image<cxdouble>::setData;


//
// The NDContainerArray class is overloaded so rename each template
//
%rename(Array_ushort_getData)   NDArrayContainer<unsigned short int>::getData;
%rename(Array_ushort_setData)   NDArrayContainer<unsigned short int>::setData;
%rename(Array_float_getData)    NDArrayContainer<float>::getData;
%rename(Array_float_setData)    NDArrayContainer<float>::setData;
%rename(Array_double_getData)   NDArrayContainer<double>::getData;
%rename(Array_double_setData)   NDArrayContainer<double>::setData;
%rename(Array_cxfloat_getData)  NDArrayContainer<cxfloat>::getData;
%rename(Array_cxfloat_setData)  NDArrayContainer<cxfloat>::setData;
%rename(Array_cxdouble_getData) NDArrayContainer<cxdouble>::getData;
%rename(Array_cxdouble_setData) NDArrayContainer<cxdouble>::setData;

%extend ISMRMRD::NDArrayContainer<unsigned short int>
{
    PyObject* getData()
    {
        int ndim = $self->dimensions_.size();
	npy_intp* dims = new npy_intp[ndim];
	for (int d=0; d<ndim; d++){
            dims[d] = $self->dimensions_[d];
        }
	PyObject *array = PyArray_New(&PyArray_Type, ndim, dims, NPY_USHORT, NULL, NULL, 0, NPY_ARRAY_FARRAY, NULL);
	delete[] dims;

        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        npy_intp raw_size = PyArray_NBYTES(array);

        int i;
        for (i = 0; i < $self->elements(); i++) {
            npy_ushort *addr = (npy_ushort*)(raw + (i * data_size));
            (*addr) = (*self)[i];
        }

        return array;
    }

    void setData(PyObject *array)
    {
        if (!PyArray_Check(array) || !PyArray_ISINTEGER(array)) {
            set_err("Argument to setData is not a numpy integer array\n");
            return;
        } 

        int ndim = PyArray_NDIM(array);
        npy_intp *dims = PyArray_DIMS(array);
        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        if (ndim != 1) {
            set_err("Argument to setData must be a 1-D array\n");
            return;
        }

        int i;
        for (i = 0; i < dims[0]; i++) {
            npy_ushort *addr = (npy_ushort*)(raw + i * data_size);
            (*$self)[i] = (unsigned short)*addr;
        }
    }
}
%ignore ISMRMRD::NDArrayContainer<unsigned short int>::getData;
%ignore ISMRMRD::NDArrayContainer<unsigned short int>::setData;

%extend ISMRMRD::NDArrayContainer<float>
{
    PyObject* getData()
    {
        int ndim = $self->dimensions_.size();
	npy_intp* dims = new npy_intp[ndim];
	for (int d=0; d<ndim; d++){
            dims[d] = $self->dimensions_[d];
        }
	PyObject *array = PyArray_New(&PyArray_Type, ndim, dims, NPY_FLOAT, NULL, NULL, 0, NPY_ARRAY_FARRAY, NULL);
	delete[] dims;

        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        npy_intp raw_size = PyArray_NBYTES(array);

        int i;
        for (i = 0; i < $self->elements(); i++) {
            npy_float *addr = (npy_float*)(raw + (i * data_size));
            (*addr) = (*self)[i];
        }

        return array;
    }

    void setData(PyObject *array)
    {
        if (!PyArray_Check(array) || !PyArray_ISFLOAT(array)) {
            set_err("Argument to setData is not a numpy float array\n");
            return;
        } 

        int ndim = PyArray_NDIM(array);
        npy_intp *dims = PyArray_DIMS(array);
        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        if (ndim != 1) {
            set_err("Argument to setData must be a 1-D array\n");
            return;
        }

        int i;
        for (i = 0; i < dims[0]; i++) {
            npy_float *addr = (npy_float*)(raw + i * data_size);
            (*$self)[i] = (float)*addr;
        }
    }
}
%ignore ISMRMRD::NDArrayContainer<float>::getData;
%ignore ISMRMRD::NDArrayContainer<float>::setData;

%extend ISMRMRD::NDArrayContainer<double>
{
    PyObject* getData()
    {
        int ndim = $self->dimensions_.size();
	npy_intp* dims = new npy_intp[ndim];
	for (int d=0; d<ndim; d++){
            dims[d] = $self->dimensions_[d];
        }
	PyObject *array = PyArray_New(&PyArray_Type, ndim, dims, NPY_DOUBLE, NULL, NULL, 0, NPY_ARRAY_FARRAY, NULL);
	delete[] dims;

        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        npy_intp raw_size = PyArray_NBYTES(array);

        int i;
        for (i = 0; i < $self->elements(); i++) {
            npy_double *addr = (npy_double*)(raw + (i * data_size));
            (*addr) = (*self)[i];
        }

        return array;
    }

    void setData(PyObject *array)
    {
        if (!PyArray_Check(array) || !PyArray_ISFLOAT(array)) {
            set_err("Argument to setData is not a numpy double array\n");
            return;
        } 

        int ndim = PyArray_NDIM(array);
        npy_intp *dims = PyArray_DIMS(array);
        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        if (ndim != 1) {
            set_err("Argument to setData must be a 1-D array\n");
            return;
        }

        int i;
        for (i = 0; i < dims[0]; i++) {
            npy_double *addr = (npy_double*)(raw + i * data_size);
            (*$self)[i] = (double)*addr;
        }
    }
}
%ignore ISMRMRD::NDArrayContainer<double>::getData;
%ignore ISMRMRD::NDArrayContainer<double>::setData;

%extend ISMRMRD::NDArrayContainer<cxfloat>
{
    PyObject* getData()
    {
        int ndim = $self->dimensions_.size();
        npy_intp* dims = new npy_intp[ndim];
        for (int d=0; d<ndim; d++){
            dims[d] = $self->dimensions_[d];
        }
	PyObject *array = PyArray_New(&PyArray_Type, ndim, dims, NPY_CFLOAT, NULL, NULL, 0, NPY_ARRAY_FARRAY, NULL);
        delete[] dims;

        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        npy_intp raw_size = PyArray_NBYTES(array);

        int i;
        for (i = 0; i < $self->elements(); i++) {
            npy_cfloat *addr = (npy_cfloat*)(raw + (i * data_size));
            addr->real = (*self)[i].real();
            addr->imag = (*self)[i].imag();
        }

        return array;
    }

    void setData(PyObject *array)
    {
        if (!PyArray_Check(array) || !PyArray_ISCOMPLEX(array)) {
            set_err("Argument to setData is not a numpy complex array\n");
            return;
        } 

        //if (PyArray_ISFORTRAN((PyArrayObject*)array)) {
        //    std::cout << "Fortran" << std::endl;
        //}
        //else {
        //    std::cout << "C" << std::endl;
        //}
	
        int ndim = PyArray_NDIM(array);
        npy_intp *dims = PyArray_DIMS(array);
        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        if (ndim != 1) {
            set_err("Argument to setData must be a 1-D array\n");
            return;
        }

        int i;
        for (i = 0; i < dims[0]; i++) {
            npy_cfloat *addr = (npy_cfloat*)(raw + i * data_size);
            (*$self)[i] = cxfloat(addr->real, addr->imag);
        }
    }
}
%ignore ISMRMRD::NDArrayContainer<cxfloat>::getData;
%ignore ISMRMRD::NDArrayContainer<cxfloat>::setData;

%extend ISMRMRD::NDArrayContainer<cxdouble>
{
    PyObject* getData()
    {
        int ndim = $self->dimensions_.size();
	npy_intp* dims = new npy_intp[ndim];
	for (int d=0; d<ndim; d++){
            dims[d] = $self->dimensions_[d];
        }
	PyObject *array = PyArray_New(&PyArray_Type, ndim, dims, NPY_CDOUBLE, NULL, NULL, 0, NPY_ARRAY_FARRAY, NULL);
	delete[] dims;

        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        npy_intp raw_size = PyArray_NBYTES(array);

        int i;
        for (i = 0; i < $self->elements(); i++) {
            npy_cdouble *addr = (npy_cdouble*)(raw + (i * data_size));
            addr->real = (*self)[i].real();
            addr->imag = (*self)[i].imag();
        }

        return array;
    }

    void setData(PyObject *array)
    {
        if (!PyArray_Check(array) || !PyArray_ISCOMPLEX(array)) {
            set_err("Argument to setData is not a numpy complex array\n");
            return;
        } 

        int ndim = PyArray_NDIM(array);
        npy_intp *dims = PyArray_DIMS(array);
        char *raw = PyArray_BYTES(array);
        int data_size = PyArray_ITEMSIZE(array);
        if (ndim != 1) {
            set_err("Argument to setData must be a 1-D array\n");
            return;
        }

        int i;
        for (i = 0; i < dims[0]; i++) {
            npy_cdouble *addr = (npy_cdouble*)(raw + i * data_size);
            (*$self)[i] = cxdouble(addr->real, addr->imag);
        }
    }
}
%ignore ISMRMRD::NDArrayContainer<cxdouble>::getData;
%ignore ISMRMRD::NDArrayContainer<cxdouble>::setData;

//
// IsmrmrdDataset
//
%rename(readImage_ushort)   readImage<unsigned short int>;
%rename(readImage_float)    readImage<float>;
%rename(readImage_double)   readImage<double>;
%rename(readImage_cxfloat)  readImage<cxfloat>;
%rename(readImage_cxdouble) readImage<cxdouble>;

%rename(readArray_ushort)   readArray<unsigned short int>;
%rename(readArray_float)    readArray<float>;
%rename(readArray_double)   readArray<double>;
%rename(readArray_cxfloat)  readArray<cxfloat>;
%rename(readArray_cxdouble) readArray<cxdouble>;

%extend ISMRMRD::IsmrmrdDataset {
    std::string readHeader() {
        boost::shared_ptr<std::string> hdr = $self->readHeader();
        return *(hdr.get()); 
    }

    ISMRMRD::Acquisition* readAcquisition(unsigned long index = 0) {
        ISMRMRD::Acquisition* acq = new ISMRMRD::Acquisition(*$self->readAcquisition(index).get());
        return acq;
    }

    ISMRMRD::ImageHeader* readImageHeader(const char* varname, unsigned long index = 0) {
        ISMRMRD::ImageHeader* imghdr = new ISMRMRD::ImageHeader(*$self->readImageHeader(varname,index).get());
        return imghdr;
    }

    // Image
    ISMRMRD::Image<float>* readImage<float>(const char* varname, unsigned long index = 0) {
        ISMRMRD::Image<float>* img = new ISMRMRD::Image<float>(*$self->readImage<float>(varname,index).get());
        return img;
    }

    ISMRMRD::Image<double>* readImage<double>(const char* varname, unsigned long index = 0) {
        ISMRMRD::Image<double>* img = new ISMRMRD::Image<double>(*$self->readImage<double>(varname,index).get());
        return img;
    }

    ISMRMRD::Image<unsigned short int>* readImage<unsigned short>(const char* varname, unsigned long index = 0) {
        ISMRMRD::Image<unsigned short>* img = new ISMRMRD::Image<unsigned short>(*$self->readImage<unsigned short>(varname,index).get());
        return img;
    }

    ISMRMRD::Image<cxfloat>* readImage<cxfloat>(const char* varname, unsigned long index = 0) {
        ISMRMRD::Image< cxfloat >* img = new ISMRMRD::Image< cxfloat >(*$self->readImage< cxfloat >(varname,index).get());
        return img;
    }

    ISMRMRD::Image<cxdouble>* readImage<cxdouble>(const char* varname, unsigned long index = 0) {
        ISMRMRD::Image<cxdouble>* img = new ISMRMRD::Image<cxdouble>(*$self->readImage<cxdouble>(varname,index).get());
        return img;
    }

    // NDArray
    ISMRMRD::NDArrayContainer<float>* readArray<float>(const char* varname, unsigned long index = 0) {
        boost::shared_ptr< ISMRMRD::NDArrayContainer<float> > tmp = $self->readArray<float>(varname,index);
        ISMRMRD::NDArrayContainer<float>* img = new ISMRMRD::NDArrayContainer<float>(tmp->dimensions_, tmp->data_);
        return img;
    }

    ISMRMRD::NDArrayContainer<double>* readArray<double>(const char* varname, unsigned long index = 0) {
        boost::shared_ptr< ISMRMRD::NDArrayContainer<double> > tmp = $self->readArray<double>(varname,index);
        ISMRMRD::NDArrayContainer<double>* img = new ISMRMRD::NDArrayContainer<double>(tmp->dimensions_, tmp->data_);
        return img;
    }

    ISMRMRD::NDArrayContainer<unsigned short>* readArray<unsigned short>(const char* varname, unsigned long index = 0) {
        boost::shared_ptr< ISMRMRD::NDArrayContainer<unsigned short> > tmp = $self->readArray<unsigned short>(varname,index);
        ISMRMRD::NDArrayContainer<unsigned short>* img = new ISMRMRD::NDArrayContainer<unsigned short>(tmp->dimensions_, tmp->data_);
        return img;
    }

    ISMRMRD::NDArrayContainer< cxfloat >* readArray< cxfloat >(const char* varname, unsigned long index = 0) {
        boost::shared_ptr< ISMRMRD::NDArrayContainer<cxfloat> >  tmp = $self->readArray< cxfloat >(varname,index);
        ISMRMRD::NDArrayContainer<cxfloat>* img = new ISMRMRD::NDArrayContainer< cxfloat >(tmp->dimensions_, tmp->data_);
        return img;
    }

    ISMRMRD::NDArrayContainer<cxdouble>* readArray<cxdouble>(const char* varname, unsigned long index = 0) {
        boost::shared_ptr< ISMRMRD::NDArrayContainer<cxdouble> > tmp = $self->readArray<cxdouble>(varname,index);
        ISMRMRD::NDArrayContainer<cxdouble>* img = new ISMRMRD::NDArrayContainer<cxdouble>(tmp->dimensions_, tmp->data_);
        return img;
    }

}
%ignore readHeader;
%ignore readAcquisition;
%ignore readImageHeader;
%ignore readImage;
%ignore readArray;

//
// The header files to process
//
%include "ismrmrd.h"

// These defines are needed for the HDF5 stuff to work
%define EXPORTISMRMRD %enddef
namespace H5 {
   
}

%include "ismrmrd_hdf5.h"
%include "ismrmrd_hdf5_datatypes.h"

// Instantiate some of the overloaded objects and methods
namespace ISMRMRD {
    // The Image and ArrayContainer types
    %template(Image_ushort)   Image<unsigned short int>;
    %template(Image_float)    Image<float>;
    %template(Image_double)   Image<double>;
    %template(Image_cxfloat)  Image<cxfloat>;
    %template(Image_cxdouble) Image<cxdouble>;

    %template(Array_ushort)   NDArrayContainer<unsigned short int>;
    %template(Array_float)    NDArrayContainer<float>;
    %template(Array_double)   NDArrayContainer<double>;
    %template(Array_cxfloat)  NDArrayContainer<cxfloat>;
    %template(Array_cxdouble) NDArrayContainer<cxdouble>;

    // The Image and ArrayContainer Append methods
    %template(appendImage_ushort)   IsmrmrdDataset::appendImage<unsigned short int>;
    %template(appendImage_float)    IsmrmrdDataset::appendImage<float>;
    %template(appendImage_double)   IsmrmrdDataset::appendImage<double>;
    %template(appendImage_cxfloat)  IsmrmrdDataset::appendImage<cxfloat>;
    %template(appendImage_cxdouble) IsmrmrdDataset::appendImage<cxdouble>;

    %template(appendArray_ushort)   IsmrmrdDataset::appendArray<unsigned short int>;
    %template(appendArray_float)    IsmrmrdDataset::appendArray<float>;
    %template(appendArray_double)   IsmrmrdDataset::appendArray<double>;
    %template(appendArray_cxfloat)  IsmrmrdDataset::appendArray<cxfloat>;
    %template(appendArray_cxdouble) IsmrmrdDataset::appendArray<cxdouble>;

}
