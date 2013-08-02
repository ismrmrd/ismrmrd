//
// The ismrmrd module
//
%module ismrmrd

//
// The includes and definitions
//  not processed by SWIG
//
%{

// The main ismrmrd header stuff including files
#include "ismrmrd_hdf5.h"

// The JNI includes and env functions
#include "jni.h"

static JavaVM *cached_jvm = 0;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
    cached_jvm = jvm;
    return JNI_VERSION_1_2;
}

static JNIEnv * JNU_GetEnv() {
    JNIEnv *env;
    jint rc = cached_jvm->GetEnv((void **)&env, JNI_VERSION_1_2);
    if (rc == JNI_EDETACHED)
        throw std::runtime_error("current thread not attached");
    if (rc == JNI_EVERSION)
        throw std::runtime_error("jni version not supported");
    return env;
}

// typedef for complex numbers
typedef std::complex<float>  cxfloat;
typedef std::complex<double> cxdouble;

%}

//
// Use some of the stock swig.i files
//
%include "stdint.i"
%include "std_string.i"

// We have a std::vector<unsigned int> in the NDArrayContainer
%include "std_vector.i"
%template(vector_uint) std::vector<unsigned int>;

// We have arrays of fixed size in the EncodingCounters,
// the AcquisitionHeader and the ImageHeader structs.
// They are of type float, int16_t, uint32_t.
// This way of wrapping seems to do a nice job of handling these
// arrays in a "Java/Matlab" like way.
%include "arrays_java.i"
%apply float[]    {float *};
%apply int16_t[]  {int16_t *};
%apply uint32_t[] {uint32_t *};

%ignore *::operator=;
%ignore *::operator[];

//
// Define some extra methods for serializing and deserializing the header structures
//
%inline {
    jbyteArray acquisitionHeaderToJBytes(ISMRMRD::AcquisitionHeader* acqhdr) {
        JNIEnv *env = JNU_GetEnv();
        jbyteArray arr = env->NewByteArray(sizeof(ISMRMRD::AcquisitionHeader));
	char* tmp = reinterpret_cast<char*>(acqhdr);
        if (arr != NULL) {
            jbyte *ptr = env->GetByteArrayElements(arr, NULL);
            jint i;
            for (i = 0; i < sizeof(ISMRMRD::AcquisitionHeader); i++) {
                ptr[i] = tmp[i];
            }
            env->ReleaseByteArrayElements(arr, ptr, JNI_COMMIT);
        }
        return arr;
    }

    jbyteArray imageHeaderToJBytes(ISMRMRD::ImageHeader* acqhdr) {
        JNIEnv *env = JNU_GetEnv();
        jbyteArray arr = env->NewByteArray(sizeof(ISMRMRD::ImageHeader));
	char* tmp = reinterpret_cast<char*>(acqhdr);
        if (arr != NULL) {
            jbyte *ptr = env->GetByteArrayElements(arr, NULL);
            jint i;
            for (i = 0; i < sizeof(ISMRMRD::ImageHeader); i++) {
                ptr[i] = tmp[i];
            }
            env->ReleaseByteArrayElements(arr, ptr, JNI_COMMIT);
        }
        return arr;
    }

    void copyJBytesToAcquisitionHeader(jbyteArray arr, ISMRMRD::AcquisitionHeader* acqhdr) {
        JNIEnv *env = JNU_GetEnv();
        jbyte *ptr = env->GetByteArrayElements(arr, NULL);
        memcpy(acqhdr, ptr, sizeof(ISMRMRD::AcquisitionHeader));
        env->ReleaseByteArrayElements(arr, ptr, JNI_COMMIT);
    }

    void copyJBytesToImageHeader(jbyteArray arr, ISMRMRD::ImageHeader* imghdr) {
        JNIEnv *env = JNU_GetEnv();
        jbyte *ptr = env->GetByteArrayElements(arr, NULL);
        memcpy(imghdr, ptr, sizeof(ISMRMRD::ImageHeader));
        env->ReleaseByteArrayElements(arr, ptr, JNI_COMMIT);
    }

}

//
// The Overrides
//

