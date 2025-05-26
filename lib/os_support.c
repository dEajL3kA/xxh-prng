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

#include <xxh64_prng.h>

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN 1
#  include <Windows.h>
#  include <bcrypt.h>
#else
#  include <unistd.h>
#  include <string.h>
#  include <strings.h>
#  include <errno.h>
#endif

#if defined(__APPLE__) && defined(__MACH__)
#  include <sys/random.h>
#endif

/* ======================================================================== */
/* Platfrom-specific functions                                              */
/* ======================================================================== */

/* Initialize state from the system's secure source of entropy */
bool xxh64prng_seed(xxh64prng_t *const state)
{
#ifdef _WIN32
    BCRYPT_ALG_HANDLE handle = NULL;
    bool succeeded = false;
    if (BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&handle, BCRYPT_RNG_ALGORITHM, NULL, 0U))) {
        if (BCRYPT_SUCCESS(BCryptGenRandom(handle, (PUCHAR)state, (ULONG)sizeof(xxh64prng_t), 0U))) {
            succeeded = true;
        }
        BCryptCloseAlgorithmProvider(handle, 0U);
    }
    return succeeded;
#else
    return (getentropy(state, sizeof(xxh64prng_t)) == 0); /* supported on pretty much all Unixes */
#endif
}

/* Securely zero-fill memory area */
void xxh64prng_zero(void *const addr, const size_t len)
{
#if defined(_WIN32)
    RtlSecureZeroMemory(addr, len);
#elif defined(__linux__) || defined(__CYGWIN__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__DragonFly__) || (defined(__sun) && defined(__SVR4)) || defined(__gnu_hurd__) || defined(__HAIKU__)
    explicit_bzero(addr, len);
#elif defined(__NetBSD__)
    explicit_memset(addr, 0, len);
#elif (defined(__APPLE__) && defined(__MACH__))
    memset_s(addr, RSIZE_MAX, 0, len);
#else
#pragma message("explicit_bzero() or equivalent is not supported on this platform, using fallback implementation!")
    volatile uint8_t *const vptr = (volatile uint8_t*) addr;
    size_t pos;
    for (pos = 0U; pos < len; ++pos) {
        vptr[pos] = 0U;
    }
#endif
}
