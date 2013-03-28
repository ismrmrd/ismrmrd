%module ismrmrd

%{

#include "ismrmrd_hdf5.h"
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

%}

#ifdef SWIGJAVA

//%include "arrays_java.i"
//%apply float[] {float *};

//%include "carrays.i"
//%array_class(float, floatArray);

%newobject ISMRMRD::Acquisition::getData;

%extend ISMRMRD::Acquisition {
/*
    std::vector<float> getData()
    {
        std::vector<float> datavec($self->getData().size());
        memcpy(&(datavec[0]), &($self->getData()[0]), sizeof(float) * $self->getData().size());
        return datavec;
    }
    float* getData() {
        float* data = new float[$self->getData().size()];
        memcpy(&(data[0]), &($self->getData()[0]), sizeof(float) * $self->getData().size());
        return data;
    }

*/

    //JNIEXPORT jfloatArray JNICALL getData() {
    jfloatArray getData() {
        JNIEnv *env = JNU_GetEnv();
        std::valarray<float> data = $self->getData();
        jfloatArray arr = env->NewFloatArray(data.size());
        if (arr != NULL) {
            float *ptr = env->GetFloatArrayElements(arr, NULL);

            jint i;
            for (i = 0; i < data.size(); i++) {
                ptr[i] = data[i];
            }

            env->ReleaseFloatArrayElements(arr, ptr, JNI_COMMIT);
        }
        return arr;
    }

}
#endif


%include "../ismrmrd.i"

