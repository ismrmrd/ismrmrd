//
// Created by Martyn Klassen on 2020-04-20.
//

#include "ismrmrd/serialize.h"
#include "NHLBICompression.h"
#include "compression_templates.h"
#include "ismrmrd/ismrmrd.h"
#include <algorithm>
#include <zfp/bitstream.h>
#include <zfp.h>

namespace ISMRMRD {
namespace _private {

size_t integerPower(size_t base, size_t exponent) { // NOLINT(misc-no-recursion)
    if (exponent == 0)
        return 1;

    size_t result = integerPower(base, exponent / 2);
    result *= result;

    if (exponent & 1)
        result *= base;

    return result;
}

// Overloaded methods for selecting encoder
size_t (*get_encoder(uint dims, int32 *))(zfp_stream *, const int32 *) {
    auto table = { zfp_encode_block_int32_1,
                   zfp_encode_block_int32_2,
                   zfp_encode_block_int32_3,
                   zfp_encode_block_int32_4 };
    return *(table.begin() + (dims - 1));
}

size_t (*get_encoder(uint dims, int64 *))(zfp_stream *, const int64 *) {
    auto table = { zfp_encode_block_int64_1,
                   zfp_encode_block_int64_2,
                   zfp_encode_block_int64_3,
                   zfp_encode_block_int64_4 };
    return *(table.begin() + (dims - 1));
}

size_t encode_partial_int32_1(zfp_stream *zfp, const int32 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw) {
    return zfp_encode_partial_block_strided_int32_1(zfp, block, nx, sx);
}
size_t encode_partial_int32_2(zfp_stream *zfp, const int32 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw) {
    return zfp_encode_partial_block_strided_int32_2(zfp, block, nx, ny, sx, sy);
}
size_t encode_partial_int32_3(zfp_stream *zfp, const int32 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw) {
    return zfp_encode_partial_block_strided_int32_3(zfp, block, nx, ny, nz, sx, sy, sz);
}

size_t (*get_partial_encoder(uint dims, int32 *))(zfp_stream *, const int32 *, size_t, size_t, size_t, size_t, ptrdiff_t, ptrdiff_t, ptrdiff_t, ptrdiff_t) {
    auto table = { encode_partial_int32_1,
                   encode_partial_int32_2,
                   encode_partial_int32_3,
                   zfp_encode_partial_block_strided_int32_4 };
    return *(table.begin() + (dims - 1));
}

size_t encode_partial_int64_1(zfp_stream *zfp, const int64 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw) {
    return zfp_encode_partial_block_strided_int64_1(zfp, block, nx, sx);
}
size_t encode_partial_int64_2(zfp_stream *zfp, const int64 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw) {
    return zfp_encode_partial_block_strided_int64_2(zfp, block, nx, ny, sx, sy);
}
size_t encode_partial_int64_3(zfp_stream *zfp, const int64 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw) {
    return zfp_encode_partial_block_strided_int64_3(zfp, block, nx, ny, nz, sx, sy, sz);
}

size_t (*get_partial_encoder(uint dims, int64 *))(zfp_stream *, const int64 *, size_t, size_t, size_t, size_t, ptrdiff_t, ptrdiff_t, ptrdiff_t, ptrdiff_t) {
    auto table = { encode_partial_int64_1,
                   encode_partial_int64_2,
                   encode_partial_int64_3,
                   zfp_encode_partial_block_strided_int64_4 };
    return *(table.begin() + (dims - 1));
}

// Overloaded functions for selecting decoder
size_t (*get_decoder(uint dims, int32 *))(zfp_stream *, int32 *) {
    auto table = { zfp_decode_block_int32_1,
                   zfp_decode_block_int32_2,
                   zfp_decode_block_int32_3,
                   zfp_decode_block_int32_4 };
    return *(table.begin() + (dims - 1));
}

size_t (*get_decoder(uint dims, int64 *))(zfp_stream *, int64 *) {
    auto table = { zfp_decode_block_int64_1,
                   zfp_decode_block_int64_2,
                   zfp_decode_block_int64_3,
                   zfp_decode_block_int64_4 };
    return *(table.begin() + (dims - 1));
}

size_t decode_partial_int32_1(zfp_stream *zfp, int32 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw) {
    return zfp_decode_partial_block_strided_int32_1(zfp, block, nx, sx);
}
size_t decode_partial_int32_2(zfp_stream *zfp, int32 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw) {
    return zfp_decode_partial_block_strided_int32_2(zfp, block, nx, ny, sx, sy);
}
size_t decode_partial_int32_3(zfp_stream *zfp, int32 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw) {
    return zfp_decode_partial_block_strided_int32_3(zfp, block, nx, ny, nz, sx, sy, sz);
}

size_t (*get_partial_decoder(uint dims, int32 *))(zfp_stream *, int32 *, size_t, size_t, size_t, size_t, ptrdiff_t, ptrdiff_t, ptrdiff_t, ptrdiff_t) {
    auto table = { decode_partial_int32_1,
                   decode_partial_int32_2,
                   decode_partial_int32_3,
                   zfp_decode_partial_block_strided_int32_4 };
    return *(table.begin() + (dims - 1));
}

size_t decode_partial_int64_1(zfp_stream *zfp, int64 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw) {
    return zfp_decode_partial_block_strided_int64_1(zfp, block, nx, sx);
}
size_t decode_partial_int64_2(zfp_stream *zfp, int64 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw) {
    return zfp_decode_partial_block_strided_int64_2(zfp, block, nx, ny, sx, sy);
}
size_t decode_partial_int64_3(zfp_stream *zfp, int64 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw) {
    return zfp_decode_partial_block_strided_int64_3(zfp, block, nx, ny, nz, sx, sy, sz);
}

size_t (*get_partial_decoder(uint dims, int64 *))(zfp_stream *, int64 *, size_t, size_t, size_t, size_t, ptrdiff_t, ptrdiff_t, ptrdiff_t, ptrdiff_t) {
    auto table = { decode_partial_int64_1,
                   decode_partial_int64_2,
                   decode_partial_int64_3,
                   zfp_decode_partial_block_strided_int64_4 };
    return *(table.begin() + (dims - 1));
}
} // namespace _private

void decompress_acquisition(ISMRMRD::ISMRMRD_Acquisition &acq, std::vector<uint8_t> &buffer) {
    ISMRMRD_AcquisitionHeader &header = acq.head;

    auto field = std::unique_ptr<zfp_field, decltype(&zfp_field_free)>(zfp_field_alloc(), &zfp_field_free);

    auto zfp = std::unique_ptr<zfp_stream, decltype(&zfp_stream_close)>(zfp_stream_open(nullptr), &zfp_stream_close);

    auto stream = std::unique_ptr<bitstream, decltype(&stream_close)>(
        stream_open(buffer.data(), buffer.size()), &stream_close);

    if (!stream) {
        throw std::runtime_error("Unable to open compressed stream");
    }

    zfp_stream_set_bit_stream(zfp.get(), stream.get());
    zfp_stream_rewind(zfp.get());

    if (!zfp_read_header(zfp.get(), field.get(), ZFP_HEADER_FULL)) {
        throw std::runtime_error("Unable to read compressed stream header");
    }

    if (field->type != zfp_type_float) {
        throw std::runtime_error("Wrong data format");
    }

    size_t nx = std::max(field->nx, size_t(1));
    size_t ny = std::max(field->ny, size_t(1));
    size_t nz = std::max(field->nz, size_t(1));
    size_t nw = std::max(field->nw, size_t(1));

    switch (zfp_field_dimensionality(field.get())) {
    case 1:
        zfp_field_set_stride_1d(field.get(), 2);
        break;
    case 2:
        zfp_field_set_stride_2d(field.get(), 2, 2 * static_cast<ptrdiff_t>(nx));
        break;
    case 3:
        zfp_field_set_stride_3d(field.get(), 2, 2 * static_cast<ptrdiff_t>(nx), 2 * static_cast<ptrdiff_t>(nx * ny));
        break;
    case 4:
        zfp_field_set_stride_4d(field.get(), 2, 2 * static_cast<ptrdiff_t>(nx), 2 * static_cast<ptrdiff_t>(nx * ny), 2 * static_cast<ptrdiff_t>(nx * ny * nz));
        break;
    }

    if (nx * ny * nz * nw != (header.number_of_samples * header.active_channels)) {
        std::stringstream errorstream;
        errorstream << "Size of decompressed stream does not match the acquisition header ";
        errorstream << "nx=" << nx << ", ny=" << ny << ", nz=" << nz << ", nw=" << nw;
        errorstream << ", number_of_samples=" << header.number_of_samples;
        errorstream << "active_channels=" << header.active_channels;

        throw std::runtime_error(errorstream.str());
    }

    zfp_field_set_pointer(field.get(), acq.data);

    if (!zfp_decompress(zfp.get(), field.get())) {
        throw std::runtime_error("Unable to decompress real stream");
    }

    // Move to the imaginary data and decompress
    zfp_field_set_pointer(field.get(), reinterpret_cast<uint8_t *>(acq.data) + sizeof(float));
    if (!zfp_decompress(zfp.get(), field.get())) {
        throw std::runtime_error("Unable to decompress imaginary stream");
    }
}

void compress_acquisition(ISMRMRD::ISMRMRD_Acquisition const &acq, std::vector<uint8_t> &buffer, unsigned int compression_precision, float compression_tolerance) {
    zfp_type type = zfp_type_float;
    auto field = std::unique_ptr<zfp_field, decltype(&zfp_field_free)>(zfp_field_alloc(), &zfp_field_free);
    auto zfp = std::unique_ptr<zfp_stream, decltype(&zfp_stream_close)>(zfp_stream_open(nullptr), &zfp_stream_close);

    zfp_field_set_pointer(field.get(), acq.data);
    zfp_field_set_type(field.get(), type);

    // Signal should be smooth in array dimensions.
    // The real and imaginary signal should be independently smooth, but there is no reason to expect smoothness
    // with interleaved real and imaginary values. Compress should be done separately, as recommended by zfp documentation.
    if (acq.head.active_channels > 1) {
        // There really is also no reason to expect channel to channel data to be smooth without phase alignment
        // eg if alternative channels are 180 out of phase you will have a saw tooth pattern within the block
        // TODO: Add pre compression phase alignment. Ideal you would align the phase of the zero frequency component
        //       but that would require a Fourier transform of the signal. An alternative would be the aligning the phase
        //       of the complex mean. You would need to store at least channels number of phase shifts and reapply them on decompress
        // Phase aligned and correlated channel would probably be smoother.
        // TODO: Investigate SVD to improve compressibility. X = U * S * W^H then store W^H (channels x channels) and
        //       compress T = X * W = U * S. Recovery of X would involved decompressing T and computing X = T * W^H
        //       This is significantly more computation, but could improve compress.
        zfp_field_set_size_2d(field.get(), acq.head.number_of_samples, acq.head.active_channels);
        zfp_field_set_stride_2d(field.get(), 2, 2 * acq.head.number_of_samples);
    } else {
        // Single channel so it is a 1D array to compress
        zfp_field_set_size_1d(field.get(), acq.head.number_of_samples);
        zfp_field_set_stride_1d(field.get(), 2);
    }

    if (compression_precision > 0) {
        zfp_stream_set_precision(zfp.get(), compression_precision);
    } else if (compression_tolerance > std::numeric_limits<float>::epsilon()) {
        zfp_stream_set_accuracy(zfp.get(), compression_tolerance);
    } else {
        zfp_stream_set_reversible(zfp.get());
    }

    // Compressing real and imaginary so the stream needs to be twice as large as the estimate for a single field
    buffer.resize(2 * zfp_stream_maximum_size(zfp.get(), field.get()));

    auto stream = std::unique_ptr<bitstream, decltype(&stream_close)>(stream_open(buffer.data(), buffer.size()), &stream_close);

    if (!stream) {
        throw std::runtime_error("Cannot open compressed stream");
    }
    zfp_stream_set_bit_stream(zfp.get(), stream.get());
    zfp_stream_rewind(zfp.get());

    if (!zfp_write_header(zfp.get(), field.get(), ZFP_HEADER_FULL)) {
        throw std::runtime_error("Unable to write compression header to stream");
    }

    size_t zfpsize = zfp_compress(zfp.get(), field.get());
    if (zfpsize == 0) {
        throw std::runtime_error("Compression real failed");
    }

    // Move to the imaginary data
    zfp_field_set_pointer(field.get(), reinterpret_cast<float *>(acq.data) + 1);
    zfpsize = zfp_compress(zfp.get(), field.get());
    if (zfpsize == 0) {
        throw std::runtime_error("Compression imaginary failed");
    }
    zfp_stream_flush(zfp.get());
    buffer.resize(zfp_stream_compressed_size(zfp.get()));
}

void decompress_image(ISMRMRD::ISMRMRD_Image &image, std::vector<uint8_t> &buffer) {
    auto field = std::unique_ptr<zfp_field, decltype(&zfp_field_free)>(zfp_field_alloc(), &zfp_field_free);
    auto zfp = std::unique_ptr<zfp_stream, decltype(&zfp_stream_close)>(zfp_stream_open(nullptr), &zfp_stream_close);

    auto stream = std::unique_ptr<bitstream, decltype(&stream_close)>(stream_open(buffer.data(), buffer.size()), &stream_close);

    if (!stream) {
        throw std::runtime_error("Unable to open compressed stream");
    }

    zfp_stream_set_bit_stream(zfp.get(), stream.get());
    zfp_stream_rewind(zfp.get());

    if (!zfp_read_header(zfp.get(), field.get(), ZFP_HEADER_FULL)) {
        throw std::runtime_error("Unable to read compressed stream header");
    }

    size_t (*decompress)(zfp_stream *, zfp_field *) = zfp_decompress;

    // Real and imaginary of complex images should be compresses separately
    void *imaginary_data = nullptr;

    ptrdiff_t sx = 0;
    switch (image.head.data_type) {
    case ISMRMRD::ISMRMRD_USHORT:
        decompress = ::ISMRMRD::_private::decompress<uint16, int32>;
        if (field->type != zfp_type_int32) {
            throw std::runtime_error("Wrong image format");
        }
        break;
    case ISMRMRD::ISMRMRD_SHORT:
        decompress = ::ISMRMRD::_private::decompress<int16, int32>;
        if (field->type != zfp_type_int32) {
            throw std::runtime_error("Wrong image format");
        }
        break;
    case ISMRMRD::ISMRMRD_UINT:
        if (field->type == zfp_type_int64) {
            decompress = ISMRMRD::_private::decompress<uint32, int64>;
        } else if (field->type != zfp_type_int32) {
            throw std::runtime_error("Wrong image format");
        }
        break;
    case ISMRMRD::ISMRMRD_INT:
        if (field->type == zfp_type_int64) {
            decompress = ::ISMRMRD::_private::decompress<int32, int64>;
        } else if (field->type != zfp_type_int32) {
            throw std::runtime_error("Wrong image format");
        }
        break;
    case ISMRMRD::ISMRMRD_CXFLOAT:
        imaginary_data = reinterpret_cast<void *>(reinterpret_cast<float *>(image.data) + 1);
        sx = 2;
    case ISMRMRD::ISMRMRD_FLOAT:
        if (field->type != zfp_type_float) {
            throw std::runtime_error("Wrong image format");
        }
        break;
    case ISMRMRD::ISMRMRD_CXDOUBLE:
        imaginary_data = reinterpret_cast<void *>(reinterpret_cast<double *>(image.data) + 1);
        sx = 2;
    case ISMRMRD::ISMRMRD_DOUBLE:
        if (field->type != zfp_type_double) {
            throw std::runtime_error("Wrong image format");
        }
        break;
    }

    size_t nx = std::max(field->nx, size_t(1));
    size_t ny = std::max(field->ny, size_t(1));
    size_t nz = std::max(field->nz, size_t(1));
    size_t nw = std::max(field->nw, size_t(1));

    if (sx) {
        switch (zfp_field_dimensionality(field.get())) {
        case 1:
            zfp_field_set_stride_1d(field.get(), sx);
            break;
        case 2:
            zfp_field_set_stride_2d(field.get(), sx, sx * static_cast<ptrdiff_t>(nx));
            break;
        case 3:
            zfp_field_set_stride_3d(field.get(), sx, sx * static_cast<ptrdiff_t>(nx), sx * static_cast<ptrdiff_t>(nx * ny));
            break;
        case 4:
            zfp_field_set_stride_4d(field.get(), sx, sx * static_cast<ptrdiff_t>(nx), sx * static_cast<ptrdiff_t>(nx * ny), sx * static_cast<ptrdiff_t>(nx * ny * nz));
            break;
        }
    }

    constexpr uint16_t min_value = 1;
    if (nx * ny * nz * nw != (std::max(image.head.matrix_size[0], min_value) *
                              std::max(image.head.matrix_size[1], min_value) *
                              std::max(image.head.matrix_size[2], min_value) *
                              std::max(image.head.channels, min_value))) {
        std::stringstream errorstream;
        errorstream << "Size of decompressed stream does not match the image header ";
        errorstream << "nx=" << nx << ", ny=" << ny << ", nz=" << nz << ", nw=" << nw;
        errorstream << ", matrix=[" << image.head.matrix_size[0];
        for (size_t i = 1; i < 3; i++)
            errorstream << ", " << image.head.matrix_size[i];
        errorstream << "], channels=" << image.head.channels;

        throw std::runtime_error(errorstream.str());
    }

    // Decompress the real data
    zfp_field_set_pointer(field.get(), image.data);
    if (!decompress(zfp.get(), field.get())) {
        throw std::runtime_error("Unable to decompress real stream");
    }

    // Move to the imaginary data and decompress if required
    if (nullptr != imaginary_data) {
        zfp_field_set_pointer(field.get(), imaginary_data);
        if (!decompress(zfp.get(), field.get())) {
            throw std::runtime_error("Unable to decompress imaginary stream");
        }
    }
}

void compress_image(ISMRMRD::ISMRMRD_Image const &image, std::vector<uint8_t> &buffer, unsigned int compression_precision, float compression_tolerance) {
    auto field = std::unique_ptr<zfp_field, decltype(&zfp_field_free)>(zfp_field_alloc(), &zfp_field_free);
    auto zfp = std::unique_ptr<zfp_stream, decltype(&zfp_stream_close)>(zfp_stream_open(nullptr), &zfp_stream_close);

    zfp_field_set_pointer(field.get(), image.data);

    if (compression_precision) {
        zfp_stream_set_precision(zfp.get(), compression_precision);
    } else if (compression_tolerance > std::numeric_limits<float>::epsilon()) {
        zfp_stream_set_accuracy(zfp.get(), compression_tolerance);
    } else {
        zfp_stream_set_reversible(zfp.get());
    }

    size_t (*compress)(zfp_stream *, const zfp_field *) = zfp_compress;

    zfp_type type;
    void *imaginary_data = nullptr;
    int stride = 1;
    switch (image.head.data_type) {
    case ISMRMRD::ISMRMRD_USHORT:
        // 16-bit integers are not supported and must be promoted to int32
        type = zfp_type_int32;
        compress = ::ISMRMRD::_private::compress<uint16, int32>;
        break;
    case ISMRMRD::ISMRMRD_SHORT:
        // 16-bit integers are not supported and must be promoted to int32
        type = zfp_type_int32;
        compress = ::ISMRMRD::_private::compress<int16, int32>;
        break;
    case ISMRMRD::ISMRMRD_UINT:
        // If all value < 2^30 then int32 can be used
        if (std::any_of(reinterpret_cast<uint32 *>(image.data),
                        reinterpret_cast<uint32 *>(image.data) + ismrmrd_size_of_image_data(&image) / ismrmrd_sizeof_data_type(image.head.data_type),
                        [](uint32 value) { return value >= 0x40000000; })) {
            // Because all 2^32 bits of uint32 are required, we must promote to int64
            type = zfp_type_int64;
            compress = ::ISMRMRD::_private::compress<uint32, int64>;
        } else {
            type = zfp_type_int32;
        }
        break;
    case ISMRMRD::ISMRMRD_INT:
        // If all |value| < 2^30 then int32 can be used
        if (std::any_of(reinterpret_cast<int32 *>(image.data),
                        reinterpret_cast<int32 *>(image.data) + ismrmrd_size_of_image_data(&image) / ismrmrd_sizeof_data_type(image.head.data_type),
                        [](int32 value) { return std::abs(value) >= 0x40000000; })) {
            // Because all 2^32 bits of int32 are required, we must promote to int64
            type = zfp_type_int64;
            compress = ::ISMRMRD::_private::compress<int32, int64>;
        } else {
            type = zfp_type_int32;
        }
        break;
    case ISMRMRD::ISMRMRD_FLOAT:
        type = zfp_type_float;
        break;
    case ISMRMRD::ISMRMRD_DOUBLE:
        type = zfp_type_double;
        break;
    case ISMRMRD::ISMRMRD_CXFLOAT:
        type = zfp_type_float;
        stride = 2;
        imaginary_data = reinterpret_cast<void *>(reinterpret_cast<float *>(image.data) + 1);
        break;
    case ISMRMRD::ISMRMRD_CXDOUBLE:
        type = zfp_type_double;
        stride = 2;
        imaginary_data = reinterpret_cast<void *>(reinterpret_cast<double *>(image.data) + 1);
        break;
    default:
        throw std::runtime_error("Invalid datatype");
    }

    zfp_field_set_type(field.get(), type);

    constexpr uint16_t min_value = 1;
    int nx = std::max(image.head.matrix_size[0], min_value);
    int ny = std::max(image.head.matrix_size[1], min_value);
    int nz = std::max(image.head.matrix_size[2], min_value);
    int nw = std::max(image.head.channels, min_value);

    // Collapse image dimensions to make blocks of at least four
    // because images are Fourier transforms so have intrinsic repetition.
    // The start of one dimension should be similar to the end
    while ((nx < 4) && (ny * nz * nw) > 1) {
        nx *= ny;
        ny = nz;
        nz = nw;
        nw = 1;
    }

    while ((ny < 4) && (nz * nw) > 1) {
        ny *= nz;
        nz = nw;
        nw = 1;
    }

    while ((nz < 4) && (nw > 1)) {
        nz *= nw;
        nw = 1;
    }

    if ((nw < 4)) {
        nz *= nw;
        nw = 1;
    }

    // Set the field dimensions and stride
    switch ((nx > 1) + (ny > 1) + (nz > 1) + (nw > 1)) {
    case 4:
        zfp_field_set_size_4d(field.get(), nx, ny, nz, nw);
        zfp_field_set_stride_4d(field.get(), stride, stride * nx, stride * nx * ny, stride * nx * ny * nz);
        break;
    case 3:
        zfp_field_set_size_3d(field.get(), nx, ny, nz);
        zfp_field_set_stride_3d(field.get(), stride, stride * nx, stride * nx * ny);
        break;
    case 2:
        zfp_field_set_size_2d(field.get(), nx, ny);
        zfp_field_set_stride_2d(field.get(), stride, stride * nx);
        break;
    case 1:
    default:
        zfp_field_set_size_1d(field.get(), nx);
        zfp_field_set_stride_1d(field.get(), stride);
        break;
    }

    // Complex data will need stride times (twice) the computed maximum
    buffer.resize(stride * zfp_stream_maximum_size(zfp.get(), field.get()));

    auto stream = std::unique_ptr<bitstream, decltype(&stream_close)>(stream_open(buffer.data(), buffer.size()), &stream_close);

    if (!stream) {
        throw std::runtime_error("Cannot open compressed stream");
    }
    zfp_stream_set_bit_stream(zfp.get(), stream.get());

    if (!zfp_write_header(zfp.get(), field.get(), ZFP_HEADER_FULL)) {
        throw std::runtime_error("Unable to write compression header to stream");
    }

    size_t zfpsize = compress(zfp.get(), field.get());

    // zfp documentation claims zfpsize == 0 is an error.
    if (zfpsize == 0) {
        throw std::runtime_error("Compression failed");
    }

    // Compress the imaginary part, if required
    if (nullptr != imaginary_data) {
        zfp_field_set_pointer(field.get(), imaginary_data);
        zfpsize = compress(zfp.get(), field.get());
        if (zfpsize == 0) {
            throw std::runtime_error("Compression failed");
        }
    }
    buffer.resize(zfp_stream_compressed_size(zfp.get()));
}

void compress_acquisition_nhlbi(ISMRMRD::ISMRMRD_Acquisition const &acq, std::vector<uint8_t> &buffer, float tolerance, uint8_t precision) {
    size_t datasize = ismrmrd_size_of_acquisition_data(&acq) * 2;
    auto d_ptr = reinterpret_cast<float *>(acq.data);
    CompressedBuffer<float> comp(d_ptr, d_ptr + datasize, tolerance, precision);
    buffer = comp.serialize();
}

void decompress_acquisition_nhlbi(ISMRMRD::ISMRMRD_Acquisition &acq, std::vector<uint8_t> &buffer) {
    CompressedBuffer<float> comp;
    comp.deserialize(buffer);

    size_t datasize = ismrmrd_size_of_acquisition_data(&acq) * 2;

    if (comp.size() != datasize) { //*2 for complex
        std::stringstream error;
        error << "Mismatch between uncompressed data samples " << comp.size();
        error << " and expected number of samples" << datasize;
    }

    auto d_ptr = reinterpret_cast<float *>(acq.data);
    for (size_t i = 0; i < comp.size(); i++) {
        d_ptr[i] = comp[i];
    }
}

} // namespace ISMRMRD
