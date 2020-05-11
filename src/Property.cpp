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

Property::Property(char const* propertyName, char const* propertyValue,bool stringIsQuoted, int structLevel)
	:propertyName {propertyName},
	stringValue {propertyValue},
	isStringValueDefined {true},
	isStringQuoted{stringIsQuoted},
	structLevel{structLevel},
	propertyType{String}
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

	if (!isStringValueDefined) {
		setLazyStringValue();
	}

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

void Property::setStructLevel(int structLevel) {
	this->structLevel = structLevel;
}

std::ostream &Property::writeOut (std::ostream &os) const {

	if (structLevel > 0) {
		std::string padString;
		padString.append (structLevel,'\t');

		os << padString;
	}

	os << propertyName << " = ";
	writeOutValue(os);
	os << std::endl;

	return os;

}

std::ostream &Property::writeOutValue (std::ostream &os) const {


	if (!isStringValueDefined) {
		setLazyStringValue();
	}

	if (isStringQuoted){

		os.put('"');

		streamEscapedString (os,stringValue);

		os.put('"');

	} else {
		os << stringValue;
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


PropertyDouble::PropertyDouble(char const* propertyName, char const* propertyValue,double propertyValueDbl, int structLevel)
	:Property(propertyName,propertyValue,/*stringIsQuoted*/false,structLevel),
	 doubleValue{propertyValueDbl}
{
	propertyType = Double;
}

PropertyDouble::PropertyDouble(char const* propertyName, double propertyValueDbl, int structLevel)
	:Property(propertyName,std::to_string(propertyValueDbl).c_str(),/*stringIsQuoted*/false,structLevel),
	 doubleValue{propertyValueDbl}
{
	propertyType = Double;
}

PropertyDouble::~PropertyDouble() {

}

long long PropertyDouble::getIntVal() const {

	return (long long) (round(doubleValue));

}

double PropertyDouble::getDoubleValue() const {
	return doubleValue;
}

PropertyInt::PropertyInt(char const* propertyName, char const* propertyValue,long long propertyValueInt, int structLevel)
	:Property(propertyName,propertyValue,/*stringIsQuoted*/false,structLevel),
	 intValue{propertyValueInt}
{
	propertyType = Integer;
}

PropertyInt::PropertyInt(char const* propertyName, long long propertyValueInt, int structLevel)
	:Property(propertyName,std::to_string(propertyValueInt).c_str(),/*stringIsQuoted*/false,structLevel),
	 intValue{propertyValueInt}
{
	propertyType = Integer;
}

PropertyInt::~PropertyInt() {

}

long long PropertyInt::getIntVal() const {

	return intValue;

}

double PropertyInt::getDoubleValue() const {
	return double (intValue);
}

PropertyBool::PropertyBool(char const* propertyName, char const* propertyValue,bool propertyValueBool, int structLevel)
	:Property(propertyName,propertyValue,/*stringIsQuoted*/false,structLevel),
	boolValue{propertyValueBool}
{
	propertyType = Bool;
}

PropertyBool::PropertyBool(char const* propertyName, bool propertyValueBool, int structLevel)
	:Property(propertyName,propertyValueBool?"true":"false",/*stringIsQuoted*/false,structLevel),
	 boolValue{propertyValueBool}
 {
	propertyType = Bool;
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
	propertyType = List;
}

PropertyList::PropertyList(char const* propertyName, int structLevel)
	:Property{propertyName,structLevel}
{

}

PropertyList::~PropertyList() {

}


PropertyValueList const &PropertyList::getPropertyValueList() const {
	return valueList;
}


void PropertyList::setLazyStringValue() const {
	std::ostringstream newString;
	auto it = valueList.cbegin();

	if (it != valueList.cend()) {
		newString.put('"');
		streamEscapedString(newString, *it);
		newString.put('"');
		it++;
	}

	while (it != valueList.cend()) {
		newString << " , \"";
		streamEscapedString(newString, *it);
		newString.put('"');
		it++;
	}

	stringValue = newString.str();
	isStringValueDefined = true;

}

void PropertyList::appendString (std::string const &str) {
	valueList.push_back(str);
	isStringValueDefined = false;
}

PropertyStruct::PropertyStruct(char const* propertyName, int structLevel)
	:Property{propertyName,structLevel},
	 propertyList{new Properties}
{
	propertyType = Struct;
	this->propertyList->setStructLevel(structLevel + 1);
}

PropertyStruct::PropertyStruct(char const* propertyName, Properties const &propertyList, int structLevel)
	:Property{propertyName,structLevel},
	 propertyList{new Properties}
{
	propertyType = Struct;
	this->propertyList->getPropertyMap() = propertyList.getCPropertyMap();
	this->propertyList->setStructLevel(structLevel + 1);
}


PropertyStruct::~PropertyStruct() {
	delete propertyList;
}


Properties const& PropertyStruct::getPropertiesStructure() const {

	return *propertyList;

}

void PropertyStruct::setStructLevel(int structLevel) {

	Property::setStructLevel(structLevel);
	propertyList->setStructLevel(structLevel + 1);

}

void PropertyStruct::addProperty (Property  *prop) {

	propertyList->addProperty(prop);

}

void PropertyStruct::setLazyStringValue() const {
	std::ostringstream ostr;

	ostr << '{' << std::endl;
	propertyList->writeOut(ostr);

	if (structLevel > 0) {
		std::string padString;
		padString.append (structLevel,'\t');

		ostr << padString;
	}

	ostr << '}';

	stringValue = ostr.str();
	isStringValueDefined = true;


}


} /* namespace Properties4CXX */
