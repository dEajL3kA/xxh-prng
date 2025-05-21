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

#include "xxh64.h"
#include "os_support.h"
#include "version.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
#  include <fcntl.h>
#  include <io.h>
#endif

static const unsigned int VERSION_MAJOR = XXH64PRNG_VERSION_MAJOR;
static const unsigned int VERSION_MINOR = XXH64PRNG_VERSION_MINOR;
static const unsigned int VERSION_PATCH = XXH64PRNG_VERSION_PATCH;

static const char *const BUILD_DATE = __DATE__;
static const char *const BUILD_ARCH = XXH64PRNG_ARCH;

#define ENABLE_DEBUG_LOGGING 0

#define STATE_WORDS 8U
#define STATE_BYTES ((size_t)(sizeof(uint64_t) * STATE_WORDS))

#define TEMP_MULTI 2U
#define TEMP_WORDS (TEMP_MULTI * STATE_WORDS)
#define TEMP_BYTES ((size_t)(sizeof(uint64_t) * TEMP_WORDS))

#define BUFFER_MULTI 8U
#define BUFFER_WORDS (BUFFER_MULTI * STATE_WORDS)
#define BUFFER_BYTES ((size_t)(sizeof(uint64_t) * BUFFER_WORDS))

#define HEX_BUFFLEN 256U
#define INITIALIZATION_BYTES 29U

/* ======================================================================== */
/* Utility functions                                                        */
/* ======================================================================== */

static INLINE int parse_uint64(const char *const str, uint64_t *const value)
{
    char *endptr = NULL;
    errno = 0;
    *value = strtoull(str, &endptr, 0);
    if (errno || ((!(*value)) && (endptr == str))) {
        return 0;
    }
    return 1;
}

static INLINE int fwrite_hexchars(const uint8_t *const buffer, const size_t length, FILE *const stream)
{
    static const char *const HEX_CHARS = "0123456789ABCDEF";
    char hexstr[HEX_BUFFLEN];
    size_t pos, counter = 0U;

    for (pos = 0U; pos < length; ++pos) {
        hexstr[counter++] = HEX_CHARS[buffer[pos] & 0xF];
        hexstr[counter++] = HEX_CHARS[buffer[pos] >> 4L];
        if (counter >= HEX_BUFFLEN) {
            counter = 0U;
            if (FWRITE(hexstr, sizeof(char), HEX_BUFFLEN, stream) != HEX_BUFFLEN) {
                return 0;
            }
        }
    }

    if (counter > 0U) {
        if (FWRITE(hexstr, sizeof(char), counter, stream) != counter) {
            return 0;
        }
    }

    return 1;
}

/* ======================================================================== */
/* Help screen                                                              */
/* ======================================================================== */

#ifdef _WIN32
#  define EXE_FILENAME "xxh_rand.exe"
#else
#  define EXE_FILENAME "xxh_rand"
#endif

