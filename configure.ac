AC_PREREQ([2.69])
AC_INIT([futile], [0.0.1], [rob@marianski.com])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AM_INIT_AUTOMAKE([subdir-objects foreign])
AC_PROG_CC_C99

LT_INIT

# Checks for libraries.
AC_CHECK_LIB([m], [pow])

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_CHECK_FUNCS([floor pow sinh atan cos tan log])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

AC_CONFIG_MACRO_DIR([m4])
