package = "oocairo"
version = "scm-0"
local versionNumber = version:gsub("^(.*)-.-$", "%1")
source = {
  url = "https://github.com/awesomeWM/oocairo/archive/master.zip",
  dir = "oocairo-master",
}
description = {
  summary = "Cairo bindings for lua ",
  homepage = "https://github.com/awesomeWM/oocairo",
  license = "MIT/X11",
  detailed = [[
	Lua-OOCairo provides Lua with full access to the Cairo vector graphics API. 
	It can be used for drawing onto bitmap images and saving the resulting files
	in PNG format, or for generating files in vector formats like SVG and PDF. 
  ]],
}
dependencies = {
  "lua >= 5.1, < 5.4",
}
external_dependencies = {
  CAIRO = {
    header = "cairo/cairo.h",
    library = "cairo",
  }
}

build = {
  type = "builtin",
  modules = {
    oocairo = {
      defines = { 
      	"VERSION=\""..versionNumber.."\"" 
      },
      sources = { 
          "oocairo.c",
      },
      libraries = {
      	"cairo"
      },
      incdirs = {
        "$(CAIRO_INCDIR)/cairo"
      },
      libdirs = {
      	"$(CAIRO_LIBDIR)"
      },
    },
  }
}
