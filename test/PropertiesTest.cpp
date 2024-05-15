/*
 * PropertiesTest.cpp
 *
 *  Created on: May 1, 2018
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
#  include "config.h"
#endif

#include <iostream>
#include <cstring>
#include <clocale>

#include "Properties4CXX/Properties.h"
#include "Properties4CXX/Property.h"

static void testString (Properties4CXX::Properties const &props,const char* propName,char const *compVal) {

	try {
		Properties4CXX::Property const &prop = *props.searchProperty(propName);

		if (prop.isString()) {
			if (! strcmp(prop.getStrValue(), compVal)) {
				std::cout << propName << " OK" << std::endl;
			} else {
				std::cout << propName << " NOK: value \"" << prop.getStrValue() << "\" not equal" << compVal << std::endl;
			}
		} else {
			std::cout << propName << " NOK: is not a string property" << std::endl;
		}

	} catch (std::exception const &e) {
		std::cout << "Exception in testString (\"" << propName <<"\"," << compVal << "): " << e.what()  << std::endl;
	}
}

static void testInt (Properties4CXX::Properties const &props,const char* propName,long long compVal) {

	try {
		Properties4CXX::Property const &prop = *props.searchProperty(propName);

		if (prop.isInteger()) {
			if (prop.getIntVal() == compVal) {
				std::cout << propName << " OK" << std::endl;
			} else {
				std::cout << propName << " NOK: value " << prop.getIntVal() << " not equal" << compVal << std::endl;
			}
		} else {
			std::cout << propName << " NOK: is not a integer property" << std::endl;
		}
	} catch (std::exception const &e) {
		std::cout << "Exception in testInt (\"" << propName <<"\"," << compVal << "): " << e.what()  << std::endl;
	}
}

static void testDouble (Properties4CXX::Properties const &props,const char* propName,long double compVal) {

	try {
		Properties4CXX::Property const &prop = *props.searchProperty(propName);

		if (prop.isDouble()) {
			if (prop.getDoubleValue() == compVal) {
				std::cout << propName << " OK" << std::endl;
			} else {
				std::cout << propName << " NOK: value " << prop.getDoubleValue() << " not equal" << compVal << std::endl;
			}
		} else {
			std::cout << propName << " NOK: is not a double property" << std::endl;
		}
	} catch (std::exception const &e) {
		std::cout << "Exception in testDouble (\"" << propName <<"\"," << compVal << "): " << e.what()  << std::endl;
	}
}

static void testBool (Properties4CXX::Properties const &props,const char* propName,bool compVal) {

	try {
		Properties4CXX::Property const &prop = *props.searchProperty(propName);

		if (prop.isBool()) {
			if (prop.getBoolValue() == compVal) {
				std::cout << propName << " OK" << std::endl;
			} else {
				std::cout << propName << " NOK: value " << prop.getBoolValue() << " not equal" << compVal << std::endl;
			}
		} else {
			std::cout << propName << " NOK: is not a bool property" << std::endl;
		}
	} catch (std::exception const &e) {
		std::cout << "Exception in testBool (\"" << propName <<"\"," << compVal << "): " << e.what() << std::endl;
	}
}

static void testList (Properties4CXX::Properties const &props,const char* propName,char const **strings,int lenList) {

	try {
		Properties4CXX::Property const &prop = *props.searchProperty(propName);

		if (prop.isList()) {
			Properties4CXX::PropertyValueList l =  prop.getPropertyValueList();
			auto it = l.cbegin();
			int i = 0;

			for (i=0;i<lenList;i++) {
				if (it == l.cend()) {
					std::cout << propName << " NOK: Expected " << i << ". list item \"" << strings[i] << "\" not in the property." << std::endl;
				} else {
					if ((*it) != strings[i] ) {
						std::cout << propName << " NOK: " << i << ". list item \"" << strings[i] << "\" not equal list value" << *it << std::endl;
					} else {
						std::cout << propName << "." << i << " OK" << std::endl;
					}
				}
				it++;
			}

			if (it != l.cend()) {
				std::cout << propName << " NOK: " << i << ". list item \"" << *it << "\" is not in the compare list."<< std::endl;
			}
		} else {
			std::cout << propName << " NOK: is not a list property" << std::endl;
		}
	} catch (std::exception const &e) {
		std::cout << "Exception in testList (\"" << propName <<"\": " << e.what() << std::endl;
	}
}


int main(int argc,char**argv) {

	// Allow locale setting to become effective
	std::setlocale (LC_ALL, "");

	std::ofstream outStream;

	const char* configFileContent =
			" # a comment \n"
			" prop01 = aProperty # with a line comment\n"
			"\n"
			" prop02 = \" A quoted property with \\\t tab and\\\nnewline and \\\" double quotes and \\\' ' single quotes \" # and a comment\n"
			"\n"
			" # Integers\n"
			" prop03 = 112233\n"
			" prop04 = +112233\n"
			" prop05 = -112233\n"
			" prop06 = 0334455 # octal\n"
			" prop07 = 0x124abcde # hex\n"
			" prop08 = 0X124AbCdE # hex\n"
			" prop09 = 0b101100101011 # binary\n"
			"\n"
			" # double values\n"
			" prop10 = 12345.678\n"
			" prop11 = +12345.678\n"
			" prop12 = -12345.678\n"
			" prop13 = 12345.678E2\n"
			" prop14 = +12345.678E-2\n"
			" prop15 = -12345.678E+2\n"
			" prop16 = 12345678E2\n"
			" prop17 = +12345678E-2\n"
			" prop18 = -12345678E+2\n"
			" prop19 = .12345678E2\n"
			" prop20 = +.12345678E-2\n"
			" prop21 = -.12345678E+2\n"
			"\n"
			" # lists\n"
			" prop22 = item1, item2 , item3\n"
			" prop23 = \" item1 \" , item2,\" Item\\\t3\\\nand a newline \"\n"
			" prop24 = { # a structure \n"
			" 	prop24.prop1 = aProperty # with a line comment\n"
			" 	prop24.prop2 = 0x124abcde # hex\n"
			" 	prop24.prop3 = +.12345678E-2\n"
			"\n"
			" 	prop4 = \" item1 \" , item2,\" Item\t3\nand a newline \"\n"
			"}\n"
			"\n"
			" prop25 = on\n"
			" prop26 = off\n"
			" prop27 = ON\n"
			" prop28 = OFF\n"
			" prop29 = On\n"
			" prop30 = OFf\n"
			" prop31 = yes\n"
			" prop32 = no\n"
			" prop33 = YES\n"
			" prop34 = NO\n"
			" prop35 = YEs\n"
			" prop36 = No\n"
			" prop37 = true\n"
			" prop38 = false\n"
			" prop39 = TRUE\n"
			" prop40 = FALSE\n"
			" prop41 = TRue\n"
			" prop42 = FALse\n"
			;

	outStream.open("PropertiesTest.properties",outStream.out|outStream.trunc);
	outStream << configFileContent << std::endl;
	outStream.close();

	Properties4CXX::Properties props("PropertiesTest.properties");

	props.readConfiguration();

	testString (props,"prop01","aProperty");
	testString (props,"prop02"," A quoted property with \t tab and\nnewline and \" double quotes and \' ' single quotes ");

	testInt (props, "prop03", 112233);
	testInt (props, "prop04", +112233);
	testInt (props, "prop05", -112233);
	testInt (props, "prop06", 0334455);
	testInt (props, "prop07", 0x124abcde);
	testInt (props, "prop08", 0X124AbCdE);
	testInt (props, "prop09", 0b101100101011);
	// test the exception
	testInt (props, "prop109", 0b101100101011);

	testDouble (props, "prop10", 12345.678);
	testDouble (props, "prop11", +12345.678);
	testDouble (props, "prop12", -12345.678);
	testDouble (props, "prop13", 12345.678E2);
	testDouble (props, "prop14", +12345.678E-2);
	testDouble (props, "prop15", -12345.678E+2);
	testDouble (props, "prop16", 12345678E2);
	testDouble (props, "prop17", +12345678E-2);
	testDouble (props, "prop18", -12345678E+2);
	testDouble (props, "prop19", .12345678E2);
	testDouble (props, "prop20", +.12345678E-2);
	testDouble (props, "prop21", -.12345678E+2);
	// test the exception
	testDouble (props, "prop121", -.12345678E+2);

	char const *listValues[] = {
			" item1 " ,
			"item2",
			" Item\t3\nand a newline ",
			"Moose"
	};

	// testList(props,"prop23",listValues,2);
	testList(props,"prop23",listValues,3);
	// testList(props,"prop23",listValues,4);

	try {
		Properties4CXX::Property const& prop = *props.searchProperty("prop24");
		if (prop.isStruct()) {
			Properties4CXX::Properties const &structProps = prop.getPropertiesStructure();

			testString(structProps,"prop24.prop1","aProperty");
			testInt(structProps,"prop24.prop2",0x124abcde);
			testDouble(structProps,"prop24.prop3",+.12345678E-2);
		} else {
			std::cout << "prop24 NOK: is not a struct property" << std::endl;
		}

	} catch (std::exception const &e) {
		std::cout << "Cannot find property prop24" << std::endl;
	}

	testBool(props,"prop25",true);
	testBool(props,"prop26",false);
	testBool(props,"prop27",true);
	testBool(props,"prop28",false);
	testBool(props,"prop29",true);
	testBool(props,"prop30",false);
	testBool(props,"prop31",true);
	testBool(props,"prop32",false);
	testBool(props,"prop33",true);
	testBool(props,"prop34",false);
	testBool(props,"prop35",true);
	testBool(props,"prop36",false);
	testBool(props,"prop37",true);
	testBool(props,"prop38",false);
	testBool(props,"prop39",true);
	testBool(props,"prop40",false);
	testBool(props,"prop41",true);
	testBool(props,"prop42",false);
	// test the exception
	testBool(props,"prop142",false);

	// Test locale independence upon writing new values.
	props.addProperty(new Properties4CXX::PropertyDouble("newProp01",123.456));
	props.addProperty(new Properties4CXX::PropertyDouble("newProp02",-12345.678E+2));

	outStream.open("PropertiesTestWrite.properties",outStream.out);
	props.writeOut(outStream);
	outStream.close();


}



