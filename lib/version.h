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

#ifndef _XXH64PRNG_VERSION_H
#define _XXH64PRNG_VERSION_H

/* Version */
#define _XXH64PRNG_VERSION_MAJOR 1
#define _XXH64PRNG_VERSION_MINOR 3
#define _XXH64PRNG_VERSION_PATCH 0

/* CPU architecture */
#if defined(_M_X64) || defined(__x86_64__) || defined(__amd64__)
#  define _XXH64PRNG_ARCH "x64"
#elif defined(_M_ARM64) || defined(__aarch64__)
#  define _XXH64PRNG_ARCH "ARM64"
#elif defined(_M_IX86) || defined(__IA32__) || defined(__i386__) || defined(__i386)
#  define _XXH64PRNG_ARCH "x86"
#else
#  error Unknown CPU architecture!
#endif

#endif /*_XXH64PRNG_VERSION_H*/
