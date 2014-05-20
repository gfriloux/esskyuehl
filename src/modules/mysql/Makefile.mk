EXTRA_DIST += \
src/modules/mysql/esql_mysql_backend.c

if MYSQL
mod_LTLIBRARIES += src/modules/mysql/mysql.la

src_modules_mysql_mysql_la_SOURCES = \
src/modules/mysql/esql_mysql_backend.c

src_modules_mysql_mysql_la_CFLAGS = \
$(MOD_CFLAGS) \
@MYSQL_CFLAGS@ \
-Isrc/modules/mysql/mysac

src_modules_mysql_mysql_la_LIBADD = \
$(MOD_LIBS) \
@MYSQL_LIBS@ \
src/modules/mysql/mysac/libmysac.la

src_modules_mysql_mysql_la_LDFLAGS = $(LD_FLAGS)
src_modules_mysql_mysql_la_LIBTOOLFLAGS = $(LT_FLAGS)
endif
include src/modules/mysql/mysac/Makefile.mk
