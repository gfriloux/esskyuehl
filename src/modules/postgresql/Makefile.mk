EXTRA_DIST += src/modules/postgresql/esql_postgresql_backend.c

if POSTGRESQL
mod_LTLIBRARIES += src/modules/postgresql/psql.la

src_modules_postgresql_psql_la_SOURCES = \
src/modules/postgresql/esql_postgresql_backend.c

src_modules_postgresql_psql_la_CFLAGS = \
$(MOD_CFLAGS) \
@POSTGRESQL_CFLAGS@

src_modules_postgresql_psql_la_LIBADD = \
$(MOD_LIBS) \
@POSTGRESQL_LIBS@

src_modules_postgresql_psql_la_LDFLAGS = $(LD_FLAGS)
src_modules_postgresql_psql_la_LIBTOOLFLAGS = $(LT_FLAGS)
endif
