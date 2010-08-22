ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = @DEPS_CFLAGS@

EXTRA_DIST = obj_context.c obj_font_face.c obj_font_opt.c obj_matrix.c obj_path.c obj_pattern.c obj_scaled_font.c obj_surface.c

lualibdir = @lualibdir@

lib_LTLIBRARIES = liboocairo.la
liboocairo_la_SOURCES = oocairo.c
liboocairo_la_LDFLAGS = -version 0:0:0
liboocairo_la_LIBADD = @DEPS_LIBS@

include_HEADERS = oocairo.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = oocairo.pc

TESTS_ENVIRONMENT = echo 'lunit.main({...})' | LUA_PATH='@srcdir@/?.lua' LUA_CPATH='.libs/lib?.so' lua -llunit -
TESTS  = test/context.lua
TESTS += test/font_face.lua
TESTS += test/font_opt.lua
TESTS += test/general.lua
TESTS += test/matrix.lua
TESTS += test/path.lua
TESTS += test/pattern.lua
TESTS += test/pdf_surface.lua
TESTS += test/ps_surface.lua
TESTS += test/scaled_font.lua
TESTS += test/surface.lua
TESTS += test/svg_surface.lua
EXTRA_DIST += $(TESTS) lunit.lua test-setup.lua lunit-console.lua test-loading.lua

# Make this library loadable by lua
install-exec-hook:
	$(MKDIR_P) $(DESTDIR)$(lualibdir)
	-rm -f $(DESTDIR)$(lualibdir)/oocairo.so
	$(LN_S) $(libdir)/liboocairo.so $(DESTDIR)$(lualibdir)/oocairo.so

# Test whether loading oocairo works
installcheck-local:
	LUA_PATH='${srcdir}/?.lua' LUA_CPATH='@lualibdir@/?.so' lua ${srcdir}/test-loading.lua
