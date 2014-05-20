EXTRA_DIST += src/modules/sqlite/esql_sqlite_backend.c

if SQLITE
mod_LTLIBRARIES += src/modules/sqlite/sqlite.la

src_modules_sqlite_sqlite_la_SOURCES = \
src/modules/sqlite/esql_sqlite_backend.c

src_modules_sqlite_sqlite_la_CFLAGS = \
$(MOD_CFLAGS) \
@SQLITE3_CFLAGS@

src_modules_sqlite_sqlite_la_LIBADD = \
$(MOD_LIBS) \
@SQLITE3_LIBS@

src_modules_sqlite_sqlite_la_LDFLAGS = $(LD_FLAGS)
src_modules_sqlite_sqlite_la_LIBTOOLFLAGS = $(LT_FLAGS)
endif
