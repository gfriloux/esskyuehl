pkginclude_HEADERS = src/lib/Esskyuehl.h
pkgincludedir = @includedir@/esskyuehl-@VMAJ@

lib_LTLIBRARIES = src/lib/libesskyuehl.la

src_lib_libesskyuehl_la_CFLAGS = \
$(MOD_CFLAGS) \
-DESQL_MODULE_PATH=\"$(libdir)/esskyuehl/$(MODULE_ARCH)\"

src_lib_libesskyuehl_la_LIBADD = \
@EFL_LIBS@

src_lib_libesskyuehl_la_LDFLAGS = @EFL_LTLIBRARY_FLAGS@

src_lib_libesskyuehl_la_SOURCES = \
src/lib/esql_private.h \
src/lib/esql.c \
src/lib/esql_alloc.c \
src/lib/esql_connect.c \
src/lib/esql_convert.c \
src/lib/esql_events.c \
src/lib/esql_module.c \
src/lib/esql_module.h \
src/lib/esql_pool.c \
src/lib/esql_query.c \
src/lib/esql_res.c
