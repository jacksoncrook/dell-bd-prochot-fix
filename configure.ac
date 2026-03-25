AC_PREREQ([2.69])
AC_INIT([dell-bd-prochot-fix], [0.1.0], [https://github.com/jacksoncrook/dell-bd-prochot-fix])
AM_INIT_AUTOMAKE([1.14 -Wall -Werror -Wno-extra-portability foreign no-dist-gzip dist-zip])
AM_SILENT_RULES([yes])
LT_INIT([disable-static])
AC_CONFIG_SRCDIR([rdmsr.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for header files.
AC_CHECK_HEADERS([ctype.h])
AC_CHECK_HEADERS([dirent.h])
AC_CHECK_HEADERS([errno.h])
AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADERS([getopt.h])
AC_CHECK_HEADERS([inttypes.h])
AC_CHECK_HEADERS([stdbool.h])
AC_CHECK_HEADERS([stdint.h])
AC_CHECK_HEADERS([stdio.h])
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADERS([sys/types.h])
AC_CHECK_HEADERS([unistd.h])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

AC_MSG_RESULT([
        dell-bd-prochot-fix $VERSION
        ========

        prefix:                 ${prefix}
        bindir:                 ${bindir}

        compiler:               ${CC}
        cflags:                 ${CFLAGS}
        ldflags:                ${LDFLAGS}
])
