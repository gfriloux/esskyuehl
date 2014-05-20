MYSAC_SRC = \
src/modules/mysql/mysac/mysac.c \
src/modules/mysql/mysac/mysac_encode_values.c \
src/modules/mysql/mysac/mysac_query.c \
src/modules/mysql/mysac/mysac_connect.c \
src/modules/mysql/mysac/mysac_encode_values.h \
src/modules/mysql/mysac/mysac_res.c \
src/modules/mysql/mysac/mysac_decode_field.c \
src/modules/mysql/mysac/mysac_errors.c \
src/modules/mysql/mysac/mysac_stmt.c \
src/modules/mysql/mysac/mysac_decode_field.h \
src/modules/mysql/mysac/mysac.h \
src/modules/mysql/mysac/mysac_usedatabase.c \
src/modules/mysql/mysac/mysac_decode_respbloc.c \
src/modules/mysql/mysac/mysac_memory.c \
src/modules/mysql/mysac/mysac_utils.c \
src/modules/mysql/mysac/mysac_decode_respbloc.h \
src/modules/mysql/mysac/mysac_memory.h \
src/modules/mysql/mysac/mysac_utils.h \
src/modules/mysql/mysac/mysac_decode_row.c \
src/modules/mysql/mysac/mysac_net.c \
src/modules/mysql/mysac/mysac_decode_row.h \
src/modules/mysql/mysac/mysac_net.h

EXTRA_DIST += $(MYSAC_SRC)

if MYSQL
noinst_LTLIBRARIES = src/modules/mysql/mysac/libmysac.la

src_modules_mysql_mysac_libmysac_la_LIBADD = @MYSQL_LIBS@
src_modules_mysql_mysac_libmysac_la_CFLAGS = \
-I@top_srcdir@ \
@MYSQL_CFLAGS@

src_modules_mysql_mysac_libmysac_la_SOURCES = \
$(MYSAC_SRC)
endif