static void print_helpscreen(const int full_help)
{
    if (VERSION_PATCH) {
        printf("XXH64-PRNG v%u.%u-%u [%s] [%s]\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, BUILD_ARCH, BUILD_DATE);
    }
    else {
        printf("XXH64-PRNG v%u.%u [%s] [%s]\n", VERSION_MAJOR, VERSION_MINOR, BUILD_ARCH, BUILD_DATE);
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
/* Initialization                                                           */
/* ======================================================================== */

static const char *const INITIALIZATION_DATA[STATE_WORDS] = {
    "\xB7\xE1\x51\x62\x8A\xED\x2A\x6A\xBF\x71\x58\x80\x9C\xF4\xF3\xC7\x62\xE7\x16\x0F\x38\xB4\xDA\x56\xA7\x84\xD9\x04\x51",
    "\x90\xCF\xEF\x32\x4E\x77\x38\x92\x6C\xFB\xE5\xF4\xBF\x8D\x8D\x8C\x31\xD7\x63\xDA\x06\xC8\x0A\xBB\x11\x85\xEB\x4F\x7C",
    "\x7B\x57\x57\xF5\x95\x84\x90\xCF\xD4\x7D\x7C\x19\xBB\x42\x15\x8D\x95\x54\xF7\xB4\x6B\xCE\xD5\x5C\x4D\x79\xFD\x5F\x24",
    "\xD6\x61\x3C\x31\xC3\x83\x9A\x2D\xDF\x8A\x9A\x27\x6B\xCF\xBF\xA1\xC8\x77\xC5\x62\x84\xDA\xB7\x9C\xD4\xC2\xB3\x29\x3D",
    "\x20\xE9\xE5\xEA\xF0\x2A\xC6\x0A\xCC\x93\xED\x87\x44\x22\xA5\x2E\xCB\x23\x8F\xEE\xE5\xAB\x6A\xDD\x83\x5F\xD1\xA0\x75",
    "\x3D\x0A\x8F\x78\xE5\x37\xD2\xB9\x5B\xB7\x9D\x8D\xCA\xEC\x64\x2C\x1E\x9F\x23\xB8\x29\xB5\xC2\x78\x0B\xF3\x87\x37\xDF",
    "\x8B\xB3\x00\xD0\x13\x34\xA0\xD0\xBD\x86\x45\xCB\xFA\x73\xA6\x16\x0F\xFE\x39\x3C\x48\xCB\xBB\xCA\x06\x0F\x0F\xF8\xEC",
    "\x6D\x31\xBE\xB5\xCC\xEE\xD7\xF2\xF0\xBB\x08\x80\x17\x16\x3B\xC6\x0D\xF4\x5A\x0E\xCB\x1B\xCD\x28\x9B\x06\xCB\xBF\xEA"
};

static INLINE void xxh64prng_init(uint64_t *const state, const uint64_t seed)
{
    size_t pos;
    const char *const *input = INITIALIZATION_DATA;

    for (pos = 0U; pos < STATE_WORDS; ++pos) {
        state[pos] = XXH64(*input++, INITIALIZATION_BYTES, seed);
    }
}

/* ======================================================================== */
/* Random generator                                                         */
/* ======================================================================== */

static const uint64_t ZERO_STATE[STATE_WORDS] = { 0U };

static const uint64_t SEED[BUFFER_WORDS] = {
    UINT64_C(0x243F6A8885A308D3), UINT64_C(0x13198A2E03707344), UINT64_C(0xA4093822299F31D0), UINT64_C(0x082EFA98EC4E6C89),
    UINT64_C(0x452821E638D01377), UINT64_C(0xBE5466CF34E90C6C), UINT64_C(0xC0AC29B7C97C50DD), UINT64_C(0x3F84D5B5B5470917),
    UINT64_C(0x9216D5D98979FB1B), UINT64_C(0xD1310BA698DFB5AC), UINT64_C(0x2FFD72DBD01ADFB7), UINT64_C(0xB8E1AFED6A267E96),
    UINT64_C(0xBA7C9045F12C7F99), UINT64_C(0x24A19947B3916CF7), UINT64_C(0x0801F2E2858EFC16), UINT64_C(0x636920D871574E69),
    UINT64_C(0xA458FEA3F4933D7E), UINT64_C(0x0D95748F728EB658), UINT64_C(0x718BCD5882154AEE), UINT64_C(0x7B54A41DC25A59B5),
    UINT64_C(0x9C30D5392AF26013), UINT64_C(0xC5D1B023286085F0), UINT64_C(0xCA417918B8DB38EF), UINT64_C(0x8E79DCB0603A180E),
    UINT64_C(0x6C9E0E8BB01E8A3E), UINT64_C(0xD71577C1BD314B27), UINT64_C(0x78AF2FDA55605C60), UINT64_C(0xE65525F3AA55AB94),
    UINT64_C(0x5748986263E81440), UINT64_C(0x55CA396A2AAB10B6), UINT64_C(0xB4CC5C341141E8CE), UINT64_C(0xA15486AF7C72E993),
    UINT64_C(0xB3EE1411636FBC2A), UINT64_C(0x2BA9C55D741831F6), UINT64_C(0xCE5C3E169B87931E), UINT64_C(0xAFD6BA336C24CF5C),
    UINT64_C(0x7A32538128958677), UINT64_C(0x3B8F48986B4BB9AF), UINT64_C(0xC4BFE81B66282193), UINT64_C(0x61D809CCFB21A991),
    UINT64_C(0x487CAC605DEC8032), UINT64_C(0xEF845D5DE98575B1), UINT64_C(0xDC262302EB651B88), UINT64_C(0x23893E81D396ACC5),
    UINT64_C(0x0F6D6FF383F44239), UINT64_C(0x2E0B4482A4842004), UINT64_C(0x69C8F04A9E1F9B5E), UINT64_C(0x21C66842F6E96C9A),
    UINT64_C(0x670C9C61ABD388F0), UINT64_C(0x6A51A0D2D8542F68), UINT64_C(0x960FA728AB5133A3), UINT64_C(0x6EEF0B6C137A3BE4),
    UINT64_C(0xBA3BF0507EFB2A98), UINT64_C(0xA1F1651D39AF0176), UINT64_C(0x66CA593E82430E88), UINT64_C(0x8CEE8619456F9FB4),
    UINT64_C(0x7D84A5C33B8B5EBE), UINT64_C(0xE06F75D885C12073), UINT64_C(0x401A449F56C16AA6), UINT64_C(0x4ED3AA62363F7706),
    UINT64_C(0x1BFEDF72429B023D), UINT64_C(0x37D0D724D00A1248), UINT64_C(0xDB0FEAD349F1C09B), UINT64_C(0x075372C980991B7B)
};

static INLINE void xxh64prng_step(uint64_t *const state, uint64_t *const value)
{
    uint64_t temp_state[TEMP_MULTI][STATE_WORDS];
    size_t pos;
    const uint64_t *seed = SEED;

    do {
        for (pos = 0U; pos < TEMP_WORDS; ++pos) {
            ((uint64_t*)temp_state)[pos] = XXH64(state, STATE_BYTES, *seed++);
        }
    } while ((!memcmp(temp_state[0U], ZERO_STATE, STATE_BYTES)) || (!memcmp(temp_state[1U], ZERO_STATE, STATE_BYTES)));

    for (pos = 0U; pos < STATE_WORDS; ++pos) {
        value[pos]  = temp_state[1U][pos] ^ state[pos];
        state[pos] ^= temp_state[0U][pos];
    }

    zero_memory((uint8_t*)temp_state, TEMP_BYTES);
}

static INLINE void xxh64prng_next(uint64_t *const state, uint64_t *const output)
{
    uint64_t temp_state[TEMP_MULTI][STATE_WORDS];
    size_t pos;
    const uint64_t *seed = SEED;

#if ENABLE_DEBUG_LOGGING
    fprintf(stderr, "state: %016llX %016llX %016llX %016llX %016llX %016llX %016llX %016llX\n", state[0U], state[1U], state[2U], state[3U], state[4U], state[5U], state[6U], state[7U]);
#endif

    xxh64prng_step(state, temp_state[0U]);
    xxh64prng_step(state, temp_state[1U]);

#if ENABLE_DEBUG_LOGGING
    fprintf(stderr, "temp0: %016llX %016llX %016llX %016llX %016llX %016llX %016llX %016llX\n", temp_state[0U][0U], temp_state[0U][1U], temp_state[0U][2U], temp_state[0U][3U], temp_state[0U][4U], temp_state[0U][5U], temp_state[0U][6U], temp_state[0U][7U]);
    fprintf(stderr, "temp1: %016llX %016llX %016llX %016llX %016llX %016llX %016llX %016llX\n", temp_state[1U][0U], temp_state[1U][1U], temp_state[1U][2U], temp_state[1U][3U], temp_state[1U][4U], temp_state[1U][5U], temp_state[1U][6U], temp_state[1U][7U]);
#endif

    for (pos = 0U; pos < BUFFER_WORDS; ++pos) {
        output[pos] = XXH64(temp_state[1U], STATE_BYTES, XXH64(temp_state[0U], STATE_BYTES, *seed++));
    }

    zero_memory((uint8_t*)temp_state, TEMP_BYTES);
}

/* ======================================================================== */
/* MAIN                                                                     */
/* ======================================================================== */

int main(int argc, char *argv[])
{
    int index = 1, hex_output = 0, no_buffer = 0, show_help = 0, is_seeded = 0;
    size_t chunk_size;
    uint64_t remaining = UINT64_MAX, buffer[BUFFER_WORDS], state[STATE_WORDS];

    while (index < argc) {
        if (((argv[index][0] == '/') || (argv[index][0] == '-')) && (argv[index][1] == '?')) {
            ++index;
            show_help = 2;
        }
        else if ((argv[index][0] == '-') && (argv[index][1] == '-')) {
            const char *const arg = argv[index++] + 2U;
            if (!(*arg)) {
                break; /*no more options*/
            }
            else if (STRICMP(arg, "hex") == 0) {
                hex_output = 1;
            }
            else if (STRICMP(arg, "no-buffer") == 0) {
                no_buffer = 1;
            }
            else if (STRICMP(arg, "version") == 0) {
                if (!show_help) {
                    show_help = 1;
                }
            }
            else if (STRICMP(arg, "help") == 0) {
                show_help = 2;
            }
            else {
                fprintf(stderr, "Error: Unknown option! (\"--%s\")\n", arg);
                return EXIT_FAILURE;
            }
        }
        else {
            break; /*no more options*/
        }
    }

    if (show_help) {
        print_helpscreen(show_help > 1);
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
                fprintf(stderr, "Error: Invalid seed value! (\"%s\")\n", argv[index]);
                return EXIT_FAILURE;
            }
            xxh64prng_init(state, seed);
            is_seeded = 1;
        }
        ++index;
    }

    if (!is_seeded) {
        if (!read_entropy((uint8_t*)state, STATE_BYTES)) {
            fputs("Error: Failed to acquire seed from the system entropy source!\n", stderr);
            return EXIT_FAILURE;
        }
    }

    if (index < argc) {
        if ((*argv[index]) && (STRICMP(argv[index], "-") != 0)) {
            if (!parse_uint64(argv[index], &remaining)) {
                fprintf(stderr, "Error: Invalid output size value! (\"%s\")\n", argv[index]);
                return EXIT_FAILURE;
            }
        }
        ++index;
    }

    if (index < argc) {
        fputs("Warning: Ignoring the excess argument(s)!\n", stderr);
    }

    while (remaining) {
        chunk_size = (remaining >= BUFFER_BYTES) ? BUFFER_BYTES : ((size_t)remaining);

        xxh64prng_next(state, buffer);

        if (!hex_output) {
            if (FWRITE(buffer, 1U, chunk_size, stdout) != chunk_size) {
                break;
            }
        } else {
            if (!fwrite_hexchars((const uint8_t*)buffer, chunk_size, stdout)) {
                break;
            }
        }

        zero_memory((uint8_t*)buffer, BUFFER_BYTES);

        if (remaining != UINT64_MAX) {
            remaining -= chunk_size;
        }
    }

    zero_memory((uint8_t*)state, STATE_BYTES);
    zero_memory((uint8_t*)buffer, BUFFER_BYTES);

    return EXIT_SUCCESS;
}
