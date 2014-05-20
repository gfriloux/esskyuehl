pkginclude_HEADERS = Esskyuehl.h
pkgincludedir = @includedir@/esskyuehl-@VMAJ@

AM_CFLAGS = \
  @EFL_CFLAGS@ \
  -DESQL_MODULE_PATH=\"$(libdir)/esskyuehl/$(MODULE_ARCH)\" \
  -I@top_srcdir@


lib_LTLIBRARIES = libesskyuehl.la

libesskyuehl_la_LIBADD = \
  @EFL_LIBS@

libesskyuehl_la_LDFLAGS = @EFL_LTLIBRARY_FLAGS@

libesskyuehl_la_SOURCES = \
esql_private.h \
esql.c \
esql_alloc.c \
esql_connect.c \
esql_convert.c \
esql_events.c \
esql_module.c \
esql_pool.c \
esql_query.c \
esql_res.c

MAINTAINERCLEANFILES = Makefile.in
