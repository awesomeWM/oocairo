#!/bin/sh
SRCDIR=$1
shift
echo 'require"lunit".main({...})' | LUA_PATH="${SRCDIR}/?.lua" LUA_CPATH='.libs/lib?.so' lua -e "srcdir=\"${SRCDIR}\"" - "$@"
