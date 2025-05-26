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

#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
#  include <fcntl.h>
#  include <io.h>
#endif

#if defined(_WIN32)
#  define FWRITE_NOLOCK _fwrite_nolock
#elif defined(__linux__) || defined(__CYGWIN__) || defined(__FreeBSD__) || defined(__HAIKU__)
#  define FWRITE_NOLOCK fwrite_unlocked
#else
#  define FWRITE_NOLOCK fwrite
#endif

#ifdef _WIN32
#  define STRICMP _stricmp
#else
#  define STRICMP strcasecmp
#endif

/* ======================================================================== */
/* Utility functions                                                        */
/* ======================================================================== */

#define HEX_BUFFLEN 256U

static const char* const HEX_CHARS = "0123456789ABCDEF";

static int fwrite_hexchars(const void *const input, const size_t length, FILE *const stream)
{
    const uint8_t *const data = (const uint8_t*)input;
    size_t pos, offset = 0U;
    char hex_buffer[HEX_BUFFLEN];

    for (pos = 0U; pos < length; ++pos) {
        hex_buffer[offset++] = HEX_CHARS[data[pos] & 0xF];
        hex_buffer[offset++] = HEX_CHARS[data[pos] >> 4L];
        if (offset >= HEX_BUFFLEN) {
            offset = 0U;
            if (FWRITE_NOLOCK(hex_buffer, sizeof(char), HEX_BUFFLEN, stream) != HEX_BUFFLEN) {
                return 0;
            }
        }
    }

    if (offset > 0U) {
        if (FWRITE_NOLOCK(hex_buffer, sizeof(char), offset, stream) != offset) {
            return 0;
        }
    }

    return 1;
}

static bool parse_uint64(const char* const str, uint64_t* const value)
{
    char* endptr = NULL;
    errno = 0;
    *value = strtoull(str, &endptr, 0);
    if (errno || ((!(*value)) && (endptr == str))) {
        return false;
    }
    return true;
}

/* ======================================================================== */
/* Help screen                                                              */
/* ======================================================================== */

#ifdef _WIN32
#  define EXE_FILENAME "xxh_rand.exe"
#else
#  define EXE_FILENAME "xxh_rand"
#endif

static void print_helpscreen(const bool full_help)
{
    char version_str[32U];

    const int len = snprintf(version_str, 32U, XXH64PRNG_VERSION_PATCH ? "%u.%u-%u" : "%u.%u", XXH64PRNG_VERSION_MAJOR, XXH64PRNG_VERSION_MINOR, XXH64PRNG_VERSION_PATCH);
    if ((len > 0) && (len < 32)) {
        printf("XXH64-PRNG v%s [%s] [%s]\n", version_str, XXH64PRNG_ARCH, XXH64PRNG_DATE);
    }

    if (full_help) {
        puts("Blazing fast XXH64-based secure pseudo-random number generator\n");
        puts("Synopsis:");
        puts("  " EXE_FILENAME " [OPTIONS] [SEED] [OUTPUT_SIZE]\n");
        puts("Options:");
        puts("  --hex        Output as hexadecimal string. Default is \"raw\" bytes.");
        puts("  --no-buffer  Disable output buffering. Can be very slow!");
        puts("  --help       Print help screen and exit.");
        puts("  --version    Print version information and exit.\n");
        puts("If SEED is *not* specified (or set to \"-\"), uses a random seed from the OS' entropy source.");
        puts("If OUTPUT_SIZE is *not* specified, generates an indefinite amount of random bytes.");
    }
}

/* ======================================================================== */
/* Generation loop                                                          */
/* ======================================================================== */

static void generate_loop(xxh64prng_t *const state, uint64_t remaining, const bool hex_output)
{
    uint64_t buffer[XXH64PRNG_OUTPUT_WORDS];

    while (remaining) {
        const size_t chunk_size = (remaining >= XXH64PRNG_OUTPUT_BYTES) ? XXH64PRNG_OUTPUT_BYTES : ((size_t)remaining);

        xxh64prng_next(state, buffer);

        if (!hex_output) {
            if (FWRITE_NOLOCK(buffer, 1U, chunk_size, stdout) != chunk_size) {
                break;
            }
        }
        else {
            if (!fwrite_hexchars(buffer, chunk_size, stdout)) {
                break;
            }
        }

        xxh64prng_zero(buffer, sizeof(buffer));

        if (remaining != UINT64_MAX) {
            remaining -= chunk_size;
        }
    }

    xxh64prng_zero(buffer, sizeof(buffer));
}

/* ======================================================================== */
/* MAIN                                                                     */
/* ======================================================================== */

int main(int argc, char *argv[])
{
    int index = 1, exit_code = EXIT_FAILURE;
    bool hex_output = false, no_buffer = false, show_help = false, full_help = false, is_seeded = false;
    xxh64prng_t state;
    uint64_t output_size = UINT64_MAX;

    while (index < argc) {
        if (((argv[index][0] == '/') || (argv[index][0] == '-')) && (argv[index][1] == '?')) {
            show_help = true;
            full_help = true;
            break; /*no more options*/
        }
        else if ((argv[index][0] == '-') && (argv[index][1] == '-')) {
            const char *const arg = argv[index++] + 2U;
            if (!(*arg)) {
                break; /*no more options*/
            }
            else if (STRICMP(arg, "hex") == 0) {
                hex_output = true;
            }
            else if (STRICMP(arg, "no-buffer") == 0) {
                no_buffer = true;
            }
            else if (STRICMP(arg, "help") == 0) {
                show_help = true;
                full_help = true;
            }
            else if (STRICMP(arg, "version") == 0) {
                show_help = true;
            }
            else {
                fprintf(stderr, "Error: Option \"--%s\" is not supported!\n", arg);
                return EXIT_FAILURE;
            }
        }
        else {
            break; /*no more options*/
        }
    }

    if (show_help) {
        print_helpscreen(full_help);
        return EXIT_SUCCESS;
    }

#ifdef _WIN32
    if (_setmode(_fileno(stdout), O_BINARY) == (-1)) {
        abort();
    }
#endif

    if (no_buffer) {
        if (setvbuf(stdout, NULL, _IONBF, 0U) != 0) {
            abort();
        }
    }

    if (index < argc) {
        if ((*argv[index]) && (STRICMP(argv[index], "-") != 0)) {
            uint64_t seed;
            if (!parse_uint64(argv[index], &seed)) {
                fprintf(stderr, "Error: Seed value \"%s\" could not be parsed!\n", argv[index]);
                goto clean_up;
            }
            xxh64prng_init(&state, seed);
            is_seeded = true;
        }
        ++index;
    }

    if (index < argc) {
        if ((*argv[index]) && (STRICMP(argv[index], "-") != 0)) {
            if (!parse_uint64(argv[index], &output_size)) {
                fprintf(stderr, "Error: Output size \"%s\" could not be parsed!\n", argv[index]);
                goto clean_up;
            }
        }
        ++index;
    }

    if (index < argc) {
        fputs("Warning: Ignoring the excess argument(s)!\n", stderr);
    }

    if (!(is_seeded || xxh64prng_seed(&state))) {
        fputs("Error: Failed to acquire seed data from the system's entropy source!\n", stderr);
        goto clean_up;
    }

    generate_loop(&state, output_size, hex_output);

    exit_code = EXIT_SUCCESS;

clean_up:

    xxh64prng_zero(&state, sizeof(xxh64prng_t));

    return exit_code;
}
