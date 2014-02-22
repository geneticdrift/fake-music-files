AC_PREREQ([2.68])
AC_INIT([fmf], [1.0.1], [gd@iotide.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

m4_include([m4/ax_cxx_compile_stdcxx_11.m4])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

AC_DEFINE_UNQUOTED([LICENSE_YEAR], m4_esyscmd([echo -n \"`date +"%Y"`\"]), [License copyright year])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL

# Check for languages
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])

AX_PTHREAD(,AC_MSG_ERROR([pthread required]))

# Config for dependencies
PKG_CHECK_MODULES([DEPS], [taglib >= 1.7.0])

# Get uchardet library and include locations
AC_ARG_WITH([uchardet-include-path],
  [AS_HELP_STRING([--with-uchardet-include-path],
    [location of the uchardet headers, defaults to /usr/include/uchardet])],
  [UCHARDET_CFLAGS="-I$withval"],
  [UCHARDET_CFLAGS='-I/usr/include/uchardet'])
AC_SUBST([UCHARDET_CFLAGS])

AC_ARG_WITH([uchardet-lib-path],
  [AS_HELP_STRING([--with-uchardet-lib-path], [location of the uchardet libraries])],
  [UCHARDET_LIBS="-L$withval -luchardet"],
  [UCHARDET_LIBS='-luchardet'])
AC_SUBST([UCHARDET_LIBS])

# Checks for header files.
AC_CHECK_HEADERS([limits.h stddef.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([mkdir realpath strcasecmp strchr strerror])

# Configure options: --enable-debug[=no].
AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug],
    [enable debug code (default is no)])],
  [debug="$enableval"], [debug=no])

CC="${PTHREAD_CC}"

if test "x${debug}" = xyes; then
  CXXFLAGS="${CXXFLAGS} -O0 -g3 -gdwarf-3"
fi

CXXFLAGS+=" -Wall -Wextra"

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo "
  ($PACKAGE_NAME) version $PACKAGE_VERSION
  Prefix.........: $prefix
  Debug Build....: $debug
  C++ Compiler...: $CXX $CXXFLAGS $CPPFLAGS $PTHREAD_CFLAGS
  Linker.........: $LD $LDFLAGS $LIBS $PTHREAD_LIBS $PTHREAD_CFLAGS
"
