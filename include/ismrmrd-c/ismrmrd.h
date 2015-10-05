#ifndef ISMRMRD_C_H
#define ISMRMRD_C_H

#include "ismrmrd/export.h"

#ifdef _MSC_VER /* MS C compiler */
typedef int bool;
#define false 0
#define true 1
#else /* C99 compiler */
#include <stdbool.h>
#endif /* _MSC_VER */

/** Error Codes */
enum ErrorCodes {
    ISMRMRD_BEGINERROR=-1,
    ISMRMRD_NOERROR,
    ISMRMRD_MEMORYERROR,
    ISMRMRD_FILEERROR,
    ISMRMRD_TYPEERROR,
    ISMRMRD_RUNTIMEERROR,
    ISMRMRD_HDF5ERROR,
    ISMRMRD_ENDERROR
};

struct ISMRMRD_AcquisitionHeader;
typedef struct ISMRMRD_AcquisitionHeader ISMRMRD_AcquisitionHeader;

/**
 * Initialize an Acquisition Header
 * @ingroup capi
 */
EXPORTISMRMRD int ismrmrd_init_acquisition_header(ISMRMRD_AcquisitionHeader *hdr);

/** Individual MR acquisition. */
typedef struct ISMRMRD_Acquisition {
    ISMRMRD_AcquisitionHeader head; /**< Header, see above */
    float *traj;
    void *data;
} ISMRMRD_Acquisition;

/** @addtogroup capi
 *  @{
 */
EXPORTISMRMRD ISMRMRD_Acquisition * ismrmrd_create_acquisition();
EXPORTISMRMRD int ismrmrd_free_acquisition(ISMRMRD_Acquisition *acq);
EXPORTISMRMRD int ismrmrd_init_acquisition(ISMRMRD_Acquisition *acq);
EXPORTISMRMRD int ismrmrd_cleanup_acquisition(ISMRMRD_Acquisition *acq);
EXPORTISMRMRD int ismrmrd_copy_acquisition(ISMRMRD_Acquisition *acqdest, const ISMRMRD_Acquisition *acqsource);
EXPORTISMRMRD int ismrmrd_make_consistent_acquisition(ISMRMRD_Acquisition *acq);
EXPORTISMRMRD size_t ismrmrd_size_of_acquisition_traj(const ISMRMRD_Acquisition *acq);
EXPORTISMRMRD size_t ismrmrd_size_of_acquisition_data(const ISMRMRD_Acquisition *acq);
/** @} */


EXPORTISMRMRD int ismrmrd_init_image_header(ISMRMRD_ImageHeader *hdr);

/** An individual Image
 * @ingroup capi
 */
typedef struct ISMRMRD_Image {
    ISMRMRD_ImageHeader head;
    char *attribute_string;
    void *data;
} ISMRMRD_Image;


/** @addtogroup capi
 *  @{
 */
EXPORTISMRMRD ISMRMRD_Image * ismrmrd_create_image();
EXPORTISMRMRD int ismrmrd_free_image(ISMRMRD_Image *im);
EXPORTISMRMRD int ismrmrd_init_image(ISMRMRD_Image *im);
EXPORTISMRMRD int ismrmrd_cleanup_image(ISMRMRD_Image *im);
EXPORTISMRMRD int ismrmrd_copy_image(ISMRMRD_Image *imdest, const ISMRMRD_Image *imsource);
EXPORTISMRMRD int ismrmrd_make_consistent_image(ISMRMRD_Image *im);
EXPORTISMRMRD size_t ismrmrd_size_of_image_attribute_string(const ISMRMRD_Image *im);
EXPORTISMRMRD size_t ismrmrd_size_of_image_data(const ISMRMRD_Image *im);
/** @} */


/** A simple N dimensional array */
struct ISMRMRD_NDArray {
    uint16_t version;                      /**< First unsigned int indicates the version */
    uint16_t data_type;                    /**< e.g. unsigned short, float, complex float, etc. */
    uint16_t ndim;                         /**< Number of dimensions */
    size_t   dims[ISMRMRD_NDARRAY_MAXDIM]; /**< Dimensions */
    void     *data;                        /**< Pointer to data */
} ISMRMRD_NDArray;

/** @addtogroup capi
 *  @{
 */
EXPORTISMRMRD ISMRMRD_NDArray * ismrmrd_create_ndarray();
EXPORTISMRMRD int ismrmrd_free_ndarray(ISMRMRD_NDArray *arr);
EXPORTISMRMRD int ismrmrd_init_ndarray(ISMRMRD_NDArray *arr);
EXPORTISMRMRD int ismrmrd_cleanup_ndarray(ISMRMRD_NDArray *arr);
EXPORTISMRMRD int ismrmrd_copy_ndarray(ISMRMRD_NDArray *arrdest, const ISMRMRD_NDArray *arrsource);
EXPORTISMRMRD int ismrmrd_make_consistent_ndarray(ISMRMRD_NDArray *arr);
EXPORTISMRMRD size_t ismrmrd_size_of_ndarray_data(const ISMRMRD_NDArray *arr);
/** @} */



/*********/
/* Flags */
/*********/
/** @addtogroup capi
 *  @{
 */
EXPORTISMRMRD bool ismrmrd_is_flag_set(const uint64_t flags, const uint64_t val);
EXPORTISMRMRD int ismrmrd_set_flag(uint64_t *flags, const uint64_t val);
EXPORTISMRMRD int ismrmrd_set_flags(uint64_t *flags, const uint64_t val);
EXPORTISMRMRD int ismrmrd_clear_flag(uint64_t *flags, const uint64_t val);
EXPORTISMRMRD int ismrmrd_clear_all_flags(uint64_t *flags);
/** @} */

/*****************/
/* Channel Masks */
/*****************/
/** @addtogroup capi
 *  @{
 */
EXPORTISMRMRD bool ismrmrd_is_channel_on(const uint64_t channel_mask[ISMRMRD_CHANNEL_MASKS], const uint16_t chan);
EXPORTISMRMRD int ismrmrd_set_channel_on(uint64_t channel_mask[ISMRMRD_CHANNEL_MASKS], const uint16_t chan);
EXPORTISMRMRD int ismrmrd_set_channel_off(uint64_t channel_mask[ISMRMRD_CHANNEL_MASKS], const uint16_t chan);
EXPORTISMRMRD int ismrmrd_set_all_channels_off(uint64_t channel_mask[ISMRMRD_CHANNEL_MASKS]);
/** @} */

/******************/
/* Error Handling */
/******************/
/** @addtogroup capi
 *  @{
 */
typedef void (*ismrmrd_error_handler_t)(const char *file, int line,
        const char *function, int code, const char *msg);
#define ISMRMRD_PUSH_ERR(code, msg) ismrmrd_push_error(__FILE__, __LINE__, \
        __func__, (code), (msg))
int ismrmrd_push_error(const char *file, const int line, const char *func,
        const int code, const char *msg);
/** Sets a custom error handler */
EXPORTISMRMRD void ismrmrd_set_error_handler(ismrmrd_error_handler_t);
/** Returns message for corresponding error code */
EXPORTISMRMRD char *ismrmrd_strerror(int code);
/** @} */

/** Populates parameters (if non-NULL) with error information
 * @returns true if there was error information to return, false otherwise */
bool ismrmrd_pop_error(char **file, int *line, char **func,
        int *code, char **msg);

#endif /* ISMRMRD_C_H */
