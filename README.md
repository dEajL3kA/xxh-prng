# XXH64-PRNG

A pseudo-random number generator based on the [XXH64](https://xxhash.com/) hash function.

Supported platforms include Windows, Linux, macOS, FreeBSD, OpenBSD, NetBSD, Solaris, Haiku OS and GNU Hurd.

It has been verified to successfully pass the full [Dieharder](https://webhome.phy.duke.edu/~rgb/General/dieharder.php) test suite &#128526;

## Usage

This section describes how to use the XXH64-PRNG command-line tool.

### Synopsis

```
xxh_rand.exe [OPTIONS] [SEED] [OUTPUT_SIZE]
```

### Options

* `--hex`  
  Output as hexadecimal string. Default is "raw" bytes.
* `--no-buffer`  
  Disable output buffering. Can be very slow!
* `--help`  
  Print help screen and exit.
* `--version`  
  Print version information and exit.

### Remarks

* If `SEED` is *not* specified (or set to `-`), uses a random seed from the OS' entropy source.
* If `OUTPUT_SIZE` is *not* specified, generates an indefinite amount of random bytes.

### Examples

1. Generate 1 GB of random data and dump to file as "raw" bytes:
   ```
   xxh_rand.exe - 1073741824 > random.out
   ```

2. Generate 32 bytes of random data and print as hexadecimal string:
   ```
   xxh_rand.exe --hex - 32
   ```

## Algorithm

XXH64-PRNG core "transition" function overview:

![](etc/images/xxh64-prng-core.svg)

XXH64-PRNG "output" function overview:

![](etc/images/xxh64-prng-ofun.svg)

## API Specification

This section describes the programming interface of the XXH64-PRNG library.

### xxh64prng_init()

Initialize (or reset) the XXH64-PRNG state from a 64-bit “seed” value.

Use this function to set up the PRNG when generating *reproducible* sequences of “random” numbers is desired.

- **Synopsis:**

  ```c
  void xxh64prng_init(xxh64prng_t *const state, const uint64_t seed);
  ```

- **Parameters:**

  * **`state`**: Pointer to the PRNG state of type `xxh64prng_t` that will be initialized.

  * **`seed`**: The seed value. Different seed values produce different sequences of “random” numbers.

### xxh64prng_seed()

Initialize (or reset) the XXH64-PRNG state from the system's secure entropy source.

Use this function to set up the PRNG when generating *unpredictable* sequences of “random” numbers is desired.

- **Synopsis:**

  ```c
  bool xxh64prng_seed(xxh64prng_t *const state);
  ```

- **Parameters:**

  * **`state`**: Pointer to the PRNG state of type `xxh64prng_t` that will be initialized.

- **Return value:**

  * **`true`**, if the state was initialized successfully.

  * **`false`**, if the system's entropy source could *not* provide the requested number of entropy bytes.

### xxh64prng_next()

Generate the next “block” of random data from the current XXH64-PRNG state. Also iterates the PRNG state.

- **Synopsis:**

  ```c
  void xxh64prng_next(xxh64prng_t *const state, uint64_t *const out);
  ```

- **Parameters:**

  * **`state`**: Pointer to the PRNG state of type `xxh64prng_t` that will be read and updated.

  * **`out`**: Pointer to the buffer of type `uint64_t[XXH64PRNG_OUTPUT_WORDS]` that is going to be filled.

- **Remarks:**
  
  * Even though the output buffer is defined as an array of `XXH64PRNG_OUTPUT_WORDS` words of type `uint64_t`, a pointer to this buffer can simply be cast to type `uint8_t*` in order to access the “raw” bytes!

  * The size of the output buffer, in bytes, is equal to `XXH64PRNG_OUTPUT_BYTES`, which is currently 768 bytes.

## Source Code

Official GitHub repository:

* <https://github.com/dEajL3kA/xxh-prng>

Mirrors:

* <https://gitlab.com/deajl3ka1/xxh64-prng>

* <https://repo.or.cz/xhh-prng.git>

## License

This work has been released under the **BSD 2-Clause "Simplified" License**.  
<https://opensource.org/license/bsd-2-clause/>

```
Copyright (c) 2025 "dEajL3kA" <Cumpoing79@web.de>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

### Acknowledgment

This work is based on code from the **xxHash** project:  
<https://github.com/Cyan4973/xxHash>
