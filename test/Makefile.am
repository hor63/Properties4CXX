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

SUBDIRS=../src

noinst_PROGRAMS = testProperties$(EXEEXT)
testProperties_SOURCES = PropertiesTest.cpp

testProperties_LDFLAGS = $(AM_LDFLAGS) -L../src

testProperties_LDADD = -lProperties4CXX \
	$(LOG4CXX_LIBS) \
	$(PTHREAD_LIBS)

AM_LDFLAGS = $(LOG4CXX_LDFLAGS)

AM_CXXFLAGS = -I$(top_srcdir)/include \
	$(PTHREAD_CFLAGS)
