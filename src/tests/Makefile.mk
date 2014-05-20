check_PROGRAMS = \
src/tests/basic_query \
src/tests/basic_pool


if SQLITE
check_PROGRAMS += src/tests/test_sqlite
endif

src_tests_basic_query_SOURCES = src/tests/basic_query.c
src_tests_basic_query_CFLAGS = $(MOD_CFLAGS)
src_tests_basic_query_LDADD = $(MOD_LIBS)
src_tests_basic_pool_SOURCES = src/tests/basic_pool.c
src_tests_basic_pool_CFLAGS = $(MOD_CFLAGS)
src_tests_basic_pool_LDADD = $(MOD_LIBS)
src_tests_test_sqlite_SOURCES = src/tests/test_sqlite.c
src_tests_test_sqlite_CFLAGS = $(MOD_CFLAGS)
src_tests_test_sqlite_LDADD = $(MOD_LIBS)
