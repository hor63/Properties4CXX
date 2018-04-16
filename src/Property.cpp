/*
 * Property.cpp
 *
 *  Created on: Mar 26, 2018
 *      Author: kai_horstmann
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


#include "Properties4CXX/Property.h"

#include <sstream>
#include <math.h>

namespace Properties4CXX {

ExceptionWrongPropertyType::~ExceptionWrongPropertyType() noexcept {

}

const char*
ExceptionWrongPropertyType::what() const noexcept {
	return description.c_str();
}




Property::Property(char const* propertyName, char* const propertyValue,bool stringIsQuoted, int structLevel)
	:propertyName {propertyName},
	stringValue {propertyValue},
	isStringValueDefined {true},
	isStringQuoted{stringIsQuoted},
	structLevel{structLevel}
	{ }

Property::Property(char const* propertyName, int structLevel)
	:propertyName {propertyName},
	isStringValueDefined {false},
	isStringQuoted{false},
	structLevel{structLevel}
	{ }



Property::~Property() {

}

std::string const &Property::getStringValue() const {

	if (!isStringValueDefined) {
		setLazyStringValue();
	}

	return stringValue;

}

char const *Property::getStrValue() const {

	return stringValue.c_str();

}

long long Property::getIntVal() const {

	throwWrongTypeException("Integer");

	return 0l;
}

double Property::getDoubleValue() const {

	throwWrongTypeException("Double");

	return 0.0;
}

bool Property::getBoolValue() const {

	throwWrongTypeException("Boolean");

	return false;

}

PropertyValueList const &Property::getPropertyValueList() const {

	PropertyValueList* ret = 0;

	throwWrongTypeException("Value List");

	return *ret;
}

Properties const& Property::getPropertiesStructure() const {

	Properties *ret = 0;

	throwWrongTypeException("Sub-structure");

	return *ret;
}

virtual void Property::setStructLevel(int structLevel) {
	this->structLevel = structLevel;
}

std::ostream &Property::writeOut (std::ostream &os) const {

	if (structLevel > 0) {
		std::string padString;
		padString.append (structLevel,'\t');

		os << padString;
	}


	os << propertyName << " = ";


	os << std::endl;

	return os;

}

std::ostream &Property::writeOutValue (std::ostream &os) const {


	if (!isStringValueDefined) {
		setLazyStringValue();
	}

	if (isStringQuoted){

		os.put('"');

	}

	streamEscapedString (os,stringValue);

	if (isStringQuoted){

		os.put('"');

	}

	return os;
}


void Property::throwWrongTypeException (char const* expectedPropertyTypeName) const {

	std::ostringstream strstr;
	strstr << "Property " << propertyName << " is not a " << expectedPropertyTypeName << " value.";

	throw ExceptionWrongPropertyType(strstr.str());
}

void Property::setLazyStringValue () const {
	// The string value is immutable, therefore always defined.
	isStringValueDefined = true;
}

void Property::streamEscapedString (std::ostream &os, std::string const &str) const {

	char const *cstr = str.c_str();

	while (*cstr) {
		switch (*cstr) {
		case '\"':
			os << "\\\"";
			break;
		case '\\':
			os << "\\\\";
			break;

		case '\f':
			os << "\\f";
			break;

		case '\t':
			os << "\\t";
			break;

		case '\v':
			os << "\\v";
			break;

		case '\n':
			if (isNewlineEscaped) {
				os << "\\n";
			} else {
				os.put('\n');
			}
			break;

		case '\r':
			if (isNewlineEscaped) {
				os << "\\r";
			} else {
				os.put('\r');
			}
			break;

		default:
			os.put (*cstr);

		}
		cstr++;
	}

}


PropertyDouble::PropertyDouble(char const* propertyName, char* const propertyValue,double propertyValueDbl, int structLevel)
	:Property(propertyName,propertyValue,/*stringIsQuoted*/false,structLevel),
	 doubleValue{propertyValueDbl} {

	 }

PropertyDouble::~PropertyDouble() {

}

long long PropertyDouble::getIntVal() const {

	return (long long) (round(doubleValue));

}

double PropertyDouble::getDoubleValue() const {
	return doubleValue;
}

PropertyInt::PropertyInt(char const* propertyName, char* const propertyValue,long long propertyValueInt, int structLevel)
	:Property(propertyName,propertyValue,/*stringIsQuoted*/false,structLevel),
	 intValue{propertyValueInt} {

	 }

PropertyInt::~PropertyInt() {

}

long long PropertyInt::getIntVal() const {

	return intValue;

}

double PropertyInt::getDoubleValue() const {
	return double (intValue);
}

PropertyBool::PropertyBool(char const* propertyName, char* const propertyValue,bool propertyValueBool, int structLevel)
	:Property(propertyName,propertyValue,/*stringIsQuoted*/false,structLevel),
	 boolValue{propertyValueBool} {

	 }

PropertyBool::~PropertyBool() {

}

bool PropertyBool::getBoolValue() const {

	return boolValue;

}

PropertyList::PropertyList(char const* propertyName, PropertyValueList const &valueList, int structLevel)
	:Property{propertyName,structLevel},
	 valueList{valueList}
{

}

/** \brief Destructor
 * Virtual is a must here because it will be overloaded.
 */
PropertyList::~PropertyList() {

}


/** \brief Returns the list of strings of the property
 *
 * \see Property::getPropertyValueList
 * \see PropertyValueList
 *
 * @return List of string values of the property
 */
PropertyValueList const &PropertyList::getPropertyValueList() const {
	return valueList;
}


void PropertyList::setLazyStringValue() const {
	std::string newString;
	auto it = valueList.cbegin();

	if (it != valueList.cend()) {
		newString = '"';
		newString.append(*it);
		newString.append(1,'"');
		it++;
	}

	while (it != valueList.cend()) {
		newString.append(" , \"");
		newString.append(*it);
		newString.append(1,'"');
		it++;
	}

	isStringValueDefined = true;

}



} /* namespace Properties4CXX */
