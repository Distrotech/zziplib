# -----------------------------------------------------------------------
AC_INIT([libgz.spec])
AC_PREREQ(2.49)
AC_COPYRIGHT([Guido Draheim <guido@gmx.de> for ZZipLib.SF.net])
AC_REVISION($Revision: 1.15 $)
AC_CONFIG_AUX_DIR([mak])
AC_CONFIG_MACRO_DIR([m4])
# =======================================================================
AC_CANONICAL_SYSTEM
AC_SET_DEFAULT_PATHS_SYSTEM
AX_SPEC_DEFAULTS
AM_INIT_AUTOMAKE($PACKAGE,$VERSION)
AC_PROG_CC
AC_PROG_INSTALL
AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL
AX_SET_VERSION_INFO
AC_CONFIG_HEADERS([inc/_config.h])
AX_PREFIX_CONFIG_H([inc/gzconfig.h],[gz],[inc/_config.h])
AM_MAINTAINER_MODE
AC_ARG_WITH(32bit, [compile 32bit objectcode on a 64bit machine],
                   [ test .$GCC = ".yes" && CFLAGS="$CFLAGS -m32" ])
# ----------------------------------------------------------------
AC_SYS_LARGEFILE
AC_C_INLINE
AC_C_CONST
AC_STDC_HEADERS
AC_HEADER_DIRENT
AC_CHECK_HEADERS(zlib.h)
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_CHECK_TYPE(ssize_t, int)
AC_CHECK_TYPE(off64_t, off_t)
dnl and show the relations...
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(off_t)
AC_CHECK_SIZEOF(off64_t)
# ----------------------------------------------------------------
AC_MSG_CHECKING(zlib developement files installed)
cat >conftest.c <<EOF
#include <zlib.h>
const char* zlib_VeRsIoN = ZLIB_VERSION "dEtEcT";
EOF
ZLIB_VERSION=`(eval "$ac_cpp conftest.c") 2>/dev/null \
| grep zlib_VeRsIoN | sed -e 's,.*= *.,,' -e 's,. *.dEtEcT.*,,'`
rm conftest.c
AC_SUBST(ZLIB_VERSION)
AC_MSG_RESULT(yes - $ZLIB_VERSION)
# ----------------------------------------------------------------
AX_MAINTAINER_MODE_AUTO_SILENT
AX_CFLAGS_WARN_ALL
AX_CFLAGS_GCC_OPTION(-Wpointer-arith)
AX_CFLAGS_GCC_OPTION(-Wsign-compare)
AX_CFLAGS_GCC_OPTION(-Wmissing-declarations)
AX_CFLAGS_GCC_OPTION(-Wdeclaration-after-statement)
AX_CFLAGS_GCC_OPTION(-Werror-implicit-function-declaration)
AX_CFLAGS_GCC_OPTION(-Wstrict-aliasing)
AX_CFLAGS_GCC_OPTION(-Warray-bounds)
AX_CFLAGS_NO_WRITABLE_STRINGS
AX_CFLAGS_STRICT_PROTOTYPES
# ---------------------------------------------------------------
AC_OUTPUT([Makefile inc/Makefile lib/Makefile bin/Makefile]) 
echo '-----' $PACKAGE $VERSION $prefix
$SHELL libtool --features || true
echo "CPPFLAGS=$CPPFLAGS"
echo "CFLAGS=$CFLAGS"
echo '# make && make check && make install'
