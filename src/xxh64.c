/*
 * XXH64 hash function
 *
 * This is a simplified "XXH64 only" version based on xxHash code.
 * Hopefully I didn't break anything ;-)
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * xxHash - Extremely Fast Hash algorithm
 * Header File
 * Copyright (C) 2012-2023 Yann Collet
 *
 * BSD 2-Clause License (https://www.opensource.org/licenses/bsd-license.php)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following disclaimer
 *      in the documentation and/or other materials provided with the
 *      distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * You can contact the author at:
 *   - xxHash homepage: https://www.xxhash.com
 *   - xxHash source repository: https://github.com/Cyan4973/xxHash
 */

#include "xxh64.h"
#include <string.h>

#if defined(__LITTLE_ENDIAN__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#  define XXH_CPU_LITTLE_ENDIAN 1
#elif defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#  define XXH_CPU_LITTLE_ENDIAN 0
#elif defined(_WIN32)
#  define XXH_CPU_LITTLE_ENDIAN 1
#else
#  error Failed to determine endianess of the CPU!
#endif

#if defined(__GNUC__) || defined(__clang__)
#  define XXH_FORCE_INLINE static __inline__ __attribute__((__always_inline__, __unused__))
#elif defined(_MSC_VER)
#  define XXH_FORCE_INLINE static __forceinline
#elif defined (__cplusplus) || (defined (__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))
#  define XXH_FORCE_INLINE static inline
#else
#  define XXH_FORCE_INLINE static
#endif

#define XXH_aligned   1
#define XXH_unaligned 0

#define XXH_PRIME64_1 UINT64_C(0x9E3779B185EBCA87)
#define XXH_PRIME64_2 UINT64_C(0xC2B2AE3D27D4EB4F)
#define XXH_PRIME64_3 UINT64_C(0x165667B19E3779F9)
#define XXH_PRIME64_4 UINT64_C(0x85EBCA77C2B2AE63)
#define XXH_PRIME64_5 UINT64_C(0x27D4EB2F165667C5)

#if defined(_MSC_VER)
#  define XXH_rotl64 _rotl64
#else
#  define XXH_rotl64(x,r) (((x) << (r)) | ((x) >> (64 - (r))))
#endif

#if defined(__GNUC__) || defined(__clang__) || defined(__INTEL_LLVM_COMPILER)
#  define XXH_swap32 __builtin_bswap32
#elif defined(_MSC_VER)
#  define XXH_swap32 _byteswap_ulong
#else
XXH_FORCE_INLINE uint32_t XXH_swap32(const uint32_t x)
{
	return
		((x << 24) & UINT32_C(0xff000000)) |
		((x << 8) & UINT32_C(0x00ff0000)) |
		((x >> 8) & UINT32_C(0x0000ff00)) |
		((x >> 24) & UINT32_C(0x000000ff));
}
#endif

#if defined(__GNUC__) || defined(__clang__) || defined(__INTEL_LLVM_COMPILER)
#  define XXH_swap64 __builtin_bswap64
#elif defined(_MSC_VER)
#  define XXH_swap64 _byteswap_uint64
#else
XXH_FORCE_INLINE uint32_t XXH_swap64(const uint64_t x)
{
	return
		((x << 56) & UINT64_C(0xff00000000000000)) |
		((x << 40) & UINT64_C(0x00ff000000000000)) |
		((x << 24) & UINT64_C(0x0000ff0000000000)) |
		((x << 8) & UINT64_C(0x000000ff00000000)) |
		((x >> 8) & UINT64_C(0x00000000ff000000)) |
		((x >> 24) & UINT64_C(0x0000000000ff0000)) |
		((x >> 40) & UINT64_C(0x000000000000ff00)) |
		((x >> 56) & UINT64_C(0x00000000000000ff));
}
#endif

XXH_FORCE_INLINE uint64_t XXH_read64(const void *const memPtr)
{
	uint64_t val;
	memcpy(&val, memPtr, sizeof(val));
	return val;
}

XXH_FORCE_INLINE uint32_t XXH_read32(const void *const memPtr)
{
	uint32_t val;
	memcpy(&val, memPtr, sizeof(val));
	return val;
}

XXH_FORCE_INLINE uint64_t XXH_readLE64(const void *const ptr)
{
	return XXH_CPU_LITTLE_ENDIAN ? XXH_read64(ptr) : XXH_swap64(XXH_read64(ptr));
}

XXH_FORCE_INLINE uint32_t XXH_readLE32(const void *const ptr)
{
	return XXH_CPU_LITTLE_ENDIAN ? XXH_read32(ptr) : XXH_swap32(XXH_read32(ptr));
}

XXH_FORCE_INLINE uint64_t XXH_readLE64_align(const void *const ptr, const int align)
{
	if (align == XXH_unaligned) {
		return XXH_readLE64(ptr);
	}
	else {
		return XXH_CPU_LITTLE_ENDIAN ? *(const uint64_t *)ptr : XXH_swap64(*(const uint64_t *)ptr);
	}
}

