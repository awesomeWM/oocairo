AC_INIT([oocairo], [1.4], [psychon@znc.in])
AC_CONFIG_SRCDIR([oocairo.c])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([config])
LT_INIT([disable-static])
AM_INIT_AUTOMAKE([-Wall -Werror foreign dist-bzip2])

WFLAGS="-Wall -Wextra -Wcast-align -Wmissing-declarations -Winit-self -Wundef"
WFLAGS="$WFLAGS -Wredundant-decls -Wwrite-strings -Wformat=2 -Wlogical-op"
WFLAGS="$WFLAGS -Wmissing-include-dirs -Wformat-nonliteral -Wformat-security"
WFLAGS="$WFLAGS -Wunused -Wmissing-format-attribute -Wstrict-aliasing -pedantic"

CC_CHECK_CFLAGS_APPEND([$WFLAGS])
CC_NOUNDEFINED
AC_PROG_CC_C99
LDFLAGS="$LDFLAGS $LDFLAGS_NOUNDEFINED"

# Lua gets different names in different distros :(
PKG_PROG_PKG_CONFIG
AC_MSG_CHECKING([for lua])
for lua_name in lua lua5.1
do
  PKG_CHECK_EXISTS([$lua_name], [LUA_NAME=$lua_name ; break])
done
AS_IF([test "x$LUA_NAME" = "x"],
      [AC_ERROR([Could not find lua via pkg-config!]) ])
AC_MSG_RESULT([$LUA_NAME])

AC_PROG_LN_S
AC_PATH_PROG([POD2MAN], [pod2man], [notfound])
PKG_CHECK_MODULES([DEPS], [$LUA_NAME cairo])
LUA_LIBDIR([$LUA_NAME], [AC_SUBST([LUALIBDIR], [$VALUE])])

AS_IF([test "x$POD2MAN" = "xnotfound"],
      [AC_ERROR([Could not find pod2man]) ])

AC_CONFIG_FILES([Makefile oocairo.pc oocairo-uninstalled.pc])
AC_OUTPUT
