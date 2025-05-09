.PHONY: all

all:
	mkdir -p bin
	gcc -Wall -O3 -march=native -static -static -static-libgcc -mcrtdll=msvcr120 -DNDEBUG -o bin/xxh_rand.exe $(wildcard src/*.c) -lbcrypt
