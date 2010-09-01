AC_INIT([oocairo], [1.3], [psychon@znc.in])
AC_CONFIG_SRCDIR([oocairo.c])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([config])
LT_INIT([disable-static])
AM_INIT_AUTOMAKE([-Wall -Werror foreign dist-bzip2])

WFLAGS="-Wall -Wextra -Wcast-align -Wmissing-declarations -Winit-self -Wundef"
WFLAGS="$WFLAGS -Wredundant-decls -Wwrite-strings -Wformat=2 -Wlogical-op"
WFLAGS="$WFLAGS -Wmissing-include-dirs -Wformat-nonliteral -Wformat-security"
WFLAGS="$WFLAGS -Wmissing-format-attribute -Wstrict-aliasing -pedantic"

CC_CHECK_CFLAGS_APPEND([$WFLAGS])
CC_NOUNDEFINED
AC_PROG_CC_C99
LDFLAGS="$LDFLAGS $LDFLAGS_NOUNDEFINED"

AC_PROG_LN_S
AC_PATH_PROG([POD2MAN], [pod2man])
PKG_CHECK_MODULES(DEPS, lua5.1 cairo)
LUA_LIBDIR([lua5.1], [AC_SUBST([LUALIBDIR], [$VALUE])])

AC_CONFIG_FILES([Makefile oocairo.pc oocairo-uninstalled.pc])
AC_OUTPUT
