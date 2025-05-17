#!/bin/sh
set -e

if ! [ -x "bin/xxh_rand" ]; then
	echo "Error: Must build 'xxh_rand' before running the tests !!!"
	exit 1
fi

run_test_case() {
	echo "Computing checksum, please wait..."
	local digest=`bin/xxh_rand $3 -- $1 1073741824 | sha512sum -b | cut -d' ' -f1`
	if [ "$digest" != "$2" ]; then
		echo "Test has failed: Checksum mismatch error !!!"
		printf "> computed: %s\n> expected: %s\n\n" "$digest" "$2"
		exit 1
	else
		printf "Success.\n\n"
	fi
}

run_test_case 1 abbf0374e11212fe96ab1999980c453ad34426947585d1cdd748cb830d5960da1a664ff565126db322f8edd298cd96ce4292507e318e09c8088b9c3f29d6bef9
run_test_case 2 aa6a83138bd4e85c6ab1b8fc1afd9d9579f219125733bbe4cac121ae07e58b00f05c8b66ff4feb869f7038c20e03196cf1ef38b06589c6a73fb23752846dad46

run_test_case 1 c2123cbdb8a7e3d515b33be1960a4594b4e61a49a3aca8569fec1c78855344ba3ea7e348f6cf13ec51392a3886535d7aa2c2706f89c92b69b15db38cf62018bb "--hex"
run_test_case 2 f06f5a0670150e13195c19081e0611fd07590cc97f96f351b286a9213497cad283fa2492810f4c3f2d075e258f7bc5d72fffbeb91de70c528cc6830fb54ad2fa "--hex"

bin/xxh_rand | dieharder -g 200 -a -k 2 -Y 1

echo "All tests completed."
