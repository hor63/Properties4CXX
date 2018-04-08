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

long long strToLL (char const *str){
	long long rc = 0ll;
	bool negative = false;

	if (*str == '-') {
		negative = true;
		str++;
	} else {
		if (*str == '+') {
			str++;
		}
	}

	while (*str) {
		rc = rc * 10ll + (*str - '0');
		str++;
	}

	if (negative) {
		rc *= -1ll;
	}

	return rc;
}


long long strOctToLL (char const *str){
	long long rc = 0ll;

	while (*str) {
		rc = rc * 8ll + (*str - '0');
		str++;
	}

	return rc;
}

long long strBinToLL (char const *str){
	long long rc = 0ll;

	// skip over the "0b"
	str ++;
	str ++;

	while (*str) {
		rc = rc * 2ll + (*str - '0');
		str++;
	}

	return rc;
}

long long strHexToLL (char const *str){
	long long rc = 0ll;
	long long digit;

	// skip over the "0x"
	str ++;
	str ++;

	while (*str) {
		if (*str >= '0' && *str <= '9') {
			digit = *str - '0';
		} else {
			if (*str >= 'a' && *str <= 'f') {
				digit = *str - 'a' + 10ll;
			} else {
				digit = *str - 'A' + 10ll;
			}
		}
		rc = rc * 16ll + digit;
		str++;
	}

	return rc;
}

long double strToLD (char const *str){
	long double rc = 0.0l;
	long double fractFactor = 1.0l;
	bool negative = false;
	int exp = 0;
	long double expFactor = 1.0l;
	bool expNegative = false;
	int i;

	// The significant
	if (*str == '-') {
		negative = true;
		str++;
	} else {
		if (*str == '+') {
			str++;
		}
	}

	while (*str >= '0' && *str <= '9') {
		rc = rc * 10.0l + (long double)(*str - '0');
	}

	// now the decimals
	if (*str == '.') {
		str ++;

		while (*str >= '0' && *str <= '9') {
			// I am multiplying because multiply by 10 can be expressed precisely in binary form.
			// Dividing by 10 howewer is not precisely represented in binary form by IEEE long doubles.
			// Repeatedly dividing the factor by 10 accumulates inaccuracies.
			// Dividing by the accumulated factor once minimizes the inaccuracy.
			fractFactor *= 10.0l;
			rc += (long double)(*str - '0') / fractFactor;
		}
	}

	// Now the exponent
	if (*str == 'e' || *str == 'E') {
		if (*str == '-') {
			expNegative = true;
			str++;
		} else {
			if (*str == '+') {
				str++;
			}
		}
		while (*str >= '0' && *str <= '9') {
			exp = exp * 10 + (*str - '0');
		}

		// Calculate the exp factor by calculating the power of 10 brute force :)
		for (i=0; i < exp; i++) {
			expFactor *= 10.0l;
		}

		if (expNegative)  {
			expFactor = 1.0f / expFactor;
		}

		rc *= expFactor;
	}

	if (negative) {
		rc = -rc;
	}

	return rc;
}

} // namespace Properties4CXX {
