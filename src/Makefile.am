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



lib_LTLIBRARIES=libProperties4CXX.la

libProperties4CXX_la_SOURCES=scanner.ll parser.yy Properties.cpp Property.cpp
 
libProperties4CXX_la_LIBADD=$(PTHREAD_LIBS)

libProperties4CXX_la_CXXFLAGS=$(AM_CXXFLAGS) -DBUILDING_PROPERTIES4CXX=1 $(DLL_VISIBLE_CFLAGS)

libProperties4CXX_la_LDFLAGS=$(LD_NO_UNDEFINED_OPT)

AM_CXXFLAGS = -I$(top_srcdir)/include \
	$(PTHREAD_CFLAGS)

BUILT_SOURCES = parser.hh
AM_YFLAGS = -d

EXTRA_DIST = parserTypes.h lexer.h

