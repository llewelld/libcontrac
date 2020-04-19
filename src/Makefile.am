lib_LIBRARIES = ../libcontrac.a
pkginclude_HEADERS = ../include/contrac/*.h

___libcontrac_a_SOURCES = contrac.c rpi.c log.c utils.c dtk.c rpi_list.c dtk_list.c match.c
___libcontrac_a_CFLAGS = -std=gnu99 -fPIC -Wall -Werror -I"../include" @LIBCONTRAC_CFLAGS@
ARFLAGS = cr
AR_FLAGS = cr

lib_LTLIBRARIES = ../libcontrac.la
___libcontrac_la_SOURCES = $(___libcontrac_a_SOURCES)
___libcontrac_la_CFLAGS = $(___libcontrac_a_CFLAGS) @LIBCONTRAC_CFLAGS@
___libcontrac_la_LDFLAGS = = -version-info 1:0:0 @LIBCONTRAC_LIBS@

