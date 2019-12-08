#!/bin/sh
SRCDIR=$1
shift
echo 'lunit.main({...})' | LUA_PATH="${SRCDIR}/?.lua" LUA_CPATH='.libs/lib?.so' lua -e "srcdir=\"${SRCDIR}\"" -llunit - "$@"
