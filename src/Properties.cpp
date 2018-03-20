/*
 * parser.yy
 *
 *  Created on: Mar 28, 2018
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif


#include "Properties4CXX/Properties.h"


namespace Properties4CXX {

PROPERTIES4CXX_PUBLIC
Properties::Properties () {

}

PROPERTIES4CXX_PUBLIC
Properties::Properties (char const *configFileName) {

}

PROPERTIES4CXX_PUBLIC
Properties::Properties (std::string const &configFileName) {

}

PROPERTIES4CXX_PUBLIC
Properties::Properties (std::istream *inputStream) {

}

PROPERTIES4CXX_PUBLIC
Properties::~Properties() {

}

PROPERTIES4CXX_PUBLIC
void Properties::setFileName (char const *configFileName) {

}

PROPERTIES4CXX_PUBLIC
void Properties::setFileName (std::string const configFileName) {

}

PROPERTIES4CXX_PUBLIC
void Properties::setInputStream (std::istream *inputStream){

}

PROPERTIES4CXX_PUBLIC
void Properties::readConfiguration() {

}


} // namespace Properties4CXX {
