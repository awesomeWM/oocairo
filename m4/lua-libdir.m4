dnl Macros to determine lua's c module path
dnl
dnl This software and documentation is distributed under the same terms as
dnl Lua version 5.0, the MIT/X Consortium license.  The full terms are as
dnl follows:
dnl
dnl Copyright (C) 2010      Uli Schlachter
dnl
dnl Permission is hereby granted, free of charge, to any person obtaining a copy
dnl of this software and associated documentation files (the "Software"), to deal
dnl in the Software without restriction, including without limitation the rights
dnl to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
dnl copies of the Software, and to permit persons to whom the Software is
dnl furnished to do so, subject to the following conditions:
dnl
dnl The above copyright notice and this permission notice shall be included in
dnl all copies or substantial portions of the Software.
dnl
dnl THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
dnl IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
dnl FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
dnl AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
dnl LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
dnl OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
dnl THE SOFTWARE.

dnl Determine lua's library dir. You should have used PKG_CHECK_MODULES() to
dnl check for lua before calling this macro.
dnl LUA_LIBDIR([LUA-PACKAGE], [ACTION-IF-FOUND],[ACTION-IF-NOT-FOUND])

AC_DEFUN([LUA_LIBDIR], [
  AC_REQUIRE([PKG_PROG_PKG_CONFIG])
  AC_CACHE_CHECK([lua library dir],
    AS_TR_SH([lua_cv_libdir_$1]),
    [# First, we need to figure out our own prefix
    MY_PREFIX=${prefix}
    if test "x${MY_PREFIX}" = "xNONE" ; then
      MY_PREFIX=${ac_default_prefix}
    fi
    # Then we ask lua where to install modules under that prefix
    eval AS_TR_SH([lua_cv_libdir_$1])=`$PKG_CONFIG $1 --define-variable=prefix=${MY_PREFIX} --variable=INSTALL_CMOD`
    ])

  eval VALUE="$AS_TR_SH([lua_cv_libdir_$1])"
  AS_IF([test x$VALUE != x],
    [$2],
    [m4_default([$3], [AC_MSG_ERROR([Could not find the lua CMOD path])])])
])
