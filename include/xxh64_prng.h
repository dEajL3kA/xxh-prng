/*
 * XXH64-PRNG
 * Blazing fast XXH64-based secure pseudo-random number generator
 *
 * Copyright (c) 2025 "dEajL3kA" <Cumpoing79@web.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * https://www.opensource.org/licenses/bsd-license.php
 */

#ifndef _XXH64PRNG_H
#define _XXH64PRNG_H

#if defined(__cplusplus)
#  include <cstdlib>
#  include <cstdint>
#else
#  include <stdlib.h>
#  include <stdint.h>
#  include <stdbool.h>
#endif

/* internal state size */
#define _XXH64PRNG_STATE_WORDS 8U
#define _XXH64PRNG_STATE_BYTES ((size_t)(sizeof(uint64_t) * _XXH64PRNG_STATE_WORDS))

/* output block size */
#define XXH64PRNG_OUTPUT_WORDS (12 * _XXH64PRNG_STATE_WORDS)
#define XXH64PRNG_OUTPUT_BYTES ((size_t)(sizeof(uint64_t) * XXH64PRNG_OUTPUT_WORDS))

typedef struct xxh64prng {
	uint64_t state[_XXH64PRNG_STATE_WORDS];
} xxh64prng_t;

#if defined(__cplusplus)
extern "C" {
#endif

/* version number */
extern const uint16_t XXH64PRNG_VERSION_MAJOR;
extern const uint16_t XXH64PRNG_VERSION_MINOR;
extern const uint16_t XXH64PRNG_VERSION_PATCH;

/* build info */
extern const char* const XXH64PRNG_DATE;
extern const char* const XXH64PRNG_ARCH;

/* RNG functions */
void xxh64prng_init(xxh64prng_t *const state, const uint64_t seed);
bool xxh64prng_seed(xxh64prng_t *const state);
void xxh64prng_next(xxh64prng_t *const state, uint64_t *const out);

/* utility functions */
void xxh64prng_zero(void *const addr, const size_t len);

#if defined(__cplusplus)
}
#endif

#endif /*_XXH64PRNG_H*/
