#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/version.h"
#include <boost/test/unit_test.hpp>

using namespace ISMRMRD;

BOOST_AUTO_TEST_SUITE(NDArrays)

BOOST_AUTO_TEST_CASE(ndarray_create)
{
    NDArray<uint16_t> arr;

    BOOST_CHECK_EQUAL(arr.getVersion(), ISMRMRD_VERSION_MAJOR);
    BOOST_CHECK_EQUAL(arr.getData().size(), 0);
    BOOST_CHECK_EQUAL(arr.getNDim(), 0);
    BOOST_CHECK_EQUAL(arr.getDims().size(), 0);
}

BOOST_AUTO_TEST_CASE(ndarray_copy)
{
    std::vector<size_t> dims;
    dims.push_back(42);
    dims.push_back(42);
    dims.push_back(7);
    NDArray<double> arr1(dims);
    NDArray<double> arr2(arr1);

    BOOST_CHECK_EQUAL(arr1.getData().size(), arr2.getData().size());
    BOOST_CHECK_EQUAL(arr1.getNDim(), arr2.getNDim());
    BOOST_CHECK_EQUAL_COLLECTIONS(arr1.getDims().begin(), arr1.getDims().end(),
            arr2.getDims().begin(), arr2.getDims().end());
}

BOOST_AUTO_TEST_CASE(ndarray_resize)
{
    size_t d1[] = {4, 8, 12};
    std::vector<size_t> dims1(d1, d1 + sizeof(d1) / sizeof(d1[0]));
    NDArray<float> arr(dims1);
    BOOST_CHECK_EQUAL(arr.getNDim(), 3);
    BOOST_CHECK_EQUAL(arr.getData().size(), 4*8*12);

    size_t d2[] = {16, 24, 32, 40};
    std::vector<size_t> dims2(d2, d2 + sizeof(d2) / sizeof(d2[0]));
    arr.resize(dims2);
    BOOST_CHECK_EQUAL(arr.getNDim(), 4);
    BOOST_CHECK_EQUAL(arr.getData().size(), 16*24*32*40);

    std::vector<float> zeros(16*24*32*40, 0);
    BOOST_CHECK_EQUAL_COLLECTIONS(zeros.begin(), zeros.end(),
            arr.getData().begin(), arr.getData().end());

    arr.resize(std::vector<size_t>());
    BOOST_CHECK_EQUAL(arr.getNDim(), 0);
    BOOST_CHECK_EQUAL(arr.getData().size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