//
// Acquisition
//
%extend ISMRMRD::Acquisition {

    jfloatArray getData() {
        JNIEnv *env = JNU_GetEnv();
        uint32_t nelem = 2*$self->getActiveChannels()*$self->getNumberOfSamples();
        jfloatArray arr = env->NewFloatArray(nelem);
        if (arr != NULL) {
            float *ptr = env->GetFloatArrayElements(arr, NULL);
            jint i;
            for (i = 0; i < nelem; i++) {
                ptr[i] = (*$self)[i];
            }
            env->ReleaseFloatArrayElements(arr, ptr, JNI_COMMIT);
        }
        return arr;
    }

    void setData(jfloatArray arr) {
        // Get the environment
        JNIEnv *env = JNU_GetEnv();
        // Get the size of the input array and a pointer to it
	jsize len  = env->GetArrayLength(arr);
        float *ptr = env->GetFloatArrayElements(arr, NULL);
        // TODO: make sure that the user has set the proper size.
        for (jint i = 0; i < len; i++) {
            (*$self)[i] = ptr[i];
        }
        env->ReleaseFloatArrayElements(arr, ptr, JNI_COMMIT);
    }

    jfloatArray getTraj() {
        JNIEnv *env = JNU_GetEnv();
        uint32_t nelem = $self->getTrajectoryDimensions()*$self->getNumberOfSamples();
        jfloatArray arr = env->NewFloatArray(nelem);
        if (arr != NULL) {
            float *ptr = env->GetFloatArrayElements(arr, NULL);
            jint i;
            for (i = 0; i < nelem; i++) {
                ptr[i] = $self->getTraj()[i];
            }
            env->ReleaseFloatArrayElements(arr, ptr, JNI_COMMIT);
        }
        return arr;
    }

    void setTraj(jfloatArray arr) {
        // Get the environment
        JNIEnv *env = JNU_GetEnv();
        // Get the size of the input array and a pointer to it
	jsize len  = env->GetArrayLength(arr);
        float *ptr = env->GetFloatArrayElements(arr, NULL);
	std::valarray<float> tmp(ptr,len);
        // TODO: make sure that the user has set the proper size.
	$self->setTraj(tmp);
        env->ReleaseFloatArrayElements(arr, ptr, JNI_COMMIT);
    }
}
%ignore ISMRMRD::Acquisition::getData;
%ignore ISMRMRD::Acquisition::setData;
%ignore ISMRMRD::Acquisition::getTraj;
%ignore ISMRMRD::Acquisition::setTraj;

//
// The Image and NDContainerArray classes are overloaded
// so we need to do some renaming
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

//
// Image<ushort>
//
%extend ISMRMRD::Image<unsigned short int> {

    jcharArray getData() {
        JNIEnv *env = JNU_GetEnv();
        jcharArray arr = env->NewCharArray($self->getNumberOfElements());
        if (arr != NULL) {
            unsigned short int *ptr = env->GetCharArrayElements(arr, NULL);
            for (jint i = 0; i < $self->getNumberOfElements(); i++) {
                ptr[i] = (*$self)[i];
            }
            env->ReleaseCharArrayElements(arr, ptr, JNI_COMMIT);
        }
        return arr;
    }

    void setData(jcharArray arr) {
        // Get the environment
        JNIEnv *env = JNU_GetEnv();
        // Get the size of the input array and a pointer to it
	jsize len  = env->GetArrayLength(arr);
        unsigned short int *ptr = env->GetCharArrayElements(arr, NULL);
        // TODO: make sure that the user has set the proper size.
        for (jint i = 0; i < len; i++) {
            (*$self)[i] = ptr[i];
        }
        env->ReleaseCharArrayElements(arr, ptr, JNI_COMMIT);
    }

}
%ignore ISMRMRD::Image<unsigned short int>::getData;
%ignore ISMRMRD::Image<unsigned short int>::setData;

