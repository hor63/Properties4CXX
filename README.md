# Properties4CXX
Property file reader for C++
Inspiration is the Java class properties

Properties4CXX allows key-value pairs as Java Properties does.
You can retrieve values of a key as string.

Extensions are:

- Subclassed property classes for long long int, long double, bool properties.
- Stronlgy typed value extractors for scalar values for long long int, bool, and long double values. Extracting the value as string is always possible.
- Lists of string values under one property values are separated by commas
- Hierarchical sub-sections as named blocks of property lists enclosed by { }

Special characters can be escaped in quoted strings.


    This file is part of Property ++
    Copyright (C) 2018  Kai Horstmann

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

