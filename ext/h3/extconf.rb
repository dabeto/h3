require "mkmf"


have_header("h3api.h")or raise
#have_library('h3') or raise

create_makefile('h3/h3')