//
// Image<float>
//
%extend ISMRMRD::Image<float> {

    jfloatArray getData() {
        JNIEnv *env = JNU_GetEnv();
        jfloatArray arr = env->NewFloatArray($self->getNumberOfElements());
        if (arr != NULL) {
            float *ptr = env->GetFloatArrayElements(arr, NULL);
            for (jint i = 0; i < $self->getNumberOfElements(); i++) {
                ptr[i] = (*$self)[i];
            }
            env->ReleaseFloatArrayElements(arr, ptr, JNI_COMMIT);
        }
        return arr;
    }

    void setData(jfloatArray arr) {
        // Get the environment
        JNIEnv *env = JNU_GetEnv();
        // Get the size of the input array and a pointer to it
	jsize len  = env->GetArrayLength(arr);
        float *ptr = env->GetFloatArrayElements(arr, NULL);
        // TODO: make sure that the user has already set the proper size.
        for (jint i = 0; i < len; i++) {
            (*$self)[i] = ptr[i];
        }
        env->ReleaseFloatArrayElements(arr, ptr, JNI_COMMIT);
    }

}
%ignore ISMRMRD::Image<float>::getData;
%ignore ISMRMRD::Image<float>::setData;

//
// Image<double>
//
%extend ISMRMRD::Image<double> {

    jdoubleArray getData() {
        JNIEnv *env = JNU_GetEnv();
        jdoubleArray arr = env->NewDoubleArray($self->getNumberOfElements());
        if (arr != NULL) {
            double *ptr = env->GetDoubleArrayElements(arr, NULL);
            for (jint i = 0; i < $self->getNumberOfElements(); i++) {
                ptr[i] = (*$self)[i];
            }
            env->ReleaseDoubleArrayElements(arr, ptr, JNI_COMMIT);
        }
        return arr;
    }

    void setData(jdoubleArray arr) {
        // Get the environment
        JNIEnv *env = JNU_GetEnv();
        // Get the size of the input array and a pointer to it
	jsize len  = env->GetArrayLength(arr);
        double *ptr = env->GetDoubleArrayElements(arr, NULL);
        // TODO: make sure that the user has set the proper size.
        for (jint i = 0; i < len; i++) {
            (*$self)[i] = ptr[i];
        }
        env->ReleaseDoubleArrayElements(arr, ptr, JNI_COMMIT);
    }

}
%ignore ISMRMRD::Image<double>::getData;
%ignore ISMRMRD::Image<double>::setData;

//
// Image<cxfloat>
//
%extend ISMRMRD::Image<cxfloat> {

    jfloatArray getData() {
        JNIEnv *env = JNU_GetEnv();
        jfloatArray arr = env->NewFloatArray(2*$self->getNumberOfElements());
        if (arr != NULL) {
            float *ptr = env->GetFloatArrayElements(arr, NULL);
            for (jint i = 0; i < $self->getNumberOfElements(); i++) {
                ptr[2*i]   = (*self)[i].real();
                ptr[2*i+1] = (*self)[i].imag();
            }
            env->ReleaseFloatArrayElements(arr, ptr, JNI_COMMIT);
        }
        return arr;
    }

    void setData(jfloatArray arr) {
        // Get the environment
        JNIEnv *env = JNU_GetEnv();
        // Get the size of the input array and a pointer to it
	jsize len  = env->GetArrayLength(arr)/2;
        float *ptr = env->GetFloatArrayElements(arr, NULL);
        // TODO: make sure that the user has set the proper size.
        for (jint i = 0; i < len; i++) {
            (*$self)[i] = cxfloat(ptr[2*i],ptr[2*i+1]);
        }
        env->ReleaseFloatArrayElements(arr, ptr, JNI_COMMIT);
    }

}
%ignore ISMRMRD::Image<cxfloat>::getData;
%ignore ISMRMRD::Image<cxfloat>::setData;

