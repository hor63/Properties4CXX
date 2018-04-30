/*
 * parserTypes.h
 *
 *  Created on: Apr 29, 2018
 *      Author: hor
 *
 *   This file is part of Properties4CXX, a Java-inspired properties reader
 *   Copyright (C) 2018  Kai Horstmann
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */


#ifndef SRC_PARSERTYPES_H_
#define SRC_PARSERTYPES_H_


/***************************************************************************/
/* Structures to store non-string values together with the original string */

typedef struct {
	bool isQuotedString;
	char *str;
	} tStrVal;

typedef struct {
	long double numVal;
	char *numStr;
	} tNumVal;

typedef struct {
	long long intVal;
	char *intStr;
	} tIntVal;

typedef struct {
	bool  boolVal;
	char *boolStr;
	} tBoolVal;



#endif /* SRC_PARSERTYPES_H_ */
