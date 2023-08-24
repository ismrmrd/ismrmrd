//
// Created by Martyn Klassen on 2020-05-20.
//

#pragma once
#ifndef ISMRMRD_COMPRESSION_TEMPLATES_H
#define ISMRMRD_COMPRESSION_TEMPLATES_H

#include <cassert>
#include <iostream>
#include <zfp.h>
#include <cstring>
#include <limits>

namespace ISMRMRD {

namespace _private {

constexpr size_t zfp_block_size = 4;

size_t integerPower(size_t base, size_t exponent);

// Overloaded functions for selecting encoder
size_t (*get_encoder(uint dims, int32 *))(zfp_stream *, const int32 *);
size_t (*get_encoder(uint dims, int64 *))(zfp_stream *, const int64 *);

size_t encode_partial_int32_1(zfp_stream *zfp, const int32 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw);
size_t encode_partial_int32_2(zfp_stream *zfp, const int32 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw);
size_t encode_partial_int32_3(zfp_stream *zfp, const int32 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw);
size_t (*get_partial_encoder(uint dims, int32 *))(zfp_stream *, const int32 *, size_t, size_t, size_t, size_t, ptrdiff_t, ptrdiff_t, ptrdiff_t, ptrdiff_t);

size_t encode_partial_int64_1(zfp_stream *zfp, const int64 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw);
size_t encode_partial_int64_2(zfp_stream *zfp, const int64 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw);
size_t encode_partial_int64_3(zfp_stream *zfp, const int64 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw);
size_t (*get_partial_encoder(uint dims, int64 *))(zfp_stream *, const int64 *, size_t, size_t, size_t, size_t, ptrdiff_t, ptrdiff_t, ptrdiff_t, ptrdiff_t);

// Overloaded functions for selecting decoder
size_t (*get_decoder(uint dims, int32 *))(zfp_stream *, int32 *);
size_t (*get_decoder(uint dims, int64 *))(zfp_stream *, int64 *);

size_t decode_partial_int32_1(zfp_stream *zfp, int32 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw);
size_t decode_partial_int32_2(zfp_stream *zfp, int32 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw);
size_t decode_partial_int32_3(zfp_stream *zfp, int32 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw);
size_t (*get_partial_decoder(uint dims, int32 *))(zfp_stream *, int32 *, size_t, size_t, size_t, size_t, ptrdiff_t, ptrdiff_t, ptrdiff_t, ptrdiff_t);

size_t decode_partial_int64_1(zfp_stream *zfp, int64 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw);
size_t decode_partial_int64_2(zfp_stream *zfp, int64 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw);
size_t decode_partial_int64_3(zfp_stream *zfp, int64 *block, size_t nx, size_t ny, size_t nz, size_t nw, ptrdiff_t sx, ptrdiff_t sy, ptrdiff_t sz, ptrdiff_t sw);
size_t (*get_partial_decoder(uint dims, int64 *))(zfp_stream *, int64 *, size_t, size_t, size_t, size_t, ptrdiff_t, ptrdiff_t, ptrdiff_t, ptrdiff_t);

// Templated version of promote and demote
template <typename D, typename S>
typename std::enable_if<std::is_signed<D>::value && std::is_integral<D>::value && std::is_integral<S>::value && (sizeof(D) >= sizeof(S)), void>::type
promote(D *oblock, const S *iblock, size_t points, D extra) {
    // The number of powers of 2 between D and S
    // D digits is 31 or 63, D is signed
    auto shift = static_cast<D>(std::numeric_limits<D>::digits - std::numeric_limits<S>::digits) + extra;

    if (std::is_signed<S>::value) {
        // S digits is 7, 15, or 31
        // shift is 24, 16, 0, or 32, but needs to be 23, 15, 0, or 31

        // int32 to int32 is already correct
        if (sizeof(S) != sizeof(D))
            shift--;

        // arithmetic left shift assumed, i.e. sign bit is preserved, not shifted (C++20)
        // shift is multiply by 2^shift so data spans [-2^30, 2^30 - 1] for int32
        while (points--)
            *oblock++ = static_cast<D>(*iblock++) << shift;
    } else {
        // S digits is 8, 16, or 32
        // shift is 23, 15, -1, or 31

        // D is signed so offset is 2^30 or 2^62
        D offset = static_cast<uint64_t>(1) << static_cast<uint64_t>(std::numeric_limits<D>::digits - 1);

        // if uint32 to int32
        if (sizeof(S) == sizeof(D)) {
            // shift needs to be 0 not -1, uint32 must have been verified < 2^31 already
            shift++;
        }

        // arithmetic left shift assumed, i.e. sign bit is preserved, not shifted (C++20)
        // shift is multiply by 2^shift so data spans [0, 2^31 - 1] for int32
        // offset moves range to span [-2^30, 2^30 -1]
        while (points--)
            *oblock++ = (static_cast<D>(*iblock++) << shift) - offset;
    }
}

template <typename D, typename S>
typename std::enable_if<std::is_signed<S>::value && std::is_integral<D>::value && std::is_integral<S>::value && (sizeof(D) <= sizeof(S)), void>::type
demote(D *oblock, const S *iblock, size_t points, S extra) {
    // The number of powers of 2 between S and D
    // S digits is always 31 or 63, S is signed
    auto shift = static_cast<S>(std::numeric_limits<S>::digits - std::numeric_limits<D>::digits) + extra;

    if (std::is_signed<D>::value) {
        // D digits is 7, 15, or 31
        // shift is 24, 16, 0, or 32, but needs to be 23, 15, 0, or 31

        // int32 to int32 is already correct
        if (sizeof(S) != sizeof(D))
            shift--;

        // By design S and D are both signed (in this branch of if statement) and
        // D is equal or smaller than S
        // Therefore calculate the min and max values as S types for comparison
        // Otherwise compiler complains about signed/unsigned comparison
        S maxValue = static_cast<S>(std::numeric_limits<D>::max());
        S minValue = static_cast<S>(std::numeric_limits<D>::min());

        // arithmetic right shift assumed, i.e. sign bit is preserved, not shifted (C++20)
        // shift is divide by 2^shift to range [-2^15, 2^15 - 1] for int16
        // includes extra shifts reduce the range further depending on maximum initial values
        // force the values to be within the range of D
        while (points--) {
            S i = *iblock++ >> shift;
            *oblock++ = i < minValue ? std::numeric_limits<D>::min() : (i > maxValue ? std::numeric_limits<D>::max() : static_cast<D>(i));
        }
    } else {
        // S digits is 8, 16, or 32
        // shift is 23, 15, -1, or 31

        // S is signed and D is unsigned (in this branch of if statement)
        // Calculate the limits in S type for comparison
        S minValue = static_cast<S>(std::numeric_limits<D>::min());
        S maxValue = 0; // Not used unless sizeof(D) < sizeof(S)

        // uint32 to int32
        if (sizeof(S) == sizeof(D)) {
            // Ignoring extra, offset is 2^30 not 2^31 for uint32 because first bit in int32 cannot be so promote used 2^30
            extra++;

            // shift needs to be 0 not -1
            shift++;
        }
        else {
            // D is smaller than S so maximum of D will fit in S
            maxValue = static_cast<S>(std::numeric_limits<D>::max());
        }

        S offset = 1ULL << static_cast<unsigned int>(std::numeric_limits<D>::digits - 1 - extra);

        // arithmetic right shift assumed, i.e. sign bit is preserved, not shifted (C++20)
        while (points--) {
            S i = ((*iblock++ >> shift) + offset);
            if (sizeof(S) == sizeof(D))
                // Because max value in uint32 is larger than int32, no need to check the maximum
                *oblock++ = i < minValue ? std::numeric_limits<D>::min() : static_cast<D>(i);
            else
                // Limit the range to that handled by the type D
                *oblock++ = i < minValue ? std::numeric_limits<D>::min() : (i > maxValue ? std::numeric_limits<D>::max() : static_cast<D>(i));
        }
    }
}

#if 0
template <typename T>
static void
pad_block(T *p, size_t n, size_t s) {

    // If no data provided then populate everything with zeros
    if (n < 1) {
        memset(p, 0, zfp_block_size * sizeof(T) * s);
        return;
    }

    size_t i = n;
    // Padding up to half of block size, just replicates last provided
    for (; i < (zfp_block_size - zfp_block_size / 2); i++)
        memcpy(p + i * s, p + (n - 1) * s, sizeof(T) * s);

    // Padding after half reflects first half
    for (; i < zfp_block_size; i++)
        memcpy(p + i * s, p + (zfp_block_size - i - 1) * s, sizeof(T) * s);
}
#endif

// Templated compression for integers
template <typename S, typename D>
typename std::enable_if<std::is_signed<D>::value && std::is_integral<D>::value && std::is_integral<S>::value && (sizeof(D) >= sizeof(S)), size_t>::type
compress(zfp_stream *zfp, const zfp_field *field, D shift) {
    auto dims = zfp_field_dimensionality(field);

    auto pData = reinterpret_cast<const S *>(field->data);
    size_t blockSize = integerPower(zfp_block_size, dims);
    S *iblock = new S[blockSize];
    D *oblock = new D[blockSize];

    size_t l, k, j, i, ll, kk, jj, ii;
    size_t x, y, z, w;
    size_t nx = field->nx ? field->nx : 1;
    size_t ny = field->ny ? field->ny : 1;
    size_t nz = field->nz ? field->nz : 1;
    size_t nw = field->nw ? field->nw : 1;
    ptrdiff_t sx = field->sx ? field->sx : 1;
    ptrdiff_t sy = field->sy ? field->sy : (ptrdiff_t)nx;
    ptrdiff_t sz = field->sz ? field->sz : (ptrdiff_t)(nx * ny);
    ptrdiff_t sw = field->sw ? field->sw : (ptrdiff_t)(nx * ny * nz);

    size_t (*encode)(zfp_stream *, const D *) = get_encoder(dims, oblock);
    size_t (*partial_encode)(zfp_stream *, const D*, size_t, size_t, size_t, size_t, ptrdiff_t, ptrdiff_t, ptrdiff_t, ptrdiff_t) = get_partial_encoder(dims, oblock);

    size_t created = 0;
    for (w = 0; w < nw; w += zfp_block_size) {
        auto bw = std::min(nw - w, zfp_block_size);

        for (z = 0; z < nz; z += zfp_block_size) {
            auto bz = std::min(nz - z, zfp_block_size);

            for (y = 0; y < ny; y += zfp_block_size) {
                auto by = std::min(ny - y, zfp_block_size);

                for (x = 0; x < nx; x += zfp_block_size) {
                    auto bx = std::min(nx - x, zfp_block_size);

                    // Get the block
                    for (l = 0, ll = w * sw; l < bw; l++, ll += sw) {
                        for (k = 0, kk = z * sz + ll; k < bz; k++, kk += sz) {
                            for (j = 0, jj = y * sy + kk; j < by; j++, jj += sy) {
                                for (i = 0, ii = x * sx + jj; i < bx; i++, ii += sx) {
                                    iblock[i + bx * (j + by * (k + bz * l))] = pData[ii];
                                }
                            }
                        }
                    }

                    auto totalPoints = bx * by * bz * bw;

                    // Promote the block
                    promote(oblock, iblock, totalPoints, shift);

                    // Encode the block
                    if (totalPoints != blockSize)
                        created += partial_encode(zfp, oblock, bx, by, bz, bw, 1, bx, bx*by, bx*by*bz);
                    else
                        created += encode(zfp, oblock);
                }
            }
        }
    }
    created += zfp_stream_flush(zfp);
    delete[] iblock;
    delete[] oblock;
    return created;
}

// Templated decompression for integers
template <typename S, typename D>
typename std::enable_if<std::is_signed<D>::value && std::is_integral<D>::value && std::is_integral<S>::value && (sizeof(D) >= sizeof(S)), size_t>::type
decompress(zfp_stream *zfp, zfp_field *field, D shift) {
    auto dims = zfp_field_dimensionality(field);

    S *pData = reinterpret_cast<S *>(field->data);
    size_t blockSize = integerPower(zfp_block_size, dims);
    D *iblock = new D[blockSize];
    S *oblock = new S[blockSize];

    size_t l, k, j, i, ll, kk, jj, ii;
    size_t x, y, z, w;
    size_t nx = field->nx ? field->nx : 1;
    size_t ny = field->ny ? field->ny : 1;
    size_t nz = field->nz ? field->nz : 1;
    size_t nw = field->nw ? field->nw : 1;
    ptrdiff_t sx = field->sx ? field->sx : 1;
    ptrdiff_t sy = field->sy ? field->sy : (ptrdiff_t)(nx);
    ptrdiff_t sz = field->sz ? field->sz : (ptrdiff_t)(nx * ny);
    ptrdiff_t sw = field->sw ? field->sw : (ptrdiff_t)(nx * ny * nz);

    size_t (*decode)(zfp_stream *, D *) = get_decoder(dims, iblock);
    size_t (*partial_decode)(zfp_stream *, D*, size_t, size_t, size_t, size_t, ptrdiff_t, ptrdiff_t, ptrdiff_t, ptrdiff_t) = get_partial_decoder(dims, iblock);

    size_t consumed = 0;
    for (w = 0; w < nw; w += zfp_block_size) {
        auto bw = std::min(nw - w, zfp_block_size);

        for (z = 0; z < nz; z += zfp_block_size) {
            auto bz = std::min(nz - z, zfp_block_size);

            for (y = 0; y < ny; y += zfp_block_size) {
                auto by = std::min(ny - y, zfp_block_size);

                for (x = 0; x < nx; x += zfp_block_size) {
                    auto bx = std::min(nx - x, zfp_block_size);

                    auto totalPoints = bx * by * bz * bw;

                    // Decode the block
                    if (totalPoints != blockSize)
                        consumed += partial_decode(zfp, iblock, bx, by, bz, bw, 1, bx, bx*by, bx*by*bz);
                    else
                        consumed += decode(zfp, iblock);

                    // Demote the block
                    demote(oblock, iblock, totalPoints, shift);

                    // Copy to the field
                    for (l = 0, ll = w * sw; l < bw; l++, ll += sw) {
                        for (k = 0, kk = z * sz + ll; k < bz; k++, kk += sz) {
                            for (j = 0, jj = y * sy + kk; j < by; j++, jj += sy) {
                                for (i = 0, ii = x * sx + jj; i < bx; i++, ii += sx) {
                                    pData[ii] = oblock[i + bx * (j + by * (k + bz * l))];
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    consumed += zfp_stream_align(zfp);
    delete[] iblock;
    delete[] oblock;
    return consumed;
}

} // namespace __private
} // namespace ISMRMRD

#endif //ISMRMRD_COMPRESSION_TEMPLATES_H