//
// Image<cxdouble>
//
%extend ISMRMRD::Image<cxdouble> {

    jdoubleArray getData() {
        JNIEnv *env = JNU_GetEnv();
        jdoubleArray arr = env->NewDoubleArray(2*$self->getNumberOfElements());
        if (arr != NULL) {
            double *ptr = env->GetDoubleArrayElements(arr, NULL);
            for (jint i = 0; i < $self->getNumberOfElements(); i++) {
                ptr[2*i]   = (*$self)[i].real();
                ptr[2*i+1] = (*$self)[i].imag();
            }
            env->ReleaseDoubleArrayElements(arr, ptr, JNI_COMMIT);
        }
        return arr;
    }

    void setData(jdoubleArray arr) {
        // Get the environment	   
        JNIEnv *env = JNU_GetEnv();
        // Get the size of the input array and a pointer to it
	jsize len  = env->GetArrayLength(arr)/2;
        double *ptr = env->GetDoubleArrayElements(arr, NULL);
        // TODO: make sure that the user has set the proper size.
        for (jint i = 0; i < len; i++) {
            (*$self)[i] = cxdouble(ptr[2*i],ptr[2*i+1]);
        }
        env->ReleaseDoubleArrayElements(arr, ptr, JNI_COMMIT);
    }

}
%ignore ISMRMRD::Image<cxdouble>::getData;
%ignore ISMRMRD::Image<cxdouble>::setData;

//
// NDArrayContainer<ushort>
//
%extend ISMRMRD::NDArrayContainer<unsigned short int> {

    jcharArray getData() {
        JNIEnv *env = JNU_GetEnv();
        jcharArray arr = env->NewCharArray($self->elements());
        if (arr != NULL) {
            unsigned short int *ptr = env->GetCharArrayElements(arr, NULL);
            for (jint i = 0; i < $self->elements(); i++) {
                ptr[i] = $self->data_[i];
            }
            env->ReleaseCharArrayElements(arr, ptr, JNI_COMMIT);
        }
        return arr;
    }

    void setData(jcharArray arr) {
        // Get the environment
        JNIEnv *env = JNU_GetEnv();
        // Get the size of the input array and a pointer to it
	jsize len  = env->GetArrayLength(arr);
        unsigned short int *ptr = env->GetCharArrayElements(arr, NULL);
        // TODO: make sure that the user has set the proper size.
        for (jint i = 0; i < len; i++) {
            (*self)[i] = ptr[i];
        }
        env->ReleaseCharArrayElements(arr, ptr, JNI_COMMIT);
    }

}
%ignore ISMRMRD::NDArrayContainer<unsigned short int>::getData;
%ignore ISMRMRD::NDArrayContainer<unsigned short int>::setData;

//
// NDArrayContainer<float>
//
%extend ISMRMRD::NDArrayContainer<float> {

    jfloatArray getData() {
        JNIEnv *env = JNU_GetEnv();
        jfloatArray arr = env->NewFloatArray($self->elements());
        if (arr != NULL) {
            float *ptr = env->GetFloatArrayElements(arr, NULL);
            for (jint i = 0; i < $self->elements(); i++) {
                ptr[i] = $self->data_[i];
            }
            env->ReleaseFloatArrayElements(arr, ptr, JNI_COMMIT);
        }
        return arr;
    }

    void setData(jfloatArray arr) {
        // Get the environment
        JNIEnv *env = JNU_GetEnv();
        // Get the size of the input array and a pointer to it
	jsize len  = env->GetArrayLength(arr);
        float *ptr = env->GetFloatArrayElements(arr, NULL);
        // TODO: make sure that the user has set the proper size.
        for (jint i = 0; i < len; i++) {
            (*$self)[i] = ptr[i];
        }
        env->ReleaseFloatArrayElements(arr, ptr, JNI_COMMIT);
    }

}
%ignore ISMRMRD::NDArrayContainer<float>::getData;
%ignore ISMRMRD::NDArrayContainer<float>::setData;

//
// NDArrayContainer<double>
//
%extend ISMRMRD::NDArrayContainer<double> {

    jdoubleArray getData() {
        JNIEnv *env = JNU_GetEnv();
        jdoubleArray arr = env->NewDoubleArray($self->elements());
        if (arr != NULL) {
            double *ptr = env->GetDoubleArrayElements(arr, NULL);
            for (jint i = 0; i < $self->elements(); i++) {
                ptr[i] = $self->data_[i];
            }
            env->ReleaseDoubleArrayElements(arr, ptr, JNI_COMMIT);
        }
        return arr;
    }

    void setData(jdoubleArray arr) {
        // Get the environment
        JNIEnv *env = JNU_GetEnv();
        // Get the size of the input array and a pointer to it
	jsize len  = env->GetArrayLength(arr);
        double *ptr = env->GetDoubleArrayElements(arr, NULL);
        // TODO: make sure that the user has set the proper size.
        for (jint i = 0; i < len; i++) {
            (*self)[i] = ptr[i];
        }
        env->ReleaseDoubleArrayElements(arr, ptr, JNI_COMMIT);
    }

}
%ignore ISMRMRD::NDArrayContainer<double>::getData;
%ignore ISMRMRD::NDArrayContainer<double>::setData;

