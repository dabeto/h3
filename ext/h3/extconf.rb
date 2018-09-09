require "mkmf"

HEADER_DIRS = [
  ".",
  "./h3lib/include"
]
LIB_DIRS = [
  ".",
  "./h3lib/lib"
]

dir_config("h3", HEADER_DIRS, LIB_DIRS)

have_header("h3api.h")or raise
have_library('h3') or raise

create_makefile('h3/h3')
