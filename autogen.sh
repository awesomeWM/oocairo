#!/bin/sh

set -e

# Allow for out-of-tree builds
srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

do_cmd() {
	echo "* Running $@"
	$@ || exit 1
}
(
do_cmd cd "$srcdir"
do_cmd autoreconf --install
)

if test -z "$NOCONFIGURE"; then
	do_cmd "$srcdir/configure" --cache-file=config.cache $@ &&
		( echo ; echo "Now type 'make' to start compiling" )
fi
