/*
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
#  define FPUTC _fputc_nolock
#elif defined(__linux__) || defined(__FreeBSD__) || defined(__HAIKU__)
#  define FWRITE fwrite_unlocked
#  define FPUTC fputc_unlocked
#else
#  define FWRITE fwrite
#  define FPUTC fputc
#endif

#ifdef _WIN32
#  define STRICMP _stricmp
#else
#  define STRICMP strcasecmp
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