XXH_FORCE_INLINE uint32_t XXH_readLE32_align(const void *const ptr, const int align)
{
	if (align == XXH_unaligned) {
		return XXH_readLE32(ptr);
	}
	else {
		return XXH_CPU_LITTLE_ENDIAN ? *(const uint32_t *)ptr : XXH_swap32(*(const uint32_t *)ptr);
	}
}

XXH_FORCE_INLINE void XXH64_initAccs(uint64_t *const acc, const uint64_t seed)
{
	acc[0] = seed + XXH_PRIME64_1 + XXH_PRIME64_2;
	acc[1] = seed + XXH_PRIME64_2;
	acc[2] = seed + 0;
	acc[3] = seed - XXH_PRIME64_1;
}

XXH_FORCE_INLINE uint64_t XXH64_round(uint64_t acc, const uint64_t input)
{
	acc += input * XXH_PRIME64_2;
	acc = XXH_rotl64(acc, 31);
	acc *= XXH_PRIME64_1;
	return acc;
}

XXH_FORCE_INLINE uint64_t XXH64_mergeRound(uint64_t acc, uint64_t val)
{
	val = XXH64_round(0, val);
	acc ^= val;
	acc = acc * XXH_PRIME64_1 + XXH_PRIME64_4;
	return acc;
}

XXH_FORCE_INLINE const uint8_t *XXH64_consumeLong(uint64_t *const acc, const uint8_t *input, const size_t len, const int align)
{
	const uint8_t *const bEnd = input + len;
	const uint8_t *const limit = bEnd - 31;

	do {
		acc[0] = XXH64_round(acc[0], XXH_readLE64_align(input, align)); input += 8;
		acc[1] = XXH64_round(acc[1], XXH_readLE64_align(input, align)); input += 8;
		acc[2] = XXH64_round(acc[2], XXH_readLE64_align(input, align)); input += 8;
		acc[3] = XXH64_round(acc[3], XXH_readLE64_align(input, align)); input += 8;
	} while (input < limit);

	return input;
}

XXH_FORCE_INLINE uint64_t XXH64_mergeAccs(const uint64_t *const acc)
{
	uint64_t h64 = XXH_rotl64(acc[0], 1) + XXH_rotl64(acc[1], 7) + XXH_rotl64(acc[2], 12) + XXH_rotl64(acc[3], 18);
	h64 = XXH64_mergeRound(h64, acc[0]);
	h64 = XXH64_mergeRound(h64, acc[1]);
	h64 = XXH64_mergeRound(h64, acc[2]);
	h64 = XXH64_mergeRound(h64, acc[3]);
	return h64;
}

XXH_FORCE_INLINE uint64_t XXH64_avalanche(uint64_t hash)
{
	hash ^= hash >> 33;
	hash *= XXH_PRIME64_2;
	hash ^= hash >> 29;
	hash *= XXH_PRIME64_3;
	hash ^= hash >> 32;
	return hash;
}

XXH_FORCE_INLINE uint64_t XXH64_finalize(uint64_t hash, const uint8_t *ptr, size_t len, const int align)
{
	len &= 31;
	while (len >= 8) {
		hash ^= XXH64_round(0, XXH_readLE64_align(ptr, align));
		ptr += 8;
		hash = XXH_rotl64(hash, 27) * XXH_PRIME64_1 + XXH_PRIME64_4;
		len -= 8;
	}
	if (len >= 4) {
		hash ^= (uint64_t)(XXH_readLE32_align(ptr, align)) * XXH_PRIME64_1;
		ptr += 4;
		hash = XXH_rotl64(hash, 23) * XXH_PRIME64_2 + XXH_PRIME64_3;
		len -= 4;
	}
	while (len > 0) {
		hash ^= (*ptr++) * XXH_PRIME64_5;
		hash = XXH_rotl64(hash, 11) * XXH_PRIME64_1;
		--len;
	}
	return  XXH64_avalanche(hash);
}

XXH_FORCE_INLINE uint64_t XXH64_endian_align(const uint8_t *input, const size_t len, const uint64_t seed, const int align)
{
	uint64_t h64, acc[4];

	if (len >= 32) {
		XXH64_initAccs(acc, seed);
		input = XXH64_consumeLong(acc, input, len, align);
		h64 = XXH64_mergeAccs(acc);
	}
	else {
		h64 = seed + XXH_PRIME64_5;
	}

	h64 += (uint64_t)len;
	return XXH64_finalize(h64, input, len, align);
}

uint64_t XXH64(const void *const input, const size_t len, const uint64_t seed)
{
	return XXH64_endian_align((const uint8_t *)input, len, seed, ((((uintptr_t)input) & 7) == 0) ? XXH_aligned : XXH_unaligned);
}
