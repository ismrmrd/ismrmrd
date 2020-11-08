#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/version.h"
#include <boost/test/unit_test.hpp>

using namespace ISMRMRD;

BOOST_AUTO_TEST_SUITE(NDArrayTest)

BOOST_AUTO_TEST_CASE(test_ndarray_init_cleanup)
{
    ISMRMRD_NDArray carr;

    // Check initialization of ndarray
    BOOST_CHECK_EQUAL(ismrmrd_init_ndarray(NULL), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_init_ndarray(&carr), ISMRMRD_NOERROR);
    BOOST_CHECK_EQUAL(carr.version, ISMRMRD_VERSION_MAJOR);
    BOOST_CHECK_EQUAL(carr.data_type, 0);   // TODO: enumerate ISMRMRD_NO_DATATYPE
    BOOST_CHECK_EQUAL(carr.ndim, 0);
    for (int idx = 0; idx < ISMRMRD_NDARRAY_MAXDIM; idx++) {
        BOOST_CHECK_EQUAL(carr.dims[idx], 0);
    }
    BOOST_CHECK(!carr.data);

    // Check cleanup of ndarray
    BOOST_CHECK_EQUAL(ismrmrd_cleanup_ndarray(NULL), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_cleanup_ndarray(&carr), ISMRMRD_NOERROR);
    BOOST_CHECK(!carr.data);
}

BOOST_AUTO_TEST_CASE(test_ndarray_create_free)
{
    ISMRMRD_NDArray* carrp = NULL;

    // Check creation of new ndarray
    BOOST_CHECK(carrp = ismrmrd_create_ndarray());
    BOOST_CHECK_EQUAL(carrp->version, ISMRMRD_VERSION_MAJOR);
    BOOST_CHECK_EQUAL(carrp->data_type, 0);   // TODO: enumerate ISMRMRD_NO_DATATYPE
    BOOST_CHECK_EQUAL(carrp->ndim, 0);
    for (int idx = 0; idx < ISMRMRD_NDARRAY_MAXDIM; idx++) {
        BOOST_CHECK_EQUAL(carrp->dims[idx], 0);
    }
    BOOST_CHECK(!carrp->data);

    // Check cleanup
    BOOST_CHECK_EQUAL(ismrmrd_free_ndarray(NULL), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_free_ndarray(carrp), ISMRMRD_NOERROR);
}

BOOST_AUTO_TEST_CASE(test_ndarray_copy)
{
    // Weak check of ndarray copying
    ISMRMRD_NDArray csrc, cdst;
    BOOST_CHECK_EQUAL(ismrmrd_init_ndarray(&csrc), ISMRMRD_NOERROR);
    // NOTE: it is necessary to call init_ndarray on the destination ndarray
    // before copying, in case its data is non-NULL!
    BOOST_CHECK_EQUAL(ismrmrd_init_ndarray(&cdst), ISMRMRD_NOERROR);
    BOOST_CHECK_EQUAL(ismrmrd_copy_ndarray(&cdst, NULL), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_copy_ndarray(NULL, &csrc), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_copy_ndarray(NULL, NULL), ISMRMRD_RUNTIMEERROR);
    BOOST_CHECK_EQUAL(ismrmrd_copy_ndarray(&cdst, &csrc), ISMRMRD_NOERROR);
    BOOST_CHECK_EQUAL(cdst.version, ISMRMRD_VERSION_MAJOR);
    BOOST_CHECK_EQUAL(cdst.data_type, 0);   // TODO: enumerate ISMRMRD_NO_DATATYPE
    BOOST_CHECK_EQUAL(cdst.ndim, 0);
    for (int idx = 0; idx < ISMRMRD_NDARRAY_MAXDIM; idx++) {
        BOOST_CHECK_EQUAL(cdst.dims[idx], 0);
    }
    BOOST_CHECK(!cdst.data);
}

BOOST_AUTO_TEST_SUITE_END()
