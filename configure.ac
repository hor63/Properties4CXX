#    This file is part of Properties4CXX, a Java-inspired properties reader
#    Copyright (C) 2018  Kai Horstmann
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Properties4CXX],[1.1.5],[https://github.com/hor63/Properties4CXX/issues],[Properties4CXX],https://github.com/hor63/Properties4CXX)
AC_CONFIG_HEADERS(config.h)

AC_CONFIG_MACRO_DIRS([m4])


AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE()
AM_DEFAULT_VERBOSITY=0


AC_PROG_YACC
AC_PROG_LEX
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_17(ext, mandatory)
AX_PTHREAD()

# get a suitable command like mkdir -p on LINUX systems use install if mkdir -p does not work.
AC_PROG_INSTALL
AC_PROG_MKDIR_P

# commands for the archiver
AM_PROG_AR
# obsoleted by LT_INIT
# AC_PROG_RANLIB

LT_INIT(win32-dll dlopen )
AC_SUBST([LIBTOOL_DEPS])

AC_LANG([C++])

AM_CONDITIONAL(OS_IS_CYGWIN, test "$host_os" = cygwin)

AX_CHECK_COMPILE_FLAG([-fvisibility=internal],[DLL_VISIBLE_CFLAGS="-fvisibility=internal"])
AC_SUBST([DLL_VISIBLE_CFLAGS])

AX_CHECK_LINK_FLAG([-Wl,--no-undefined], [LD_NO_UNDEFINED_OPT=-no-undefined])
AC_SUBST([LD_NO_UNDEFINED_OPT])

DX_DOXYGEN_FEATURE(ON)
DX_DOT_FEATURE(ON)
DX_HTML_FEATURE(ON)
DX_PDF_FEATURE(ON)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN(Properties4CXX, $(srcdir)/doxygen.cfg, doc/doxygen)

AC_CONFIG_FILES(Makefile
                src/Makefile
                test/Makefile
		include/Makefile)

AC_OUTPUT

# create the doc/doxygen directory right away. If it does not exist the doxygen rules will not work :(
AS_ECHO(["configure: creating directory doc/doxygen"]) 
AS_MKDIR_P(doc/doxygen)
