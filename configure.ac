EFL_VERSION([1], [0], [0], [release])
AC_INIT([esskyuehl], [efl_version], [michael.blumenkrantz@gmail.com], [esskyuehl])
AC_PREREQ([2.57])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AC_USE_SYSTEM_EXTENSIONS
AC_ISC_POSIX

AC_LIBTOOL_WIN32_DLL
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
LT_INIT([win32-dll disable-static pic-only])
EFL_INIT


AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_MAKE_SET
AC_FUNC_ALLOCA

m4_ifdef([v_mic],
   [
    EFL_COMPILER_FLAG([-Wshadow])
    EFL_COMPILER_FLAG([-Wall])
    EFL_COMPILER_FLAG([-W])
   ])

case "$host_os" in
   mingw32ce*)
      MODULE_ARCH="$host_os-$host_cpu"
      ;;
   *)
      MODULE_ARCH="$host_os-$host_cpu-v_maj.v_min.v_mic"
      ;;
esac
AC_SUBST(MODULE_ARCH)
AC_DEFINE_UNQUOTED(MODULE_ARCH, "${MODULE_ARCH}", "Module architecture")

case "$host_os" in
   mingw*)
      MODULE_EXT=".dll"
      ;;
   *)
      MODULE_EXT=".so"
      ;;
esac

AC_DEFINE_UNQUOTED(SHARED_LIB_SUFFIX, "${MODULE_EXT}", [Suffix for shared objects])

requirement_esskyuehl="eina ecore"
AC_SUBST(requirement_esskyuehl)

PKG_CHECK_MODULES([EFL], [$requirement_esskyuehl])

MYSQL_LIBS=""
AC_ARG_ENABLE([mysql],
   [AC_HELP_STRING([--disable-mysql], [disable mysql module @<:@default=detect@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       want_mysql="yes"
    else
       want_mysql="no"
    fi
   ],
   [want_mysql="yes"])

if test "x${want_mysql}" != "xno" ; then
   AC_MSG_CHECKING([for mysql])
   AC_PATH_PROG([mysql_config], [mysql_config])
   if test "x$mysql_config" != "x";then
         MYSQL_CFLAGS="$($mysql_config --cflags)"
         MYSQL_LIBS="$($mysql_config --libs)"
         cflags="$CFLAGS"
         cppflags="$CPPFLAGS"
         CFLAGS="$MYSQL_CFLAGS"
         CPPFLAGS="$CFLAGS"
         AC_CHECK_HEADERS([mysql/mysql.h mysql.h])
         CFLAGS="$cflags"
         CPPFLAGS="$cppflags"
         if test "x$ac_cv_header_mysql_h" != "xno" -o "x$ac_cv_header_mysql_mysql_h" != "xno" ; then
            AC_MSG_RESULT([yes])
            mysql=mysql
         else
            AC_MSG_RESULT([no])
         fi
   else
         AC_MSG_RESULT([no])
   fi
   AM_CONDITIONAL([MYSQL], [test -n "$MYSQL_LIBS"])
   AC_SUBST(MYSQL_CFLAGS)
   AC_SUBST(MYSQL_LIBS)
fi
AM_CONDITIONAL([MYSQL], [test -n "$MYSQL_LIBS"])
AC_SUBST(MYSQL_LIBS)

AC_ARG_ENABLE([postgresql],
   [AC_HELP_STRING([--disable-postgresql], [disable postgresql module @<:@default=detect@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       want_postgresql="yes"
    else
       want_postgresql="no"
    fi
   ],
   [want_postgresql="yes"])

POSTGRESQL_LIBS=""
if test "x${want_postgresql}" != "xno" ; then
   AC_MSG_CHECKING([for postgresql])
   AC_PATH_PROG([pg_config], [pg_config])
   if test "x$pg_config" != "x";then
         POSTGRESQL_CFLAGS="-I$($pg_config --includedir) -I$($pg_config --includedir-server)"
         POSTGRESQL_LIBS="-L$($pg_config --libdir) -lpq"
         cflags="$CFLAGS"
         CFLAGS="$POSTGRESQL_CFLAGS"
         CPPFLAGS="$CFLAGS"
         AC_CHECK_HEADERS([libpq-fe.h])
         CFLAGS="$cflags"
         CPPFLAGS="$cppflags"
         AC_MSG_RESULT([yes])
         postgresql=postgresql
   else
         AC_MSG_RESULT([no])
   fi
   AC_SUBST(POSTGRESQL_CFLAGS)
fi
AM_CONDITIONAL([POSTGRESQL], [test -n "$POSTGRESQL_LIBS"])
AC_SUBST(POSTGRESQL_LIBS)

AC_ARG_ENABLE([sqlite],
   [AC_HELP_STRING([--disable-sqlite], [disable sqlite module @<:@default=detect@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       want_sqlite="yes"
    else
       want_sqlite="no"
    fi
   ],
   [want_sqlite="yes"])

if test "x${want_sqlite}" != "xno" ; then
   PKG_CHECK_MODULES([SQLITE3], [sqlite3])
   test -n "$SQLITE3_LIBS" && sqlite=sqlite
fi
AM_CONDITIONAL([SQLITE], [test -n "$SQLITE3_LIBS"])


EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])

AC_CONFIG_FILES([
Makefile
rpm/${PACKAGE}.spec
])


AC_OUTPUT([
Makefile
esskyuehl.pc
])


#####################################################################
## Info

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "Documentation..........: ${build_doc}"
echo
echo "Backend support........: $mysql $postgresql $sqlite"
echo
echo "Compilation............: make (or gmake)"
echo "  CPPFLAGS.............: $CPPFLAGS"
echo "  CFLAGS...............: $CFLAGS"
echo "  LDFLAGS..............: $LDFLAGS"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo
