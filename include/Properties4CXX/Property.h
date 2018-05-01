/*
 * Property.h
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

#ifndef INCLUDE_PROPERTIES4CXX_PROPERTY_H_
#define INCLUDE_PROPERTIES4CXX_PROPERTY_H_

#include <exception>
#include <string>
#include <list>
#include <memory>
#include <ostream>


#if !defined PROPERTIES4CXX_EXPORT
#define PROPERTIES4CXX_EXPORT
/**
 * Define PROPERTIES4CXX_DLL_IMPORT, PROPERTIES4CXX_DLL_EXPORT, and PROPERTIES4CXX_DLL_LOCAL for Windows and Linux (ELF) ports of gcc and non-gcc compilers
 *
 * The macro definitions are highly inspired from the <a href="https://gcc.gnu.org/wiki/Visibility">GCC Wiki: Visibility</a>
 */
#if defined _WIN32 || defined __CYGWIN__
    #ifdef __GNUC__
      #define PROPERTIES4CXX_DLL_EXPORT __attribute__ ((dllexport))
      #define PROPERTIES4CXX_DLL_IMPORT __attribute__ ((dllimport))
    #else
      #define PROPERTIES4CXX_DLL_EXPORT __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
      #define PROPERTIES4CXX_DLL_IMPORT __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
    #ifdef __GNUC__
    #else
    #endif
  #define PROPERTIES4CXX_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define PROPERTIES4CXX_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define PROPERTIES4CXX_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define PROPERTIES4CXX_DLL_EXPORT
    #define PROPERTIES4CXX_DLL_LOCAL
  #endif
  #define PROPERTIES4CXX_DLL_IMPORT
#endif

#if defined (BUILDING_PROPERTIES4CXX)
  #define PROPERTIES4CXX_PUBLIC PROPERTIES4CXX_DLL_EXPORT
  #define PROPERTIES4CXX_LOCAL  PROPERTIES4CXX_DLL_LOCAL
#else /* BUILDING_PROPERTIES4CXX */
  #define PROPERTIES4CXX_PUBLIC PROPERTIES4CXX_DLL_IMPORT
  #define PROPERTIES4CXX_LOCAL  PROPERTIES4CXX_DLL_LOCAL
#endif /* BUILDING_PROPERTIES4CXX */

#endif /* #define PROPERTIES4CXX_EXPORT */


namespace Properties4CXX {
class Property;
}

#include "Properties4CXX/Properties.h"