//
// NDArrayContainer<cxfloat>
//
%extend ISMRMRD::NDArrayContainer<cxfloat> {

    jfloatArray getData() {
        JNIEnv *env = JNU_GetEnv();
        jfloatArray arr = env->NewFloatArray(2*($self->elements()));
        if (arr != NULL) {
            float *ptr = env->GetFloatArrayElements(arr, NULL);
            for (jint i = 0; i < $self->elements(); i++) {
                ptr[2*i]   = $self->data_[i].real();
                ptr[2*i+1] = $self->data_[i].imag();
            }
            env->ReleaseFloatArrayElements(arr, ptr, JNI_COMMIT);
        }
        return arr;
    }

    void setData(jfloatArray arr) {
        // Get the environment
        JNIEnv *env = JNU_GetEnv();
        // Get the size of the input array and a pointer to it
	jsize len  = env->GetArrayLength(arr)/2;
        float *ptr = env->GetFloatArrayElements(arr, NULL);
        // TODO: make sure that the user has set the proper size.
        for (jint i = 0; i < len; i++) {
            (*$self)[i] = cxfloat(ptr[2*i],ptr[2*i+1]);
        }
        env->ReleaseFloatArrayElements(arr, ptr, JNI_COMMIT);
    }

}
%ignore ISMRMRD::NDArrayContainer<cxfloat>::getData;
%ignore ISMRMRD::NDArrayContainer<cxfloat>::setData;

