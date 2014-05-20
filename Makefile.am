ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = subdir-objects

MAINTAINERCLEANFILES = \
ar-lib \
Makefile.in \
aclocal.m4 \
compile \
config.guess \
config.h.in \
config.h.in~ \
config.sub \
configure \
depcomp \
install-sh \
ltconfig \
ltmain.sh \
missing \
esskyuehl*doc*tar* \
m4/l*

CLEANFILES = \
$(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.gz \
$(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.bz2 \
$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-doc.tar.bz2

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = esskyuehl.pc

EXTRA_DIST = \
AUTHORS \
COPYING \
README \
$(pkgconfig_DATA) \
autogen.sh \
esskyuehl.pc.in \
m4/efl.m4 \
m4/efl_compiler_flag.m4 \
m4/efl_doxygen.m4

moddir = $(libdir)/esskyuehl/$(MODULE_ARCH)
mod_LTLIBRARIES =

MOD_CFLAGS = \
-I$(top_builddir) \
@EFL_CFLAGS@ \
-I$(top_srcdir)/src/lib

MOD_LIBS = \
$(top_builddir)/src/lib/libesskyuehl.la \
@EFL_LIBS@ 

LD_FLAGS = -no-undefined -module -avoid-version
LT_FLAGS = --tag=disable-static

.PHONY: doc

# Documentation

include doc/Makefile.mk
include src/lib/Makefile.mk
include src/modules/mysql/Makefile.mk
include src/modules/postgresql/Makefile.mk
include src/modules/sqlite/Makefile.mk
include src/tests/Makefile.mk
