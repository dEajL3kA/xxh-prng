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

run_test_case 1 95958f9d5db86b9ca9dbb4ed4655b40844e1c4405743b18e0296162d4879b8d67ae5c4f51e62638b2d0167473c962414c0903a4c17a1af1b54c85c4c71743083
run_test_case 2 29757303dadd627bb757b7feb40994143562eba70cd95ac0d32244fb0bebeb70ab942fe56afde15c7969988bb30539d8a47c156e28dc4f43df9eedf297dff800

run_test_case 1 59212d699c852a1fc8e5a729114eaab5abc1a4ef3e38c9b814fb34918e6544423d98eca133b0d7e657f74abdfea14858a1f0c406d8fd192d2358b605f527f063 "--hex"
run_test_case 2 1e95b1ba9870730e548cf4431ff155f619e3ad07c269234f90aef9c465ee9fe4e521f00471d4b34039872aa666daa6438b89e18cfdd2c23239b19a11c96beb5a "--hex"

bin/xxh_rand | dieharder -g 200 -a -k 2 -Y 1

echo "All tests completed."