//
// NDArrayContainer<cxdouble>
//
%extend ISMRMRD::NDArrayContainer<cxdouble> {

    jdoubleArray getData() {
        JNIEnv *env = JNU_GetEnv();
        jdoubleArray arr = env->NewDoubleArray(2*($self->elements()));
        if (arr != NULL) {
            double *ptr = env->GetDoubleArrayElements(arr, NULL);
            for (jint i = 0; i < $self->elements(); i++) {
                ptr[2*i]   = $self->data_[i].real();
                ptr[2*i+1] = $self->data_[i].imag();
            }
            env->ReleaseDoubleArrayElements(arr, ptr, JNI_COMMIT);
        }
        return arr;
    }

    void setData(jdoubleArray arr) {
        // Get the environment
        JNIEnv *env = JNU_GetEnv();
        // Get the size of the input array and a pointer to it
	jsize len  = env->GetArrayLength(arr)/2;
        double *ptr = env->GetDoubleArrayElements(arr, NULL);
        // TODO: make sure that the user has set the proper size.
        for (jint i = 0; i < len; i++) {
            (*$self)[i] = cxdouble(ptr[2*i],ptr[2*i+1]);
        }
        env->ReleaseDoubleArrayElements(arr, ptr, JNI_COMMIT);
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

//%rename(appendImage_float)    appendImage<float>;
//%rename(appendImage_double)   appendImage<double>;
//%rename(appendImage_cxfloat)  appendImage<cxfloat>;
//%rename(appendImage_cxdouble) appendImage<cxdouble>;

//%rename(appendArray_ushort)   appendArray<unsigned short int>;
//%rename(appendArray_float)    appendArray<float>;
//%rename(appendArray_double)   appendArray<double>;
//%rename(appendArray_cxfloat)  appendArray<cxfloat>;
//%rename(appendArray_cxdouble) appendArray<cxdouble>;

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


// Template instantiations from ismrmrd_hdf5.cpp
/*
template EXPORTISMRMRD int IsmrmrdDataset::appendArray(NDArrayContainer< unsigned short int >& a,const char* varname);
template EXPORTISMRMRD int IsmrmrdDataset::appendArray(NDArrayContainer< float >& a,const char* varname);
template EXPORTISMRMRD int IsmrmrdDataset::appendArray(NDArrayContainer< double >& a,const char* varname);
template EXPORTISMRMRD int IsmrmrdDataset::appendArray(NDArrayContainer< std::complex<float> >& a,const char* varname);
template EXPORTISMRMRD int IsmrmrdDataset::appendArray(NDArrayContainer< std::complex<double> >& a,const char* varname);
template EXPORTISMRMRD int IsmrmrdDataset::appendArray(NDArrayContainer< ImageHeader >& a,const char* varname);

template EXPORTISMRMRD int IsmrmrdDataset::appendImage(Image<float>& m, const char* varname);
template EXPORTISMRMRD int IsmrmrdDataset::appendImage(Image<double>& m, const char* varname);
template EXPORTISMRMRD int IsmrmrdDataset::appendImage(Image<unsigned short>& m, const char* varname);
template EXPORTISMRMRD int IsmrmrdDataset::appendImage(Image< std::complex<float> >& m, const char* varname);
template EXPORTISMRMRD int IsmrmrdDataset::appendImage(Image< std::complex<double> >& m, const char* varname);

template EXPORTISMRMRD boost::shared_ptr< Image<float> > IsmrmrdDataset::readImage(const char* varname, unsigned long index);
template EXPORTISMRMRD boost::shared_ptr< Image<double> > IsmrmrdDataset::readImage(const char* varname, unsigned long index);
template EXPORTISMRMRD boost::shared_ptr< Image<unsigned short> > IsmrmrdDataset::readImage(const char* varname, unsigned long index);
template EXPORTISMRMRD boost::shared_ptr< Image< std::complex<float> > > IsmrmrdDataset::readImage(const char* varname, unsigned long index);
template EXPORTISMRMRD boost::shared_ptr< Image< std::complex<double> > > IsmrmrdDataset::readImage(const char* varname, unsigned long index);

template EXPORTISMRMRD boost::shared_ptr< NDArrayContainer<unsigned short int> > IsmrmrdDataset::readArray(const char* varname, unsigned long int index);
template EXPORTISMRMRD boost::shared_ptr< NDArrayContainer<float> > IsmrmrdDataset::readArray(const char* varname, unsigned long int index);
template EXPORTISMRMRD boost::shared_ptr< NDArrayContainer<double> > IsmrmrdDataset::readArray(const char* varname, unsigned long int index);
template EXPORTISMRMRD boost::shared_ptr< NDArrayContainer< std::complex<float> > > IsmrmrdDataset::readArray(const char* varname, unsigned long int index);
template EXPORTISMRMRD boost::shared_ptr< NDArrayContainer< std::complex<double> > > IsmrmrdDataset::readArray(const char* varname, unsigned long int index);

*/

// valarray typemap
// Convert from Java to C++
//%typemap(in) (std::valarray<float>& data) {
//    JNIEnv *env = JNU_GetEnv();
//    jsize len = env->GetArrayLength($input);
//    float *ptr = env->GetFloatArrayElements(arr, NULL);
//    std::valarray<float> data;
//    data.resize(len);
//    for (jint i = 0; i < len; i++) {
//        data[i] = ptr[i];
//    }
//    env->ReleaseFloatArrayElements(arr, ptr, JNI_COMMIT);
//    $1 = data;
//}

// Convert from C++ to Java
//%typemap(res) (std:valarray<float>) {
//    JNIEnv *env = JNU_GetEnv();
//    jfloatArray *arr = env->NewFloatArray($1.size());
//    if (arr != NULL) {
//        float *ptr = env->GetFloatArrayElements(arr, NULL);
//        for (jint i = 0; j < $1.size(); i++) {
//            ptr[i] = $1[i];
//        }
//        env->ReleaseFloatArrayElements(arr, ptr, JNI_COMMIT);
//   }
//    $result = arr;
//}

// Apply the valarry typemaps
//%apply (std:valarray<float>) { (std:valarray<float>) };
//%apply (std:valarray<float>& ISMRMRD::Acquisition::getData()) { 
//    (std:valarray<float>&) };
//%apply (ISMRMRD::Acquisition::setData(std:valarray<float>& data)) { 
//    (std:valarray<float>&) };


