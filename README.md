# XXH64-PRNG

A pseudo-random number generator based on the [XXH64](https://xxhash.com/) hash function.

Supported platforms include Windows, Linux, macOS, FreeBSD, OpenBSD, NetBSD, Solaris, Haiku OS and GNU Hurd.

## Usage

Synopsis:
```
xxh_rand.exe [OPTIONS] [SEED] [OUTPUT_SIZE]
```

Options:
* `--hex`  
  Output as hexadecimal string. Default is "raw" bytes.
* `--no-buffer`  
  Disable output buffering. Can be very slow!
* `--help`  
  Print help screen and exit.
* `--version`  
  Print version information and exit.

Remarks:
* If `SEED` is *not* specified (or set to `-`), uses a random seed from the OS' entropy source.
* If `OUTPUT_SIZE` is *not* specified, generates an indefinite amount of random bytes.

## Examples

1. Generate 1 GB of random data and dump to file as "raw" bytes:
   ```
   xxh_rand.exe - 1073741824 > random.out
   ```

2. Generate 32 bytes of random data and print as hexadecimal string:
   ```
   xxh_rand.exe --hex - 32
   ```

## License

XXH64-PRNG and xxHash are released under the **BSD 2-Clause License**.

Please refer to <https://www.opensource.org/licenses/bsd-license.php> for details!
