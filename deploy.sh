#!/bin/sh
set -e

# --------------------------------------------------
# Select compiler and make
# --------------------------------------------------

uname_s=`uname -s 2> /dev/null || true`
uname_p=`uname -p 2> /dev/null || true`

if [ $# -gt 0 ]; then
	cc="$1"
else
	case "$uname_s" in
		Linux)
			cc="musl-gcc"
			;;
		CYGWIN*|SunOS|NetBSD|DragonFly|GNU)
			cc="gcc"
			;;
		Darwin|FreeBSD|OpenBSD)
			cc="clang"
			;;
		MINGW32*|MINGW64*)
			case "$MSYSTEM" in
				MINGW*)
					cc="gcc"
					;;
				CLANG*)
					cc="clang"
					;;
				*)
					echo "Error: MSYS environment \"$MSYSTEM\" is not recognized!"
					exit 1
					;;
			esac
			;;
		Haiku)
			case "$uname_p" in
				x86)
					cc="gcc-x86"
					;;
				*)
					cc="gcc"
					;;
			esac
			;;
		*)
			echo "Error: System name \"$uname_s\" is not recognized!"
			exit 1
			;;
	esac
fi

if [ $# -gt 1 ]; then
	strip="$2"
else
	case "$uname_s" in
		Haiku)
			case "$uname_p" in
				x86)
					strip="strip-x86"
					;;
				*)
					strip="strip"
					;;
			esac
			;;
		*)
			strip="strip"
			;;
	esac
fi

if [ $# -gt 2 ]; then
	make="$3"
else
	case "$uname_s" in
		FreeBSD|OpenBSD|NetBSD|DragonFly|SunOS)
			make="gmake"
			;;
		*)
			make="make"
			;;
	esac
fi

if ! which "$cc" > /dev/null 2>&1; then
	echo "Executable for \"$cc\" not found!"
	exit 1
fi

if ! which "$strip" > /dev/null 2>&1; then
	echo "Executable for \"$strip\" not found!"
	exit 1
fi

if ! which "$make" > /dev/null 2>&1; then
	echo "Executable for \"$make\" not found!"
	exit 1
fi

# --------------------------------------------------
# Set platform-specific flags
# --------------------------------------------------

unset xlibs suffix rescomp
xflags="-flto"
target=`$cc -dumpmachine`

if [ -z "$target" ]; then
	echo "Error: Failed to detect target platform!"
	exit 1
fi

case "$cc" in
	*gcc*)
		xflags="-Ofast $xflags"
		;;
	*)
		xflags="-O3 $xflags"
		;;
esac

case "$target" in
	*-w64-mingw32|*-pc-cygwin|*-w64-windows*)
		suffix=exe
		rescomp=windres
		;;
esac

case "$target" in
	x86_64-*|x86-64-*|amd64-*)
		xflags="$xflags -march=x86-64"
		;;
	i[3456]86-*|x86-*)
		xflags="$xflags -march=pentium3 -mtune=generic"
		;;
esac

case "$target" in
	*-w64-mingw32|*-w64-windows*)
		xlibs="-lbcrypt"
		;;
	*-haiku)
		xlibs="-lbsd"
		;;
esac

case "$target" in
	*-w64-mingw32)
		xflags="$xflags -mcrtdll=msvcr120"
		;;
esac

case "$target" in
	*-linux-gnu|*-w64-mingw32|*-w64-windows*|*-cygwin|*-haiku|*-freebsd*|*-openbsd*|*-netbsd*|*-dragonflybsd)
		xflags="$xflags -static"
		;;
esac

# --------------------------------------------------
# Build!
# --------------------------------------------------

set -x

$make CC="$cc" STRIP="$strip" RESCOMP="$rescomp" EXTRA_CFLAGS="$xflags" EXTRA_LIBS="$xlibs" SUFFIX="$suffix"
