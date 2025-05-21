//
// Created by Martyn Klassen on 2020-04-20.
//

#include "ismrmrd/serialize.h"
#include "NHLBICompression.h"
#include "compression_templates.h"
#include "ismrmrd/ismrmrd.h"
#include <algorithm>
#include <numeric>
#include <zfp.h>

namespace ISMRMRD {
namespace _private {


#ifdef _WIN32
#pragma message("Windows does not support compile time endian checks. Assuming little endian.")
#include <zfp/bitstream.inl>
#else
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define BIT_STREAM_WORD_TYPE uint8
#include <zfp/bitstream.inl>
#else
#include <zfp/bitstream.h>
#endif // !_WIN32
#endif


constexpr bitstream_count shiftSize = 8;

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

size_t encode_partial_int32_1(zfp_stream *zfp, const int32 *block, size_t nx, size_t, size_t, size_t, ptrdiff_t sx, ptrdiff_t, ptrdiff_t, ptrdiff_t) {
    return zfp_encode_partial_block_strided_int32_1(zfp, block, nx, sx);
}
size_t encode_partial_int32_2(zfp_stream *zfp, const int32 *block, size_t nx, size_t ny, size_t, size_t, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t, ptrdiff_t) {
    return zfp_encode_partial_block_strided_int32_2(zfp, block, nx, ny, sx, sy);
}
size_t encode_partial_int32_3(zfp_stream *zfp, const int32 *block, size_t nx, size_t ny, size_t nz, size_t, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t) {
    return zfp_encode_partial_block_strided_int32_3(zfp, block, nx, ny, nz, sx, sy, sz);
}

size_t (*get_partial_encoder(uint dims, int32 *))(zfp_stream *, const int32 *, size_t, size_t, size_t, size_t, ptrdiff_t, ptrdiff_t, ptrdiff_t, ptrdiff_t) {
    auto table = { encode_partial_int32_1,
                   encode_partial_int32_2,
                   encode_partial_int32_3,
                   zfp_encode_partial_block_strided_int32_4 };
    return *(table.begin() + (dims - 1));
}

size_t encode_partial_int64_1(zfp_stream *zfp, const int64 *block, size_t nx, size_t, size_t, size_t, ptrdiff_t sx, ptrdiff_t, ptrdiff_t, ptrdiff_t) {
    return zfp_encode_partial_block_strided_int64_1(zfp, block, nx, sx);
}
size_t encode_partial_int64_2(zfp_stream *zfp, const int64 *block, size_t nx, size_t ny, size_t, size_t, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t, ptrdiff_t) {
    return zfp_encode_partial_block_strided_int64_2(zfp, block, nx, ny, sx, sy);
}
size_t encode_partial_int64_3(zfp_stream *zfp, const int64 *block, size_t nx, size_t ny, size_t nz, size_t, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t) {
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

size_t decode_partial_int32_1(zfp_stream *zfp, int32 *block, size_t nx, size_t, size_t, size_t, ptrdiff_t sx, ptrdiff_t, ptrdiff_t, ptrdiff_t) {
    return zfp_decode_partial_block_strided_int32_1(zfp, block, nx, sx);
}
size_t decode_partial_int32_2(zfp_stream *zfp, int32 *block, size_t nx, size_t ny, size_t, size_t, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t, ptrdiff_t) {
    return zfp_decode_partial_block_strided_int32_2(zfp, block, nx, ny, sx, sy);
}
size_t decode_partial_int32_3(zfp_stream *zfp, int32 *block, size_t nx, size_t ny, size_t nz, size_t, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t) {
    return zfp_decode_partial_block_strided_int32_3(zfp, block, nx, ny, nz, sx, sy, sz);
}

size_t (*get_partial_decoder(uint dims, int32 *))(zfp_stream *, int32 *, size_t, size_t, size_t, size_t, ptrdiff_t, ptrdiff_t, ptrdiff_t, ptrdiff_t) {
    auto table = { decode_partial_int32_1,
                   decode_partial_int32_2,
                   decode_partial_int32_3,
                   zfp_decode_partial_block_strided_int32_4 };
    return *(table.begin() + (dims - 1));
}

size_t decode_partial_int64_1(zfp_stream *zfp, int64 *block, size_t nx, size_t, size_t, size_t, ptrdiff_t sx, ptrdiff_t, ptrdiff_t, ptrdiff_t) {
    return zfp_decode_partial_block_strided_int64_1(zfp, block, nx, sx);
}
size_t decode_partial_int64_2(zfp_stream *zfp, int64 *block, size_t nx, size_t ny, size_t, size_t, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t, ptrdiff_t) {
    return zfp_decode_partial_block_strided_int64_2(zfp, block, nx, ny, sx, sy);
}
size_t decode_partial_int64_3(zfp_stream *zfp, int64 *block, size_t nx, size_t ny, size_t nz, size_t, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t) {
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

size_t ismrmrd_num_of_image_data(const ISMRMRD::ISMRMRD_ImageHeader& hdr){
    constexpr uint16_t min_value = 1;
    return std::max(hdr.matrix_size[0], min_value) * std::max(hdr.matrix_size[1], min_value) * std::max(hdr.matrix_size[2], min_value) * std::max(hdr.channels, min_value);
}

size_t ismrmrd_size_of_image_data(const ISMRMRD::ISMRMRD_ImageHeader& hdr){
    if (ismrmrd_sizeof_data_type(hdr.data_type) == 0) {
        ISMRMRD_PUSH_ERR(ISMRMRD_TYPEERROR, "Invalid image data type");
        return 0;
    }
    return ismrmrd_num_of_image_data(hdr) * ismrmrd_sizeof_data_type(hdr.data_type);
}

size_t ismrmrd_num_of_image_attribute_chars(const ISMRMRD::ISMRMRD_ImageHeader& hdr){
    return hdr.attribute_string_len;
}

size_t ismrmrd_size_of_image_attribute_string(const ISMRMRD::ISMRMRD_ImageHeader& hdr){
    return ismrmrd_num_of_image_attribute_chars(hdr); // attribute's char* has sizeof 1
}

size_t ismrmrd_num_of_acquisition_data(const ISMRMRD::ISMRMRD_AcquisitionHeader& hdr){
    return hdr.number_of_samples * hdr.active_channels;
}

size_t ismrmrd_size_of_acquisition_data(const ISMRMRD::ISMRMRD_AcquisitionHeader& hdr){
    return ismrmrd_num_of_acquisition_data(hdr) * sizeof(complex_float_t);
}

size_t ismrmrd_num_of_acquisition_traj(const ISMRMRD::ISMRMRD_AcquisitionHeader& hdr){
    return hdr.number_of_samples * hdr.trajectory_dimensions;
}

size_t ismrmrd_size_of_acquisition_traj(const ISMRMRD::ISMRMRD_AcquisitionHeader& hdr){
    return ismrmrd_num_of_acquisition_traj(hdr) * sizeof(float);
}

void decompress_acquisition(ISMRMRD::ISMRMRD_AcquisitionHeader &hdr, void* data, std::vector<uint8_t> &buffer){
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

    if (nx * ny * nz * nw != ismrmrd_num_of_acquisition_data(hdr)) {
        std::stringstream errorstream;
        errorstream << "Size of decompressed stream does not match the acquisition header ";
        errorstream << "nx=" << nx << ", ny=" << ny << ", nz=" << nz << ", nw=" << nw;
        errorstream << ", number_of_samples=" << hdr.number_of_samples;
        errorstream << "active_channels=" << hdr.active_channels;

        throw std::runtime_error(errorstream.str());
    }

    zfp_field_set_pointer(field.get(), data);

    if (!zfp_decompress(zfp.get(), field.get())) {
        throw std::runtime_error("Unable to decompress real stream");
    }

    // Move to the imaginary data and decompress
    zfp_field_set_pointer(field.get(), reinterpret_cast<uint8_t *>(data) + sizeof(float));
    if (!zfp_decompress(zfp.get(), field.get())) {
        throw std::runtime_error("Unable to decompress imaginary stream");
    }
}

void decompress_acquisition(ISMRMRD::ISMRMRD_Acquisition &acq, std::vector<uint8_t> &buffer) {
    decompress_acquisition(acq.head, acq.data, buffer);
}

void compress_acquisition(ISMRMRD::ISMRMRD_AcquisitionHeader const &hdr, void* data, size_t data_sz, std::vector<uint8_t> &buffer, unsigned int compression_precision, float compression_tolerance){
    zfp_type type = zfp_type_float;
    auto field = std::unique_ptr<zfp_field, decltype(&zfp_field_free)>(zfp_field_alloc(), &zfp_field_free);
    auto zfp = std::unique_ptr<zfp_stream, decltype(&zfp_stream_close)>(zfp_stream_open(nullptr), &zfp_stream_close);

    zfp_field_set_pointer(field.get(), data);
    zfp_field_set_type(field.get(), type);

    // Signal should be smooth in array dimensions.
    // The real and imaginary signal should be independently smooth, but there is no reason to expect smoothness
    // with interleaved real and imaginary values. Compress should be done separately, as recommended by zfp documentation.
    if (hdr.active_channels > 1) {
        // There really is also no reason to expect channel to channel data to be smooth without phase alignment
        // eg if alternative channels are 180 out of phase you will have a saw tooth pattern within the block
        // TODO: Add pre compression phase alignment. Ideal you would align the phase of the zero frequency component
        //       but that would require a Fourier transform of the signal. An alternative would be the aligning the phase
        //       of the complex mean. You would need to store at least channels number of phase shifts and reapply them on decompress
        // Phase aligned and correlated channel would probably be smoother.
        // TODO: Investigate SVD to improve compressibility. X = U * S * W^H then store W^H (channels x channels) and
        //       compress T = X * W = U * S. Recovery of X would involved decompressing T and computing X = T * W^H
        //       This is significantly more computation, but could improve compression. U may be more compressible,
        //       as the large variation are in S, but then you are storing an additional channels singular values as well
        zfp_field_set_size_2d(field.get(), hdr.number_of_samples, hdr.active_channels);
        zfp_field_set_stride_2d(field.get(), 2, 2 * hdr.number_of_samples);
    } else {
        // Single channel so it is a 1D array to compress
        zfp_field_set_size_1d(field.get(), hdr.number_of_samples);
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
    zfp_field_set_pointer(field.get(), reinterpret_cast<float *>(data) + 1);
    zfpsize = zfp_compress(zfp.get(), field.get());
    if (zfpsize == 0) {
        throw std::runtime_error("Compression imaginary failed");
    }
    zfp_stream_flush(zfp.get());
    buffer.resize(zfp_stream_compressed_size(zfp.get()));
}

void compress_acquisition(ISMRMRD::ISMRMRD_Acquisition const &acq, std::vector<uint8_t> &buffer, unsigned int compression_precision, float compression_tolerance) {
    compress_acquisition(acq.head, acq.data, ISMRMRD::ismrmrd_size_of_acquisition_data(&acq), buffer, compression_precision, compression_tolerance);
}

void decompress_image(ISMRMRD::ISMRMRD_ImageHeader &hdr, void* data, std::vector<uint8_t> &buffer) {
    auto field = std::unique_ptr<zfp_field, decltype(&zfp_field_free)>(zfp_field_alloc(), &zfp_field_free);
    auto zfp = std::unique_ptr<zfp_stream, decltype(&zfp_stream_close)>(zfp_stream_open(nullptr), &zfp_stream_close);

    auto stream = std::unique_ptr<bitstream, decltype(&stream_close)>(stream_open(buffer.data(), buffer.size()), &stream_close);

    if (!stream) {
        throw std::runtime_error("Unable to open compressed stream");
    }

    zfp_stream_set_bit_stream(zfp.get(), stream.get());
    zfp_stream_rewind(zfp.get());

    auto shift = static_cast<int32>(stream_read_bits(zfp->stream, _private::shiftSize));

    if (!zfp_read_header(zfp.get(), field.get(), ZFP_HEADER_FULL)) {
        throw std::runtime_error("Unable to read compressed stream header");
    }

    size_t nx = std::max(field->nx, size_t(1));
    size_t ny = std::max(field->ny, size_t(1));
    size_t nz = std::max(field->nz, size_t(1));
    size_t nw = std::max(field->nw, size_t(1));

    if (nx * ny * nz * nw != ismrmrd_num_of_image_data(hdr)) {
        std::stringstream errorstream;
        errorstream << "Size of decompressed stream does not match the image header ";
        errorstream << "nx=" << nx << ", ny=" << ny << ", nz=" << nz << ", nw=" << nw;
        errorstream << ", matrix=[" << hdr.matrix_size[0];
        for (size_t i = 1; i < 3; i++)
            errorstream << ", " << hdr.matrix_size[i];
        errorstream << "], channels=" << hdr.channels;

        throw std::runtime_error(errorstream.str());
    }

    std::function<size_t (zfp_stream *, zfp_field *)> decompress = zfp_decompress;

    // Real and imaginary of complex images should be compresses separately
    void *imaginary_data = nullptr;

    ptrdiff_t sx = 0;
    switch (hdr.data_type) {
    case ISMRMRD::ISMRMRD_USHORT:
        if (field->type != zfp_type_int32) {
            throw std::runtime_error("Wrong image format");
        }
        decompress = [shift](zfp_stream *stream, zfp_field *field) { return ::ISMRMRD::_private::decompress<uint16, int32>(stream, field, shift); };
        break;
    case ISMRMRD::ISMRMRD_SHORT:
        if (field->type != zfp_type_int32) {
            throw std::runtime_error("Wrong image format");
        }
        decompress = [shift](zfp_stream *stream, zfp_field *field) { return ::ISMRMRD::_private::decompress<int16, int32>(stream, field, shift); };
        break;
    case ISMRMRD::ISMRMRD_UINT:
        if (field->type == zfp_type_int64) {
            decompress = [shift](zfp_stream *stream, zfp_field *field) { return ::ISMRMRD::_private::decompress<uint32, int64>(stream, field, shift); };
        } else if (field->type == zfp_type_int32) {
            decompress = [shift](zfp_stream *stream, zfp_field *field) { return ::ISMRMRD::_private::decompress<uint32, int32>(stream, field, shift); };
        }
        else {
            throw std::runtime_error("Wrong image format");
        }
        break;
    case ISMRMRD::ISMRMRD_INT:
        if (field->type == zfp_type_int64) {
            decompress = [shift](zfp_stream *stream, zfp_field *field) { return ::ISMRMRD::_private::decompress<int32, int64>(stream, field, shift); };
        } else if (field->type == zfp_type_int32) {
            decompress = [shift](zfp_stream *stream, zfp_field *field) { return ::ISMRMRD::_private::decompress<int32, int32>(stream, field, shift); };
        }
        else {
            throw std::runtime_error("Wrong image format");
        }
        break;
    case ISMRMRD::ISMRMRD_CXFLOAT:
        imaginary_data = reinterpret_cast<void *>(reinterpret_cast<float *>(data) + 1);
        sx = 2;
    case ISMRMRD::ISMRMRD_FLOAT:
        if (field->type != zfp_type_float) {
            throw std::runtime_error("Wrong image format");
        }
        break;
    case ISMRMRD::ISMRMRD_CXDOUBLE:
        imaginary_data = reinterpret_cast<void *>(reinterpret_cast<double *>(data) + 1);
        sx = 2;
    case ISMRMRD::ISMRMRD_DOUBLE:
        if (field->type != zfp_type_double) {
            throw std::runtime_error("Wrong image format");
        }
        break;
    }

    field->sx = sx;
    field->sy = field->sx * static_cast<ptrdiff_t>(field->nx);
    field->sz = field->sy * static_cast<ptrdiff_t>(field->ny);
    field->sw = field->sz * static_cast<ptrdiff_t>(field->nz);

    // Decompress the real data
    zfp_field_set_pointer(field.get(), data);
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

void decompress_image(ISMRMRD::ISMRMRD_Image &image, std::vector<uint8_t> &buffer) {
    decompress_image(image.head, image.data, buffer);
}

void compress_image(ISMRMRD::ISMRMRD_ImageHeader const &hdr, void* data, size_t data_sz, std::vector<uint8_t> &buffer, unsigned int compression_precision, float compression_tolerance) {
    auto field = std::unique_ptr<zfp_field, decltype(&zfp_field_free)>(zfp_field_alloc(), &zfp_field_free);
    auto zfp = std::unique_ptr<zfp_stream, decltype(&zfp_stream_close)>(zfp_stream_open(nullptr), &zfp_stream_close);

    zfp_field_set_pointer(field.get(), data);

    if (compression_precision) {
        zfp_stream_set_precision(zfp.get(), compression_precision);
    } else if (compression_tolerance > std::numeric_limits<float>::epsilon()) {
        zfp_stream_set_accuracy(zfp.get(), compression_tolerance);
    } else {
        zfp_stream_set_reversible(zfp.get());
    }

    // For increased compressibility, integer values should be spread out maximally, i.e. multiplied by largest possible factor without overflow
    // scaling = MAX_INT / max(abs(values))
    zfp_type type;
    int32 shift = 0;

    using namespace std::placeholders;
    std::function<size_t (zfp_stream *, const zfp_field *)> compress = zfp_compress;

    void *imaginary_data = nullptr;
    int stride = 1;
    switch (hdr.data_type) {
    case ISMRMRD::ISMRMRD_USHORT: {
        // 16-bit integers are not supported and must be promoted to int32
        type = zfp_type_int32;

        // bit-wise or because we only need the MSB, and it is faster than max_element
        auto maximum = std::accumulate(reinterpret_cast<uint16 *>(data),
                                       reinterpret_cast<uint16 *>(data) + data_sz / ismrmrd_sizeof_data_type(hdr.data_type),
                                       uint16(0),
                                       [](uint16 const &a, uint16 const &b) { return a | b; });

        if (maximum) {
            // Shift until MSB is used for maximum value
            while (maximum < 0x8000) {
                maximum <<= 1;
                shift++;
            }
        }

        compress = [shift](zfp_stream *stream, const zfp_field *field) { return ::ISMRMRD::_private::compress<uint16, int32>(stream, field, shift); };
    } break;
    case ISMRMRD::ISMRMRD_SHORT:
    {
        // 16-bit integers are not supported and must be promoted to int32
        type = zfp_type_int32;

        // bit-wise or because we only need the MSB, and it is faster than max_element
        auto maximum = std::accumulate(reinterpret_cast<int16 *>(data),
                                       reinterpret_cast<int16 *>(data) + data_sz / ismrmrd_sizeof_data_type(hdr.data_type),
                                       uint16(0),
                                       [](uint16 const &a, int16 const &b) { return a | static_cast<uint16>(std::abs(b)); });

        if (maximum) {
            // Shift until MSB is used for maximum value
            while (maximum < 0x400) {
                maximum <<= 1;
                shift++;
            }
        }

        compress = [shift](zfp_stream *stream, const zfp_field *field) { return ::ISMRMRD::_private::compress<int16, int32>(stream, field, shift); };
    }
        break;
    case ISMRMRD::ISMRMRD_UINT:
    {
        // bit-wise or because we only need the MSB, and it is faster than max_element
        auto maximum = std::accumulate(reinterpret_cast<uint32 *>(data),
                                       reinterpret_cast<uint32 *>(data) + data_sz / ismrmrd_sizeof_data_type(hdr.data_type),
                                       uint32(0),
                                       [](uint32 const &a, uint32 const &b) { return a | b; });

        // If any values >= 2^31 then int64 must be used
        if (maximum >= 0x80000000) {
            // Because all 32 bits of uint32 are required, we must promote to int64
            type = zfp_type_int64;
            shift = 0;
            compress = [shift](zfp_stream *stream, const zfp_field *field) { return ::ISMRMRD::_private::compress<uint32, int64>(stream, field, shift); };
        } else {
            type = zfp_type_int32;
            if (maximum) {
                while (maximum < 0x40000000) {
                    maximum <<= 1;
                    shift++;
                }
            }
            compress = [shift](zfp_stream *stream, const zfp_field *field) { return ::ISMRMRD::_private::compress<uint32, int32>(stream, field, shift); };
        }
    }
        break;
    case ISMRMRD::ISMRMRD_INT:
    {
        // Need the actual max_element, not just the MSB
        auto maximum = *std::max_element(reinterpret_cast<int32 *>(data),
                                         reinterpret_cast<int32 *>(data) + data_sz / ismrmrd_sizeof_data_type(hdr.data_type),
                                         [](const int32 &value1, const int32 &value2){ return std::abs(value1) < std::abs(value2); });

        // If any |value| >= 2^30 then int64 must be used according to documentation
        // However uint8 [0, 255] promotes to [-2^30, 2^23*(255-128)] which it NOT |value| < 2^30 as per the documentation
        // The real limit is therefore probably [-2^30, (2^30)-1], i.e. you cannot use the highest data bit
        if (maximum >= 0x40000000 || maximum < -0x40000000) {
            type = zfp_type_int64;
            shift = 0;
            compress = [shift](zfp_stream *stream, const zfp_field *field) { return ::ISMRMRD::_private::compress<int32, int64>(stream, field, shift); };
        } else {
            type = zfp_type_int32;
            maximum = std::abs(maximum);
            if (maximum) {
                while (maximum < 0x20000000) {
                    maximum <<= 1;
                    shift++;
                }
            }
            compress = [shift](zfp_stream *stream, const zfp_field *field) { return ::ISMRMRD::_private::compress<int32, int32>(stream, field, shift); };
        }
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
        imaginary_data = reinterpret_cast<void *>(reinterpret_cast<float *>(data) + 1);
        break;
    case ISMRMRD::ISMRMRD_CXDOUBLE:
        type = zfp_type_double;
        stride = 2;
        imaginary_data = reinterpret_cast<void *>(reinterpret_cast<double *>(data) + 1);
        break;
    default:
        throw std::runtime_error("Invalid datatype");
    }

    zfp_field_set_type(field.get(), type);

    constexpr uint16_t min_value = 1;
    size_t nx = std::max(hdr.matrix_size[0], min_value);
    size_t ny = std::max(hdr.matrix_size[1], min_value);
    size_t nz = std::max(hdr.matrix_size[2], min_value);
    size_t nw = std::max(hdr.channels, min_value);

    // Collapse image dimensions to make blocks of at least zfp_block_size
    // because images are Fourier transforms so have intrinsic repetition.
    // The start of one dimension should be similar to the end.
    // This ensures that not all blocks are partial blocks and that no
    // dimensions are 1 which results in using wrong dimensional codec.
    while ((nx < _private::zfp_block_size) && (ny + nz + nw) > 0) {
        nx *= ny;
        ny = nz;
        nz = nw;
        nw = 0;
    }

    while ((ny < _private::zfp_block_size) && (nz + nw) > 0) {
        ny *= nz;
        nz = nw;
        nw = 0;
    }

    while ((nz < _private::zfp_block_size) && (nw > 0)) {
        nz *= nw;
        nw = 0;
    }

    if ((nw < _private::zfp_block_size) && (nw > 0)) {
        nz *= nw;
        nw = 0;
    }
    nz = nz == 1 ? 0 : nz;
    ny = ny == 1 ? 0 : ny;

    // Set the field dimensions and stride
    field->nx = nx;
    field->ny = ny;
    field->nz = nz;
    field->nw = nw;
    field->sx = stride;
    field->sy = field->sx * static_cast<ptrdiff_t>(nx);
    field->sz = field->sy * static_cast<ptrdiff_t>(ny);
    field->sw = field->sz * static_cast<ptrdiff_t>(nz);

    // Complex data will need stride times (twice) the computed maximum
    buffer.resize(stride * zfp_stream_maximum_size(zfp.get(), field.get()));

    auto stream = std::unique_ptr<bitstream, decltype(&stream_close)>(stream_open(buffer.data(), buffer.size()), &stream_close);

    if (!stream) {
        throw std::runtime_error("Cannot open compressed stream");
    }
    zfp_stream_set_bit_stream(zfp.get(), stream.get());

    stream_write_bits(zfp->stream, shift, _private::shiftSize);

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

void compress_image(ISMRMRD::ISMRMRD_Image &image, std::vector<uint8_t> &buffer, unsigned int compression_precision, float compression_tolerance) {
    compress_image(image.head,image.data,ISMRMRD::ismrmrd_size_of_image_data(&image),buffer,compression_precision,compression_tolerance);
}

void compress_acquisition_nhlbi(void* data, size_t data_sz, std::vector<uint8_t> &buffer, float tolerance, uint8_t precision){
    size_t datasize = data_sz * 2;
    auto d_ptr = reinterpret_cast<float *>(data);
    CompressedBuffer<float> comp(d_ptr, d_ptr + datasize, tolerance, precision);
    buffer = comp.serialize();
}

void compress_acquisition_nhlbi(ISMRMRD::ISMRMRD_Acquisition const &acq, std::vector<uint8_t> &buffer, float tolerance, uint8_t precision) {
    compress_acquisition_nhlbi(acq.data, ismrmrd_size_of_acquisition_data(&acq), buffer, tolerance, precision);
}

void decompress_acquisition_nhlbi(void* data, size_t data_sz, std::vector<uint8_t> &buffer){
    CompressedBuffer<float> comp;
    comp.deserialize(buffer);

    size_t datasize = data_sz * 2;

    if (comp.size() != datasize) { //*2 for complex
        std::stringstream error;
        error << "Mismatch between uncompressed data samples " << comp.size();
        error << " and expected number of samples" << datasize;
    }

    auto d_ptr = reinterpret_cast<float *>(data);
    for (size_t i = 0; i < comp.size(); i++) {
        d_ptr[i] = comp[i];
    }
}

void decompress_acquisition_nhlbi(ISMRMRD::ISMRMRD_Acquisition &acq, std::vector<uint8_t> &buffer) {
    decompress_acquisition_nhlbi(acq.data, ismrmrd_size_of_acquisition_data(&acq), buffer);
}

} // namespace ISMRMRD
