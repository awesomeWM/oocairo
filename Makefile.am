ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = @DEPS_CFLAGS@

EXTRA_DIST = obj_context.c obj_font_face.c obj_font_opt.c obj_matrix.c obj_path.c obj_pattern.c obj_scaled_font.c obj_surface.c
EXTRA_DIST += COPYRIGHT Changes

lualibdir = $(LUALIBDIR)

lib_LTLIBRARIES = liboocairo.la
liboocairo_la_SOURCES = oocairo.c
liboocairo_la_LDFLAGS = -version 0:0:0 -no-undefined
liboocairo_la_LIBADD = @DEPS_LIBS@

include_HEADERS = oocairo.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = oocairo.pc

TESTS_ENVIRONMENT = echo 'lunit.main({...})' | LUA_PATH='@srcdir@/?.lua' LUA_CPATH='.libs/lib?.so' lua -e 'srcdir="@srcdir@"' -llunit -
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
EXTRA_DIST += examples/images/pattern.png
EXTRA_DIST += $(TESTS) lunit.lua test-setup.lua lunit-console.lua test-loading.lua

# Documentation
EXTRA_DIST += doc/lua-oocairo.pod doc/lua-oocairo-context.pod doc/lua-oocairo-fontface.pod doc/lua-oocairo-fontopt.pod
EXTRA_DIST += doc/lua-oocairo-matrix.pod doc/lua-oocairo-path.pod doc/lua-oocairo-userfont.pod
EXTRA_DIST += doc/lua-oocairo-pattern.pod doc/lua-oocairo-scaledfont.pod doc/lua-oocairo-surface.pod
manpages  = doc/lua-oocairo.3 doc/lua-oocairo-context.3 doc/lua-oocairo-fontface.3 doc/lua-oocairo-fontopt.3
manpages += doc/lua-oocairo-matrix.3 doc/lua-oocairo-path.3 doc/lua-oocairo-userfont.3
manpages += doc/lua-oocairo-pattern.3 doc/lua-oocairo-scaledfont.3 doc/lua-oocairo-surface.3
man_MANS = $(manpages)
MOSTLYCLEANFILES = $(manpages)

# All our manpages are under doc/, but for VPATH builds that dir might not exist yet
.pod.3:
	@$(MKDIR_P) doc/
	$(POD2MAN) --section=3 --center="Lua OO Cairo binding" --release="$(VERSION)" $< $@


# Make this library loadable by lua
install-exec-hook:
	$(MKDIR_P) $(DESTDIR)$(LUALIBDIR)
	-rm -f $(DESTDIR)$(LUALIBDIR)/oocairo.so
	$(LN_S) $(libdir)/liboocairo.so $(DESTDIR)$(LUALIBDIR)/oocairo.so
uninstall-local:
	-rm -f $(DESTDIR)$(LUALIBDIR)/oocairo.so

# Test whether loading oocairo works
installcheck-local:
	LUA_PATH='${srcdir}/?.lua' LUA_CPATH='${LUALIBDIR}/?.so' lua ${srcdir}/test-loading.lua
