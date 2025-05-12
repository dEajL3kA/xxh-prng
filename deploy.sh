#!/bin/sh
set -e

# --------------------------------------------------
# Select compiler and make
# --------------------------------------------------

host_name=`uname -s`

if [ "$host_name" = "Haiku" ] && [ "`uname -p`" = "x86" ]; then
	export PATH="`setarch -p x86`"
fi

case "$host_name" in
	Linux)
		cc="musl-gcc"
		;;
	MINGW32_NT*|MINGW64_NT*|CYGWIN_NT*|SunOS|NetBSD|GNU|Haiku)
		cc="gcc"
		;;
	Darwin|FreeBSD|OpenBSD)
		cc="clang"
		;;
	*)
		echo "Error: Host platform \"$host_name\" is not recognized!"
		exit 1
		;;
esac

case "$host_name" in
	FreeBSD|OpenBSD|NetBSD|SunOS)
		make="gmake"
		;;
	*)
		make="make"
		;;
esac

if ! which "$cc" > /dev/null 2>&1; then
	echo "Compiler executable for \"$cc\" not found"!
	exit 1
fi

if ! which "$make" > /dev/null 2>&1; then
	echo "Make executable for \"$make\" not found"!
	exit 1
fi

# --------------------------------------------------
# Set platform-specific flags
# --------------------------------------------------

unset xlibs suffix
xflags="-flto"
target_name=`$cc -dumpmachine`

if [ -z "$target_name" ]; then
	echo "Error: Failed to detect target platform!"
	exit 1
fi

case "$target_name" in
	*-w64-mingw32|*-pc-cygwin|*-w64-windows-gnu)
		suffix="exe"
		;;
esac

case "$target_name" in
	x86_64-*)
		xflags="$xflags -march=x86-64"
		;;
	i686-*)
		xflags="$xflags -march=pentium3 -mtune=generic"
		;;
esac

case "$target_name" in
	*-w64-mingw32)
		xflags="$xflags -mcrtdll=msvcr120"
		xlibs="-lbcrypt"
		;;
	*-haiku)
		xlibs="-lbsd"
		;;
esac

case "$target_name" in
	*-linux-gnu|*-w64-mingw32|*-cygwin|*-haiku|*-freebsd*|*-openbsd*|*-netbsd)
		xflags="$xflags -static"
		;;
esac

# --------------------------------------------------
# Build!
# --------------------------------------------------

set -x

$make CC="$cc" EXTRA_CFLAGS="$xflags" EXTRA_LIBS="$xlibs" SUFFIX="$suffix"
