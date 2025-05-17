#!/bin/sh
set -e

make EXTRA_CFLAGS="-fsanitize=address -fno-omit-frame-pointer -static-libasan -g" SUFFIX=asan

for h in "" "--hex"; do
	for i in "-" "1" "2"; do
		( set -x; ASAN_OPTIONS="atexit=true" ./bin/xxh_rand.asan ${h} -- ${i} 1073741824 > /dev/null; )
		echo "--------"
	done
done

echo "All tests completed."
