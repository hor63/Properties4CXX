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


#include "parserTypes.h"
#include "Properties4CXX/Properties.h"
#include "Properties4CXX/Property.h"

#include "parser.hh"
#include "lexer.h"


namespace Properties4CXX {

ExceptionBase::~ExceptionBase () {}

const char* ExceptionBase::what() const noexcept {
	return description.c_str();
}

ExceptionConfigReadError::~ExceptionConfigReadError () {}

ExceptionConfigFileOpenError::~ExceptionConfigFileOpenError () {}

ExceptionPropertyNotFound::~ExceptionPropertyNotFound () {}

ExceptionPropertyDuplicate::~ExceptionPropertyDuplicate () {}

Properties::Properties ()
:configFileManagedInternally{false},
 inputStream{0}
{ }

Properties::Properties (char const *configFileName)
:configFileName{configFileName},
 configFileManagedInternally{true},
 inputStream{0}
{ }

Properties::Properties (std::string const &configFileName)
:configFileName{configFileName},
 configFileManagedInternally{true},
 inputStream{0}
{ }

Properties::Properties (std::istream *iStream)
:configFileManagedInternally{false},
 inputStream{iStream}
{ }

Properties::~Properties() {

}

void Properties::setFileName (char const *configName) {

	inputStream = 0;
	configFileManagedInternally = true;
	configFileName = configName;

}

void Properties::setFileName (std::string const configName) {

	inputStream = 0;
	configFileManagedInternally = true;
	configFileName = configName;

}

void Properties::setInputStream (std::istream *iStream){

	if (inputFileStream.is_open()) {
		inputFileStream.close();
	}
	inputStream = iStream;
	configFileManagedInternally = false;

}

void Properties::setStructLevel (int structLevel) {

	this->structLevel = structLevel;

	for (PropertyIterator it=propertyMap.begin(); it != propertyMap.end(); it++) {
		it->second.get()->setStructLevel(structLevel);
	}
}

void Properties::readConfiguration() {

	// Clear the properties list
	propertyMap.clear();

	// the Flex scanner context
	void *scanner = 0;

	if (configFileManagedInternally) {
		inputFileStream.exceptions(inputFileStream.badbit );

		if (!inputFileStream.is_open()) {
			try {
				inputFileStream.open(configFileName.c_str(),inputFileStream.in);
				if (inputFileStream.fail()) {
				std::string errStr ("Cannot open configuration file \"");
				errStr.append(configFileName).append("\".");
				throw ExceptionConfigFileOpenError(errStr.c_str());
				}
			} catch (std::istream::failure const &e) {
				throw ExceptionConfigFileOpenError(e.what());
			}
		}
	} else {
		if (!inputStream) {
			throw ExceptionConfigReadError ("The external input stream is NULL.");
		} else {
			if (!inputStream->bad()) {
				throw ExceptionConfigReadError ("The external input stream in BAD state.");
			}
		}
	}

	yylex_init_extra(this,&scanner);
	YY_BUFFER_STATE buf =  yy_create_buffer ( 0, YY_BUF_SIZE ,scanner);
	yy_flush_buffer(buf,scanner);
	yy_switch_to_buffer(buf,scanner);

	// yydebug = 1;

	yyparse(scanner,this);

	if (configFileManagedInternally && inputFileStream.is_open()) {
		inputFileStream.close();
	}


}

Property const *Properties::searchProperty (std::string const &propertyName) const {

	PropertyCIterator it = propertyMap.find(propertyName);
	if (it == propertyMap.cend()) {
		std::string errText = "Cannot find property ";
		errText.append(propertyName);
		throw ExceptionPropertyNotFound(errText.c_str());
	}

	return it->second.get();

}

bool Properties::getPropertyValue(std::string const& propertyName, bool defaultVal) const {

	try {
		Property const * prop = searchProperty(propertyName);

		return prop->getBoolValue();

	} catch (ExceptionPropertyNotFound const &e) {
		return defaultVal;
	}
}

double Properties::getPropertyValue(std::string const& propertyName,double defaultVal) const {

	try {
		Property const * prop = searchProperty(propertyName);

		return prop->getDoubleValue();

	} catch (ExceptionPropertyNotFound const &e) {
		return defaultVal;
	}
}

long long Properties::getPropertyValue(std::string const& propertyName,long long defaultVal) const {

	try {
		Property const * prop = searchProperty(propertyName);

		return prop->getIntVal();

	} catch (ExceptionPropertyNotFound const &e) {
		return defaultVal;
	}
}


char const * Properties::getPropertyValue(std::string const& propertyName,char const * defaultVal) const {

	try {
		Property const * prop = searchProperty(propertyName);

		if (prop->isStruct()) {
			std::ostringstream strstr;
			strstr << "Property " << propertyName << " is not a scalar value but a struct.";

			throw ExceptionWrongPropertyType(strstr.str());
		}

		return prop->getStrValue();

	} catch (ExceptionPropertyNotFound const &e) {
		return defaultVal;
	}

}

void Properties::addProperty (Property *newProperty) {

	PropertyCIterator it = propertyMap.find(newProperty->getPropertyName());

	if (it != propertyMap.cend()) {
		std::string errText = "Property already exists: ";
		errText.append(newProperty->getPropertyName());
		throw ExceptionPropertyDuplicate(errText.c_str());
	}

	newProperty->setStructLevel(structLevel);
	propertyMap.insert (PropertyPair(newProperty->getPropertyName(),PropertyPtr(newProperty)));

}

void Properties::deletePropery (std::string const &propertyName) {

	PropertyIterator it = propertyMap.find(propertyName);

	if (it != propertyMap.end()) {
		// It exists, therefore delete it!
		propertyMap.erase(it);
	}

}

std::ostream &Properties::writeOut (std::ostream &os) const {

	auto it = propertyMap.cbegin();

	while (it != propertyMap.cend()) {
		it->second->writeOut(os);
		it++;
	}

	os << std::endl;

	return os;

}

int Properties::readConfigIntoBuffer (char* buf, size_t max_size) {
int bytesRead = 0;
std::istream & lIStream = configFileManagedInternally?inputFileStream:*inputStream;

	lIStream.exceptions(lIStream.badbit);

	try {
		lIStream.read(buf,max_size);
		bytesRead = lIStream.gcount();
	} catch (std::istream::failure const & e) {
		throw ExceptionConfigReadError(e.what());
	}

	return bytesRead;

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
		str++;
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
			str++;
		}
	}

	// Now the exponent
	if (*str == 'e' || *str == 'E') {
		str++;

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
			str++;
		}

		// Calculate the exp factor by calculating the power of 10 brute force :)
		for (i=0; i < exp; i++) {
			expFactor *= 10.0l;
		}

		if (expNegative)  {
			expFactor = 1.0l / expFactor;
		}

		rc *= expFactor;
	}

	if (negative) {
		rc = -rc;
	}

	return rc;
}

} // namespace Properties4CXX {
