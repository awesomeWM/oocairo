ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = @DEPS_CFLAGS@

EXTRA_DIST = obj_context.c obj_font_face.c obj_font_opt.c obj_matrix.c obj_path.c obj_pattern.c obj_scaled_font.c obj_surface.c

lualibdir = @lualibdir@

lualib_LTLIBRARIES = liboocairo.la
liboocairo_la_SOURCES = oocairo.c
liboocairo_la_LDFLAGS = -version 0:0:0
liboocairo_la_LIBADD = @DEPS_LIBS@

include_HEADERS = oocairo.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = oocairo.pc