namespace Properties4CXX {

/// List of string values of a property
/// It can be traversed with standard iterators like here <a href="http://en.cppreference.com/w/cpp/container/list/begin" >std::list::begin</a>
///
/// \see <a href="http://en.cppreference.com/w/cpp/container/list" >std::list</a>
typedef std::list<std::string> PropertyValueList;

/// Exception thrown when an access method of class \ref Property is invoked which is not overloaded by a specific subclass
class PROPERTIES4CXX_DLL_EXPORT
ExceptionWrongPropertyType : public std::exception {
public:
	ExceptionWrongPropertyType(char const *descr) noexcept
		:description {descr}
		{ }
	ExceptionWrongPropertyType(std::string const &descr) noexcept
		:description {descr}
		{ }

	virtual ~ExceptionWrongPropertyType() noexcept;

	/** Returns a C-style character string describing the general cause
	*  of the current error.  */
	virtual const char*
	what() const noexcept;

private:
	std::string description;
};

/** \brief String property class, and base class for complex and non-string property classes
 *
 * The class content is immutable. Once created it cannot be changed any more.
 *
 */
class PROPERTIES4CXX_DLL_EXPORT
Property {
public:

	enum PropertyTypeEnum {
		String,
		Double,
		Integer,
		Bool,
		List,
		Struct
	};

	/** \brief Constructor
	 *
	 * @param propertyName Name of the property
	 * @param propertyValue Value of the property. If the value was quoted the quotes will not be stored here
	 * @param stringIsQuoted Indicates if the property value was a quoted string or stand-alone string.
	 * 	Thus quotation is preserved when a configuration is written out
	 * @param structLevel Number of the structure level on which this property resides. Base level is 0.
	 */
	Property(char const* propertyName, char* const propertyValue,bool stringIsQuoted = true, int structLevel = 0);

	/** \brief Destructor
	 */
	virtual ~Property();

	/**
	 *
	 * @return The name of the property
	 */
	std::string const &getPropertyName() const {
		return propertyName;
	}

	/**
	 *
	 * @return The name of the property
	 */
	char const *getPropertyNameCStr() const {
		return propertyName.c_str();
	}

	/** \brief Return string value of any property
	 *
	 * Any property can be retrieved as a string. All subclasses support this feature.
	 *
	 * Numeric or boolean features store the string verbatim as it occurred in the configuration file.
	 *
	 * List and structure properties will generate a string only on demand, and potentially loose a lot of the original formatting.
	 * Please note that strings in a string list are always being quoted.
	 *
	 * @return String value unquoted
	 */
	std::string const &getStringValue() const;

	/**\brief Return string value of any property as C string
	 *
	 * \see getStringValue()
	 *
	 * @return String value of any property as C string
	 */
	char const *getStrValue() const;

	// Type specific and structure access methods; will be overloaded by sub-classes

	/** \brief Return the integer value of the property.
	 *
	 * Only implemented for class \ref PropertyInt or \ref PropertyDouble.
	 * This class and any other derived class throw an exception \ref ExceptionWrongPropertyType.
	 *
	 * @return Integer value of the property
	 * @throws ExceptionWrongPropertyType
	 */
	virtual long long getIntVal() const;

	/** \brief Return the float value of the property.
	 *
	 * Only implemented for class \ref PropertyInt or \ref PropertyDouble.
	 * This class and any other derived class throw an exception \ref ExceptionWrongPropertyType.
	 *
	 * @return Double value of the property
	 * @throws ExceptionWrongPropertyType
	 */
	virtual double getDoubleValue() const;

	/** \brief Return the boolean value of the property.
	 *
	 * Only implemented for class \ref PropertyBool.
	 * This class and any other derived class throw an exception \ref ExceptionWrongPropertyType.
	 *
	 * @return Boolean value of the property
	 * @throws ExceptionWrongPropertyType
	 */
	virtual bool getBoolValue() const;

	/** \brief Return the list of string values of the property.
	 *
	 * Only implemented for class \ref PropertyList.
	 * This class and any other derived class throw an exception \ref ExceptionWrongPropertyType.

	 * \see PropertyValueList
	 *
	 * @return List of string values of the property
	 * @throws ExceptionWrongPropertyType
	 */
	virtual PropertyValueList const &getPropertyValueList() const;

	/** \brief Return the list of properties which make up this property.
	 *
	 * Only implemented for class \ref PropertyStruct.
	 * This class and any other derived class throw an exception \ref ExceptionWrongPropertyType.
	 *
	 * @return List of properties
	 * @throws ExceptionWrongPropertyType
	 */
	virtual Properties const& getPropertiesStructure() const;

	/** \brief Set or reset newline escaping on printout
	 *
	 * @param isNewlineEscaped
	 */
	void setIsNewlineEscaped (bool isNewlineEscaped) {
		this->isNewlineEscaped = isNewlineEscaped;
	}

	/** \brief Will newline and carriage return characters be printed escaped or verbatim
	 *
	 * @return \ref isNewlineEscaped
	 */
	bool getIsNewlineEscaped () {
		return isNewlineEscaped;
	}

	/** \brief Returns the structure level of the property. Use for indention on printout
	 *
	 * @return Structure level.
	 */
	int getStructLevel() const {
		return structLevel;
	}

	/** \brief Set the structure level for a property for \ref writeOut indention.
	 *
	 * In the base class only sets \ref structLevel.
	 *
	 * komplex sub-classes need to do more. Therefore virtual
	 *
	 * @param structLevel New structure level
	 */
	virtual void setStructLevel(int structLevel);

	/** \brief Helper for \ref ::std::ostream &operator << (std::ostream &os,const Properties4CXX::Property &property)
	 *
	 * @param os Output stream
	 * @return Reference to <a href="http://en.cppreference.com/w/cpp/io/basic_ostream" >std::ostream</a>
	 */
	std::ostream &writeOut (std::ostream &os) const;

	/**
	 * Write \ref stringValue to the output stream os while writing masked characters instead of non-standard characters.
	 * Non-standard characters and their masking are:
	 *
	 * - \\"	double quote	byte 0x22 in ASCII encoding
	 * - \\\\	backslash	byte 0x5c in ASCII encoding
	 * - \\f	form feed - new page	byte 0x0c in ASCII encoding
	 * - \\n	line feed - new line	byte 0x0a in ASCII encoding
	 * - \\r	carriage return	byte 0x0d in ASCII encoding
	 * - \\t	horizontal tab	byte 0x09 in ASCII encoding
	 * - \\v	vertical tab	byte 0x0b in ASCII encoding
	 *
	 * The next two depend on \ref isNewlineEscaped. If it is set newline and carriage return characters are masked. Otherwise they are printed verbatim
	 * - \\n	line feed - new line	byte 0x0a in ASCII encoding
	 * - \\r	carriage return	byte 0x0d in ASCII encoding
	 *
	 * @param os Output stream to which the content of \ref stringValue is streamed
	 */
	void streamEscapedString (std::ostream &os, std::string const &str) const;

	bool isString () {
		return propertyType == String;
	}
	bool isDouble () {
		return propertyType == Double;
	}
	bool isInteger () {
		return propertyType == Integer;
	}
	bool isBool () {
		return propertyType == Bool;
	}
	bool isList () {
		return propertyType == List;
	}
	bool isStruct () {
		return propertyType == Struct;
	}



protected:

	/** \brief Constructor for subclasses
	 *
	 * This constructor is used by the complex subclasses which use lazy string value writing
	 *
	 * @param propertyName Name of the property
	 * @param structLevel Number of the structure level on which this property resides. Base level is 0.
	 */
	Property(char const* propertyName, int structLevel = 0);

	/** \brief Helper function to throw an \ref ExceptionWrongPropertyType exception when a not supported property type is requested
	 *
	 * @param expectedPropertyTypeName Description/name of the requested return type
	 * @throws \ref ExceptionWrongPropertyType *always*
	 */
	void throwWrongTypeException (char const* expectedPropertyTypeName) const;

	/** \brief Write \ref stringValue on demand when the value was undefined before.
	 *
	 * Derived classes (structures and lists) can set the string value lazily.
	 * in \ref writeOut this method is called when \ref isStringValueDefined is false.
	 *
	 * In the base class and other scalar type property classes the string value is always defined.
	 * Therefore here the function only sets \ref isStringValueDefined true.
	 */
	virtual void setLazyStringValue() const;

	/** \brief Internal helper for \ref setLazyStringValue
	 *
	 * Internal helper function for \ref setLazyStringValue which cannot access
	 *
	 * @param str string value of the property
	 */
	void setStringValueInternal (std::string const &str) const;

	/** \brief Helper for \ref writeOut to stream the value of a property
	 *
	 * @param os Output stream
	 * @return Reference to <a href="http://en.cppreference.com/w/cpp/io/basic_ostream" >std::ostream</a>
	 */
	virtual std::ostream &writeOutValue (std::ostream &os) const;

	std::string const propertyName;

	/** \brief Newline and carriage return characters are to printed escaped as \\n and \\r. If false they are printed verbatim
	 *
	 */
	bool isNewlineEscaped = true;

	/** \brief Level of nested structures. Used for indent for printout.
	 *
	 * Top level is 0
	 * Used for indent structured print-out with \ref writeOut
	 */
	int structLevel = 0;




	/** \brief Un-quoted string value
	 *
	 * String value is always stored without potential surrounding quotation.
	 * The string value can be retrieved for any type of property.
	 * It is mutable because it is written in a lazy fashion for structure and list properties.
	 *
	 */
	mutable std::string stringValue;

	/** \brief  Used for lazy string value definition by derived classes
	 *
	 * Used for lazy string value definition. This is useful for \ref PropertyStruct classes where a string representation of the entire sub-structure
	 * will be required only when the configuration is being written out.
	 */
	mutable bool isStringValueDefined = false;

	PropertyTypeEnum propertyType = String;

	// A bit of stuff is quite critical, and needs to be handled within the class. Also derived classes have to access it via the interface
private:

	/** \brief Determines if the value string was quoted or not.
	 *
	 * Quotation is preserved in \ref writeOut and
	 * \ref ::std::ostream &operator << (std::ostream &os,const Properties4CXX::Property &property)
	 */
	bool isStringQuoted = true;


};


/** \brief Double float property class, derived from \ref Property
 *
 * The class content is immutable. Once created it cannot be changed any more.
 *
 */
class PROPERTIES4CXX_DLL_EXPORT
PropertyDouble : public Property {
public:

	/** \brief Constructor
	 *
	 * @param propertyName Name of the property
	 * @param propertyValue String value of the property as written in the configuration.
	 * @param propertyValueDbl double float value of the property as binary value
	 * @param structLevel Number of the structure level on which this property resides. Base level is 0.
	 */
	PropertyDouble(char const* propertyName, char* const propertyValue,double propertyValueDbl, int structLevel = 0);

	/** \brief Destructor
	 * Destructor. Virtual is a must here because it will be overloaded.
	 */
	virtual ~PropertyDouble();



	/** \brief Return the integer value of the property.
	 *
	 * The double float property value is rounded to the next integer
	 *
	 * @return Integer value of the property
	 */
	virtual long long getIntVal() const override;

	/** \brief Return the float value of the property.
	 *
	 * @return Double value of the property
	 */
	virtual double getDoubleValue() const override;


private:

	double doubleValue;


};

/** \brief Integer property class, derived from \ref Property
 * Integer property class, derived from \ref Property
 *
 * The class content is immutable. Once created it cannot be changed any more.
 *
 */
class PROPERTIES4CXX_DLL_EXPORT
PropertyInt : public Property {
public:
	/** \brief Constructor
	 *
	 * @param propertyName Name of the property
	 * @param propertyValue String value of the property as written in the configuration.
	 * @param propertyValueInt long long int value of the property as binary value
	 * @param structLevel Number of the structure level on which this property resides. Base level is 0.
	 */
	PropertyInt(char const* propertyName, char* const propertyValue,long long propertyValueInt, int structLevel = 0);

	/** \brief Destructor
	 * Virtual is a must here because it will be overloaded.
	 */
	virtual ~PropertyInt();



	/** \brief Return the integer value of the property.
	 *
	 *  The double float property value is rounded to the next integer
	 *
	 * @return Integer value of the property
	 */
	virtual long long getIntVal() const override;

	/** \brief Return the float value of the property.
	 *
	 * @return Double value of the property
	 */
	virtual double getDoubleValue() const override;


protected:

	long long intValue;

};


/** \brief Boolean property class, derived from \ref Property
 *
 * The class content is immutable. Once created it cannot be changed any more.
 *
 */
class PROPERTIES4CXX_DLL_EXPORT
PropertyBool : public Property {
public:

	/** \brief Constructor
	 *
	 * @param propertyName Name of the property
	 * @param propertyValue String value of the property as written in the configuration.
	 * @param propertyValueBool bool value of the property as binary value
	 * @param structLevel Number of the structure level on which this property resides. Base level is 0.
	 */
	PropertyBool(char const* propertyName, char* const propertyValue,bool propertyValueBool, int structLevel = 0);

	/** \brief Destructor
	 *
	 * Virtual is a must here because it will be overloaded.
	 */
	virtual ~PropertyBool();



	/** \brief Return the boolean value of the property.
	 *
	 *  @return Boolean value of the property
	 */
	virtual bool getBoolValue() const override;

protected:

	bool boolValue;

};

/** \brief Property list class. This is a list of string values under one property name
 *
 * The list is created with an initial list of values. Additional strings can be added afterwards.
 *
 */
class PROPERTIES4CXX_DLL_EXPORT
PropertyList :public Property {
public:

	/** \brief Constructor
	 *
	 * This constructor creates an empty list. Value list items must be added with \ref appendString().
	 *
	 * @param propertyName Name of the property
	 * @param valueList List of string values assigned to the property
	 * @param structLevel Number of the structure level on which this property resides. Base level is 0.
	 */
	PropertyList(char const* propertyName, int structLevel = 0);

	/** \brief Constructor
	 *
	 * This constructor creates a value list with a pre-populated list of string values.
	 * You can append additional string values with \ref appendString().
	 *
	 * @param propertyName Name of the property
	 * @param valueList List of string values assigned to the property
	 * @param structLevel Number of the structure level on which this property resides. Base level is 0.
	 */
	PropertyList(char const* propertyName, PropertyValueList const &valueList, int structLevel = 0);

	/** \brief Destructor
	 * Virtual is a must here because it will be overloaded.
	 */
	virtual ~PropertyList();


	/** \brief Returns the list of strings of the property
	 *
	 * \see Property::getPropertyValueList
	 * \see PropertyValueList
	 *
	 * @return List of string values of the property
	 */
	virtual PropertyValueList const &getPropertyValueList() const override;

	/** \brief Add a string to the list
	 *
	 * @param str String to be added to the list
	 */
	void appendString (std::string const &str);

protected:


	/** \brief Set the string value on demand
	 *
	 * In \ref writeOut this method is called when \ref Property::isStringValueDefined is false.
	 *
	 * The function sets \ref isStringValueDefined true, and calls \ref Property::setStringValueInternal with the current list of string values.
	 */
	virtual void setLazyStringValue() const override;


	// A bit of stuff is quite critical, and needs to be handled within the class. Also derived classes have to access it via the interface
private:

	PropertyValueList valueList;

};


/** \brief Property structure class. This list of properties enclosed in curly brackets, combined under one property name
 *
 * The structure is created from an initial properties list. Additional properties can be added afterwards.
 */
class PROPERTIES4CXX_DLL_EXPORT
PropertyStruct :public Property {
public:

	/** \brief Constructor
	 *
	 * This constructor creates an empty property list.
	 * Properties must be added to the list with \ref appendProperty()
	 *
	 * @param propertyName Name of the property
	 * @param structLevel Number of the structure level on which this property resides. Base level is 0.
	 */
	PropertyStruct(char const* propertyName, int structLevel = 0);

	/** \brief Constructor
	 *
	 * This constructor creates a property list with a pre-populated list of properties.
	 * Additional properties can be added to the list with \ref appendProperty()
	 *
	 * @param propertyName Name of the property
	 * @param propertyList List of properties.
	 * @param structLevel Number of the structure level on which this property resides. Base level is 0.
	 */
	PropertyStruct(char const* propertyName, Properties const &propertyList, int structLevel = 0);

	/** \brief Destructor
	 *
	 * Virtual is a must here because it will be overloaded.
	 */
	virtual ~PropertyStruct();


	/** \brief Return the list of properties which make up this property.
	 *
	 * \see Property::getPropertiesStructure()
	 * \see Property
	 *
	 * @return List of properties
	 */
	virtual Properties const& getPropertiesStructure() const;

	/** \brief Set the structure level for a property structure for \ref writeOut indention.
	 *
	 * \see Property::setStructLevel
	 *
	 * @param structLevel New structure level
	 */
	virtual void setStructLevel(int structLevel);


	/** \brief Add a property to the property list
	 *
	 * @param prop Pointer to a new property. This takes ownership of the property. The caller must never delete the passed \ref prop!
	 */
	void addProperty (Property *prop);

protected:


	/** \brief Set the string value on demand
	 *
	 * in \ref writeOut this method is called when \ref Property::isStringValueDefined is false.
	 *
	 * The function sets \ref isStringValueDefined true, and calls \ref Property::setStringValueInternal with the current list of string values.
	 */
	virtual void setLazyStringValue() const override;


	// A bit of stuff is quite critical, and needs to be handled within the class. Also derived classes have to access it via the interface
private:

	/** \brief List of properties in a sub-structure
	 *
	 * This is a pointer due to hen&egg problem: Here the declaration of class \ref Properties is incomplete.
	 */
	Properties *propertyList = 0;

};


} /* namespace Properties4CXX */

/** \brief Output stream operator for \ref Properties4CXX::Property objects.
 *
 * Please note that the helper function \ref Properties4CXX::Property::writeOut is virtual and overloaded by list and struct properties.
 * A property is always terminated by a newline. Therefore this << operator always prints a \ref std::endl.
 *
 * @param os Output stream
 * @param property \ref Property to be printed/streamed.
 * @return Reference to <a href="http://en.cppreference.com/w/cpp/io/basic_ostream" >std::ostream</a>
 */
static inline std::ostream &operator << (std::ostream &os,const Properties4CXX::Property &property) {
	property.writeOut(os) << std::endl;

	return os;
}

#endif /* INCLUDE_PROPERTIES4CXX_PROPERTY_H_ */
