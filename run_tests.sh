#!/bin/sh
set -e

if ! [ -x "bin/xxh_rand" ]; then
	echo "Error: Must build 'xxh_rand' before running the tests !!!"
	exit 1
fi

case "`uname -s`" in
	NetBSD|FreeBSD|OpenBSD|DragonFly)
		sha512cmd=sha512
		;;
	Darwin)
		sha512cmd="shasum -a512"
		;;
	*)
		sha512cmd="sha512sum -b"
		;;
esac

run_test_case() {
	echo "Computing checksum, please wait..."
	digest=`bin/xxh_rand $3 -- $1 1073741824 | $sha512cmd | cut -d' ' -f1`
	if [ "$digest" != "$2" ]; then
		echo "Test has failed: Checksum mismatch error !!!"
		printf "> computed: %s\n> expected: %s\n\n" "$digest" "$2"
		exit 1
	else
		printf "Success.\n\n"
	fi
}

bin/xxh_rand --version
printf "Git: %s\n\n" "`git describe --dirty`"

run_test_case 1 c3b859699c69271ac8c28dfeef270b336dfde71b9c9c441b6364a189baa9710b231f950cb5663bdf2f58a9c07ad7a411783082a5e7711b08544fa7333b32210a
run_test_case 2 397c3f14b786b1256faa16f8805584ccbf879370d57fc1561de43eb623677dfae35017440e71591f99f69a0cdbf9b7d8eab5e4c68ab2fda0a7e048d2b2204c1a

run_test_case 1 c8bb566c40b78f82d3cd693d1f3f4dbe0d33b0c5e4c842621a3f3494b910e77193f20c5c947edfeaf68bb80b766398c4d42f7508eb3803af02d9e715140f9f1c "--hex"
run_test_case 2 1441493d403335481de5dcce203d622a2ab158389809deaae96659a752973476cbd01f9753f60e1af580930ec67e8227b8747dcebbd310cbd0f7f8af3cadfff3 "--hex"

while true; do
	logfile=`mktemp -t run_tests-XXXXX.log`
	bin/xxh_rand | dieharder -g 200 -a -k 2 -Y 1 | tee $logfile
	if grep 'FAILED' < $logfile; then
		exit 1
	fi
	if ! grep 'WEAK' < $logfile; then
		break
	fi
done

echo "All tests completed."
