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

#include "xxh64.h"
#include "os_support.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
#  include <fcntl.h>
#  include <io.h>
#endif

static const unsigned int VERSION_MAJOR = 1U;
static const unsigned int VERSION_MINOR = 0U;

#define STATE_WORDS 8U
#define STATE_BYTES ((size_t)(sizeof(uint64_t) * STATE_WORDS))

#define BUFFR_MULTP 8U
#define BUFFR_WORDS (BUFFR_MULTP * STATE_WORDS)

#define MAX_CHUNKSIZE ((size_t)(sizeof(uint64_t) * (BUFFR_MULTP - 1U) * STATE_WORDS))

static int parse_uint64(const char *const str, uint64_t *const value)
{
    char *endptr = NULL;
    errno = 0;
    *value = strtoull(str, &endptr, 0);
    if (errno || ((!(*value)) && (endptr == str))) {
        return 0;
    }
    return 1;
}

static int fwrite_hexchars(const uint8_t *const buffer, const size_t length, FILE *const stream)
{
    static const char* const HEX_CHARS = "0123456789ABCDEF";
    size_t pos, shift;
    for (pos = 0U; pos < length; ++pos) {
        for (shift = 0U; shift <= 4U; shift += 4U) {
            if (FPUTC(HEX_CHARS[(buffer[pos] >> shift) & 0xF], stream) == EOF) {
                return 0;
            }
        }
    }
    return 1;
}

int main(int argc, char *argv[])
{
    int index = 1, hex_output = 0, no_buffer = 0, show_help = 0, is_seeded = 0;
    size_t chunk_size, pos;
    uint64_t remaining = UINT64_MAX, rand_buffer[BUFFR_WORDS], state[STATE_WORDS] = { 0U };

    while (index < argc) {
        if ((argv[index][0] == '-') && (argv[index][1] == '-')) {
            const char *const arg = argv[index++] + 2U;
            if (!(*arg)) {
                break; /*no more options*/
            }
            else if (STRICMP(arg, "hexstr") == 0) {
                hex_output = 1;
            }
            else if (STRICMP(arg, "no-buffer") == 0) {
                no_buffer = 1;
            }
            else if (STRICMP(arg, "version") == 0) {
                show_help = 1;
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
        printf("XXH64-based pseudo-random number generator v%u.%u [" __DATE__ "]\n", VERSION_MAJOR, VERSION_MINOR);
        if (show_help > 1) {
            puts("\nSynopsis:");
            puts("   xxh_rand.exe [OPTIONS] [SEED] [OUTPUT_SIZE]\n");
            puts("Options:");
            puts("   --hexstr     Output as hexadecimal string. Default is \"raw\" bytes.");
            puts("   --no-buffer  Disable output buffering. Can be very slow!");
            puts("   --help       Print help screen and exit.");
            puts("   --version    Print version information and exit.\n");
            puts("If SEED is *not* specified (or set to \"-\"), uses a random seed from the OS' entropy source.");
            puts("If OUTPUT_SIZE is *not* specified, generates an indefinite amount of random bytes.");
        }
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
            if (!parse_uint64(argv[index], &state[0U])) {
                fprintf(stderr, "Error: Invalid seed value! (\"%s\")\n", argv[index]);
                return EXIT_FAILURE;
            }
            for (pos = 1U; pos < STATE_WORDS; ++pos) {
                state[pos] = XXH64(&state[0U], sizeof(uint64_t), 42U + pos);
            }
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
#ifdef _DEBUG
        fprintf(stderr, "%016llX %016llX %016llX %016llX %016llX %016llX %016llX %016llX\n", state[0U], state[1U], state[2U], state[3U], state[4U], state[5U], state[6U], state[7U]);
#endif

        for (pos = 0U; pos < BUFFR_WORDS; ++pos) {
            rand_buffer[pos] = XXH64(state, STATE_BYTES, pos);
        }

        for (pos = 0U; pos < STATE_WORDS; ++pos) {
            state[pos] ^= rand_buffer[pos];
        }

        zero_memory((uint8_t*)rand_buffer, STATE_BYTES);

        chunk_size = (remaining >= MAX_CHUNKSIZE) ? MAX_CHUNKSIZE : ((size_t)remaining);

        if (!hex_output) {
            if (FWRITE(rand_buffer + STATE_WORDS, chunk_size, 1U, stdout) != 1U) {
                break;
            }
        } else {
            if (!fwrite_hexchars((const uint8_t*)(rand_buffer + STATE_WORDS), chunk_size, stdout)) {
                break;
            }
        }

        if (remaining != UINT64_MAX) {
            remaining -= chunk_size;
        }
    }

    zero_memory((uint8_t*)rand_buffer, sizeof(rand_buffer));
    zero_memory((uint8_t*)state, sizeof(state));

    return EXIT_SUCCESS;
}
