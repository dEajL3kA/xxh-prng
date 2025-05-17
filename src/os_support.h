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

#ifndef OS_SUPPORT_H
#define OS_SUPPORT_H

#if defined(__cplusplus)
#  include <cstdlib>
#  include <cstdint>
#else
#  include <stdlib.h>
#  include <stdint.h>
#endif

#if defined(_WIN32)
#  define FWRITE _fwrite_nolock
#elif defined(__linux__) || defined(__CYGWIN__) || defined(__FreeBSD__) || defined(__HAIKU__)
#  define FWRITE fwrite_unlocked
#else
#  define FWRITE fwrite
#endif

#ifdef _WIN32
#  define STRICMP _stricmp
#else
#  define STRICMP strcasecmp
#endif

#if defined(_MSC_VER) || defined(__MINGW32__)
#  define INLINE __inline
#elif defined(__GNUC__) || defined(__clang__)
#  define INLINE __inline__
#else
#  define INLINE
#endif

#if defined(__cplusplus)
extern "C" {
#endif

int read_entropy(uint8_t *const buffer, const size_t length);
void zero_memory(uint8_t* const buffer, const size_t length);

#if defined(__cplusplus)
}
#endif

#endif /*OS_SUPPORT_H*/
