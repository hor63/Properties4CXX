#    This file is part of Properties4CXX, an electronic variometer for glider planes
#    Copyright (C) 2016  Kai Horstmann
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

liblibProperties4CXX_la_SOURCES=scanner.l parser.y Properties.cpp
 
liblibProperties4CXX_la_LIBADD=$(LOG4CXX_LIBS) $(PTHREAD_LIBS)

liblibProperties4CXX_la_CXXFLAGS=$(AM_CXXFLAGS) -DBUILDING_LIBPRPERTIESPP=1 $(DLL_VISIBLE_CFLAGS)

liblibProperties4CXX_la_LDFLAGS=$(LD_NO_UNDEFINED_OPT) $(LOG4CXX_LDFLAGS)

AM_CXXFLAGS = -I$(top_srcdir)/src $(LOG4CXX_CXXFLAGS) \
	$(PTHREAD_CFLAGS)

BUILT_SOURCES = parser.h
AM_YFLAGS = -d

