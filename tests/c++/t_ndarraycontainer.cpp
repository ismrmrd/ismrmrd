#include <iostream>
#include "ismrmrd_hdf5.h"
#include "ismrmrd.hxx"
#include <time.h>
#include <stdlib.h>
#include <limits>

using namespace ISMRMRD;

template<class T>
struct populate;

template<> struct populate <float> {

    typedef float type;
    
    static void random (NDArrayContainer<type>& ndac) {
        type mi = (type) std::numeric_limits<int>::max();
        assert (ndac.is_consistent());
        size_t i = ndac.elements();
        size_t j = ndac.data_.size();
        for (; i>=1; i--)
            ndac[i] = 1.0 - 2.0 * (type)rand() / mi;
    }

};

template<> struct populate <double> {
    
    typedef double type;
    
    static void random (NDArrayContainer<type>& ndac) {
        type mi = (type) std::numeric_limits<int>::max();
        assert (ndac.is_consistent());
        size_t i = ndac.elements();
        size_t j = ndac.data_.size();
        for (; i>=1; i--)
            ndac[i] = 1.0 - 2.0 * (type)rand() / mi;
    }

};

template<> struct populate <std::complex<float> > {
    
    typedef float               rtype;
    typedef std::complex<rtype> type;
    
    static void random (NDArrayContainer<type>& ndac) {
        rtype mi = std::numeric_limits<int>::max();
        assert (ndac.is_consistent());
        size_t i = ndac.elements();
        size_t j = ndac.data_.size();
        for (; i>=1; i--)
            ndac[i] = type(1.0, 1.0) - type(2.0 * (rtype)rand() / mi, 2.0 * (rtype)rand() / mi);
    }
    
};

template<> struct populate <std::complex<double> > {
    
    typedef double               rtype;
    typedef std::complex<rtype> type;
    
    static void random (NDArrayContainer<type>& ndac) {
        rtype mi = std::numeric_limits<int>::max();
        assert (ndac.is_consistent());
        size_t i = ndac.elements();
        size_t j = ndac.data_.size();
        for (; i>=1; i--)
            ndac[i] = type(1.0, 1.0) - type(2.0 * (rtype)rand() / mi, 2.0 * (rtype)rand() / mi);
    }
    
};

template<> struct populate <int> {
    
    typedef int type;
    
    static void random (NDArrayContainer<type>& ndac) {
        type mi = (type) std::numeric_limits<type>::max();
        assert (ndac.is_consistent());
        size_t i = ndac.elements();
        size_t j = ndac.data_.size();
        for (; i>=1; i--)
            ndac[i] = rand();
    }
    
};

template<> struct populate <short> {
    
    typedef short type;
    
    static void random (NDArrayContainer<type>& ndac) {
        type mi = (type) std::numeric_limits<type>::max();
        assert (ndac.is_consistent());
        size_t i = ndac.elements();
        size_t j = ndac.data_.size();
        for (; i>=1; i--)
            ndac[i] = rand();
    }
    
};

template<> struct populate <long> {
    
    typedef long type;
    
    static void random (NDArrayContainer<type>& ndac) {
        type mi = (type) std::numeric_limits<type>::max();
        assert (ndac.is_consistent());
        size_t i = ndac.elements();
        size_t j = ndac.data_.size();
        for (; i>=1; i--)
            ndac[i] = rand();
    }
    
};

int main (int args, char** argv) {

    srand(time(NULL));

    std::vector <unsigned> dims;
    dims.push_back (10);
    dims.push_back (8);
    dims.push_back (7);
    
    NDArrayContainer<float> ndac_s (dims);
    populate<float>::random (ndac_s);
    
    NDArrayContainer<double> ndac_d (dims);
    populate<double>::random (ndac_d);
    
    NDArrayContainer<std::complex<float> > ndac_c (dims);
    populate<std::complex<float> >::random (ndac_c);
    
    NDArrayContainer<std::complex<double> > ndac_z (dims);
    populate<std::complex<double> >::random (ndac_z);

    NDArrayContainer<int> ndac_i (dims);
    populate<int>::random (ndac_i);

    NDArrayContainer<short> ndac_si (dims);
    populate<short>::random (ndac_si);

    NDArrayContainer<long> ndac_li (dims);
    populate<long>::random (ndac_li);

    return 0;
    
}
