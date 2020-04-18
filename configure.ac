#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([libcontrac], [0.01], [david@flypig.co.uk])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])
PKG_INSTALLDIR

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AM_PROG_AR
LT_INIT
AC_CHECK_PROG([DOXYGEN], [doxygen], [doxygen])

if test "$DOXYGEN" = doxygen; then
echo Use \"make doxygen\" to generate library documentation.
else
echo ===================================================================
echo Install doxygen to allow the library documentation to be generated.
echo ===================================================================
fi

# Checks for libraries.
PKG_CHECK_MODULES([LIBCONTRAC], [libcrypto])
PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdio.h stddef.h stdlib.h string.h, stdbool.h])

# Checks for compiler characteristics
AC_C_BIGENDIAN

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 libcontrac-0.pc
                 tests/Makefile
                 doxyfile])

AC_OUTPUT
