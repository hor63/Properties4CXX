/*
 * Properties.h
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

#ifndef INCLUDE_PROPERTIES4CXX_PROPERTies_H_
#define INCLUDE_PROPERTIES4CXX_PROPERTies_H_

#include <memory>
#include <sstream>
#include <fstream>
#include <map>
#include <string>

namespace Properties4CXX {
class Properties;
}

#include "Properties4CXX/Property.h"

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

/** \brief Base for all exceptions in the namespace Properties4CXX
 *
 */
class PROPERTIES4CXX_PUBLIC
ExceptionBase : public std::exception {
public:

	ExceptionBase (char const *descr)
			:description{descr}
			{}

	virtual ~ExceptionBase ();

    /** Returns a C-style character string describing the general cause
     *  of the current error.  */
    virtual const char*
    what() const noexcept override;


private:

	std::string description;
};

/** \brief Exception thrown when a read error on the input stream occurs
 *
 */
class PROPERTIES4CXX_PUBLIC
 ExceptionConfigReadError: public ExceptionBase {
public:

	ExceptionConfigReadError(char const *descr)
	  :ExceptionBase{descr}
			{}

	virtual ~ExceptionConfigReadError ();

};

/** \brief Exception thrown when an internally managed input file stream cannot open the configuration file.
 *
 */
class PROPERTIES4CXX_PUBLIC
 ExceptionConfigFileOpenError: public ExceptionBase {
public:

	ExceptionConfigFileOpenError(char const *descr)
	  :ExceptionBase{descr}
			{}

	virtual ~ExceptionConfigFileOpenError ();

};

/** \brief An exception cannot be found using \ref Properties::searchProperty()
 *
 */
class PROPERTIES4CXX_PUBLIC
 ExceptionPropertyNotFound: public ExceptionBase {
public:

	ExceptionPropertyNotFound(char const *descr)
	  :ExceptionBase{descr}
			{}

	virtual ~ExceptionPropertyNotFound ();

};

/** \brief An exception with the same name is inserted twice on one structure level of the configuration using \ref Properties::addProperty()
 *
 */
class PROPERTIES4CXX_PUBLIC
 ExceptionPropertyDuplicate: public ExceptionBase {
public:

	ExceptionPropertyDuplicate(char const *descr)
	  :ExceptionBase{descr}
			{}

	virtual ~ExceptionPropertyDuplicate ();

};

/** \brief Properties reader. Inspired from Java Properties
 *
 * Properties reader. This class implements a properties reader which is enhanced to the very bare-bones Java
 * <a href="https://docs.oracle.com/javase/8/docs/api/java/util/Properties.html" >Properties</a> class.
 * Enhancements are typed properties (Numeric, boolean), quoted (verbatim) strings, lists of values (strings only, quoted or un-quoted)
 * and structures
 *
 * The properties file
 * ===================
 *
 * Basically a properties file looks like
 *
 *     # This is a comment line. Comment and empty lines are ignored.
 *
 *     # Basic form: A key (here 'Property1') has a string value (here 'value1). Space and tab characters are eliminated
 *     # Each key-value pair is written in one line. Separator between key and value is the '=' character.
 *     Property1 = value1
 *
 *     # Verbatim String
 *     Property2 = " This is a String
 *                    which can span across multiple lines, and can have trailing or leading spaces
 *                    The double quotes character is masked by \". "
 *
 *     # Numeric values:
 *     # Integer decimal values
 *     Property3 = -123456
 *
 *     # Integer hexadecimal values (positive values only)
 *     Property4 = 0x12aDf4
 *     Property5 = 0X12aDf4
 *
 *     # Integer octal values (positive values only)
 *     Property6 = 0123456701234
 *
 *     # Integer decimal values
 *     Property7 = 123456
 *
 *     # double float values
 *     Property8  = -1234.678
 *     Property9  = -1234.678E-12
 *     Property10 = .2343e+.2
 *
 *     # Boolean values can be yes, no, true, false, on, off. Case insensitive.
 *     PropTrue1 = True
 *     PropTrue1 = False
 *     PropTrue1 = TRUE
 *     PropTrue1 = Yes
 *     PropTrue1 = no
 *     PropTrue1 = YES
 *     PropTrue1 = On
 *     PropTrue1 = off
 *     PropTrue1 = ON
 *
 *     # Lists of values
 *     Property11 = value1," VPROPERTIES4CXX_PUBLICalue 2 ", sddsds

 *     # Structures
 *     Property12 = {
 *       Property13 = sadfsd
 *       Property14 = 0x22AF
 *       PropList = sd , sd,sd,dds,s
 *       PropStruct = {
 *         PropA = sadfas
 *         # Keys must be unique within one structure level. Therefore key Property12 on this valid on this level
 *         Property12 = Valsdds
 *       }
 *
 *
 *     # UTF-8 keys and values are valid
 *     UmlautüßPr3s15 = Übermenschlich
 *
 *     # All special characters except blank, tab, ',', '{', '}, and '"' are valid for keys and values
 *     Pro.per<;ty16 = |vls<>@!#$%^&
 *
 * Properties of the properties file (pun intended)
 * -------------------
 *
 * Property keys are case sensitive.
 *
 * Property keys must be defined only once. Double definitions lead to an exception.
 *
 * Keys within one structure must be unique within the structure only.
 *
 * All values can be retrieved as strings, even the typed ones.
 *
 * Quoted strings
 * -------------------
 *
 * A quoted string can contain any character (including any UTF-8 character)
 *
 * A few selected special characters can be written as escaped characters.
 * The writing is the same as C/C++.
 *
 * - \\"	double quote	byte 0x22 in ASCII encoding
 * - \\\\	backslash	byte 0x5c in ASCII encoding
 * - \\f	form feed - new page	byte 0x0c in ASCII encoding
 * - \\n	line feed - new line	byte 0x0a in ASCII encoding
 * - \\r	carriage return	byte 0x0d in ASCII encoding
 * - \\t	horizontal tab	byte 0x09 in ASCII encoding
 * - \\v	vertical tab	byte 0x0b in ASCII encoding
 *
 * Any other escaped character is taken over literally. A sequence "\\x" will become "x". The '\\' character is swallowed.
 *
 * Numeric locale
 * -------------------
 *
 * The locale is always "C", i.e. the decimal separator is always the dot '.'. Hundred separators are not allowed.
 *
 * Thus a number in German locale 123,456 is not valid. Neither is a number 123,456.2323 valid input.
 *
 *
 */
class PROPERTIES4CXX_PUBLIC
Properties {
public:

	// A bunch of useful type definitions to avoid writing the templates again and again.
	typedef std::shared_ptr<Property> PropertyPtr;
	typedef std::pair<std::string, PropertyPtr> PropertyPair;
	typedef std::map<std::string, PropertyPtr> PropertyMap;
	typedef PropertyMap::const_iterator PropertyCIterator;
	typedef PropertyMap::iterator PropertyIterator;


	/**
	 * Constructor. Before reading a configuration you must either set the configuration file name,
	 * or set the input stream
	 */
    Properties ();

    /** \brief Constructor defining the input file name
     * Constructor defining the input file name.
     * The file will be opened not earlier when you call \ref readConfiguration
     *
     * @param configFileName: Name of the configuration file. Can either be an absolute or relative path. The path
     * specification must comply with the OS conventions
     *
     */
    Properties (char const *configFileName);

    /** \brief Constructor defining the input file name
     * Constructor defining the input file name.
     * The file will be opened not earlier when you call \ref readConfiguration
     *
     * @param configFileName Name of the configuration file. Can either be an absolute or relative path. The path
     * specification must comply with the OS conventions.
     *
     */
    Properties (std::string const &configFileName);

    /** \brief Constructor defining the input stream
     *
     * @param inputStream Pointer to an input stream (typically an istringstream, i.e. reading out of a memory buffer.
     * \see setInputStream
     *
     */
    Properties (std::istream *inputStream);

    /**
     * The destructor does not care for the inputstream set either by \ref setInputStream or by \ref Properties(std::istream *inputStream)
     * The owner of the inputStream object must close and release it herself.
     */
    virtual ~Properties();

    /** \brief Set or reset the configuration file name
     *
     * Set or reset the configuration file name.
     *
     * Calling this function will reset any previously set input stream set by \ref setInputStream or \ref Properties(std::istream *inputStream)
     * You can call this function as often as you like. Only the last file name set will be used by \ref readConfiguration.
     *
     * This call will reset any inputStream set either by \ref setInputStream or by Properties(std::istream *inputStream)
     *
     * @param configFileName Name of the configuration file. Can either be an absolute or relative path. The path
     * specification must comply with the OS conventions.
     *
     */
    void setFileName (char const *configFileName);

    /** \brief Set or reset the configuration file name
     *
     * Set or reset the configuration file name.
     *
     * Calling this function will reset any previoProperties (std::string const &configFileName)usly set input stream set by \ref setInputStream or \ref Properties(std::istream *inputStream)
     * You can call this function as often as you like. Only the last file name set will be used by \ref readConfiguration.
     *
     * This call will reset any inputStream set either by \ref setInputStream or by Properties(std::istream *inputStream)
     *
     * @param configFileName Name of the configuration file. Can either be an absolute or relative path. The path
     * specification must comply with the OS conventions.
     *
     */
    void setFileName (std::string const configFileName);

    /** \brief Set an input stream as source for the property definitions
     *
     * Set an input stream as source for the property definitions
     *
     * It is the caller's sole responsibility to manage inputStream. Before you call \ref readConfiguration the stream
     * must be open or an istringstream must be initialized, and the read pointer must be rewound.
     *
     * \ref readConfiguration will not close or reset or rewind the stream after reading the stream.
     *
     * This call supersedes any input file name which may have been defined before.
     *
     * @param inputStream Pointer to an input stream which the call manages herself
     */
    void setInputStream (std::istream *inputStream);

    std::string const getConfigFileName() const {
    	return configFileName;
    }

    /** \brief Is the input stream for the configuration managed internally by setting the configuration file name,
     *  or is a user provided input stream being used?
     *
     *  Returns true when the configuration file name was set either by the constructor or \ref setFileName()
     *
     *			" prop23 = \" item1 \" , item2,\" Item\t3\nand a newline \"\n"
     *
     *  Returns false when the parameter less constructor was used, or the external input stream pointer was set either by the constructor
     *  or by \ref setInputStream()
     * @return
     */
    bool isConfigFileManagedInternally() const {
    	return configFileManagedInternally;
    }

    /** \brief get the structure level of this configuration list
     *
     * The base level of the configuration is 0.
     * A \ref Properties object embedded in the base \ref Properties object is 1.
     * If this sub-structure contains another \ref Properties object that level is 2, and so forth.
     *
     * The structure level governs the indent of sub-structured in configuration printouts
     *
     * @return Structure level
     */
    int getStructLevel () const {
    	return structLevel;
    }

    /** \brief Set the structure level of this configuration list, and all included properties
     *
     * \see getStructLevel for more information about sub-structures and structure levels.
     *
     * @param structLevel
     */
    void setStructLevel (int structLevel);

    /** \brief Read the properties from a file or an input stream
     *
     * Read the properties from a file or an input stream
     *
     * If the configuration file name was set the file is opened in text mode and used as input.
     *
     * If an user input stream was set by \ref setInputStream or by \ref Properties(std::istream *inputStream) it will be
     * used right away as input for the reader.
     *
     * If a configuration file name was set by either \ref setFileName or by
     * \ref Properties(std::string const &configFileName) or by \ref Properties(std::string const &configFileName)
     * an ifstream is opened read-only in text mode and used as input for the reader.
     * After reading the configuration file the ifstream is closed in any case no matter if the configuration was read succesfully
     * or not.
     *
     */
    void readConfiguration();

    /** \brief Search for a property identified by its name
     *
     * @param propertyName Name by which the property is searched.
     * @return Pointer to the property
     * @throws ExceptionPropertyNotFound when the exception does not exist.
     */
    Property const *searchProperty (std::string const &propertyName) const;

    /** \brief Return the Iterator of the first property
     *
     * If there is no property the returned iterator is equal to \ref getListEnd()
     *
     * You can traverse through the list using the increment operator ++ until the iterator reaches the end of the list which is identified by
     * equality to \ref getListEnd()
     *
     * @return Iterator to the first property
     */
    PropertyCIterator getFirstProperty () const {
    	return propertyMap.cbegin();
    }

    /** \brief Return the list end Iterator. This iterator points *past* the actual list.
     *
     * The iterator returned here does *not* point to a valid property!
     * Trying to extract a \ref Property from it will likely lead to segmentation error and crash of the program!
     *
     * @return
     */
    PropertyCIterator getListEnd () const {
    	return propertyMap.cend();
    }

    // Some convenience functions to quickly access property values in one level with default values when not found

    /** \brief Search for a boolean property and return the value, else return default value
     *
     * Searches the property level for a property named \p propertyName. If the property exists
     * return the boolean value of the property.
     * If the property is not boolean the function throws \ref ExceptionWrongPropertyType
     *
     * @param propertyName
     * @param defaultVal
     * @return
	 * @throws ExceptionWrongPropertyType
     */
    bool getPropertyValue(std::string const& propertyName, bool defaultVal = false) const;

    /** \brief Search for a double float property and return the value, else return default value
     *
     * Searches the property level for a property named \p propertyName. If the property exists
     * return the double float value of the property.
     * If the property is not double the function throws \ref ExceptionWrongPropertyType
     *
     * @param propertyName
     * @param defaultVal
     * @return
	 * @throws ExceptionWrongPropertyType
     */
    double getPropertyValue(std::string const& propertyName,double defaultVal = 0.0) const;

    /** \brief Search for a long long integer property and return the value, else return default value
     *
     * Searches the property level for a property named \p propertyName. If the property exists
     * return the long long integer value of the property.
     * If the property is not long long integer the function throws \ref ExceptionWrongPropertyType
     *
     * @param propertyName
     * @param defaultVal
     * @return
	 * @throws ExceptionWrongPropertyType
     */
    long long getPropertyValue(std::string const& propertyName,long long defaultVal = 0L) const;


    /** \brief Search for a string property and return the value, else return the default value
     *
     * Searches the property level for a property named \p propertyName. If the property exists
     * return the string value of the property.
     * The function returns the string value of scalar properties in a level regardless of type.
     * Only when the property is a structure the function throws \ref ExceptionWrongPropertyType
     *
     * @param propertyName
     * @param defaultVal
     * @return
	 * @throws ExceptionWrongPropertyType
     */
    char const * getPropertyValue(std::string const& propertyName,char const * defaultVal = nullptr) const;

    /** \brief Get number of properties on this level of the configuration
     *
     * The number of properties is not cumulative, but is retured per structure level of the configuration
     *
     * @return Number of properties on this level of the configuration
     */
    PropertyMap::size_type numProperties() {
    	return propertyMap.size();
    }

    /** \brief Helper to extract the property from an iterator
     *
     * *Note* If you call this method for an iterator equal to \ref getListEnd() of the configuration where it came from the result is undefined, and may lead
     * to unexpected behavior, in the best case immediate crash.
     *
     * @param it Iterator to extract the property from
     * @return Reference to the property associated with the iterator
     */
    static inline Property const  &getPropertyFromIterator(PropertyCIterator it) {
    	return *(it->second.get());
    }

    /** \brief Insert a new property into this level of the configuration.
     *
     * This methods inserts the property passed in newPropery into the container of this. this takes ownership of the propery passed.
     * It must *not* be deleted by the caller, or it must not be declared as plain object in the code. It *must* be created with the operator new.
     * The container of this will take care to delete the object when this is deleted or becomes invalid.
     * Properties must be unique by name on one structure level in the configuration.
     *
     * @param newProperty Pointer to property to be inserted. this takes ownership of the property.
     * @throws ExceptionPropertyDuplicate when another property with the same name already exists in the configuration on this structure level.
     */
    void addProperty (Property *newProperty);

    /** \brief Delete a propery in the current level of the configuration.
     *
     * If the property does not exist nothing happens. No exception is thrown.
     *
     * The property to be removed is only removed from the current structure level of the configuration.
     * A property with the name on a lower or higher level will not be affected.
     *
     * @param propertyName Name of the property to be deleted from the configuration
     */
    void deletePropery (std::string const &propertyName);

    /** \brief Return reference to the internal map of properties \ref Property
     *
     * This is a function for insiders to gain direct access to the internal map of properties.
     * A lot harder to program but gives access to the full functionality of std::map.
     * Type checking ensures that you cannot really break anything seriously.
     *
     * Use it at your own peril.
     *
     * @return Reference to the internal std::map \ref PropertyMap containing \ref Property
     */
    PropertyMap &getPropertyMap() {
    	return propertyMap;
    }

    /** \brief Return reference to the constant internal map of properties \ref Property
     *
     * This is a function for insiders to gain direct access to the internal map of properties.
     * A lot harder to program but gives access to the full functionality of const std::map.
     * Type checking ensures that you cannot really break anything seriously.
     *
     * Use it at your own peril.
     *
     * @return Reference to the constant internal std::map \ref PropertyMap containing \ref Property
     */
    PropertyMap const &getCPropertyMap() const{
    	return propertyMap;
    }


	/** \brief Helper for std::ostream &operator << (std::ostream &os,const Properties4CXX::Properties &properties)
	 *
	 * @param os Output stream
	 * @return Reference to <a href="http://en.cppreference.com/w/cpp/io/basic_ostream" >std::ostream</a>
	 */
	std::ostream &writeOut (std::ostream &os) const;

	/** \brief Function to fill the scanner buffer from the configuration input stream
	 *
	 * This function is for the use of the internal scanner only.
	 * It is invoked within the Flex scanner macro YY_INPUT.
	 *
	 * @param buf String buffer
	 * @param max_size Maximum space in the buffer
	 * @return Number of characters read. 0 means the end of the stream is reached.
	 */
	int readConfigIntoBuffer (char* buf, size_t max_size);


private:

    int structLevel = 0;

    /// \brief Name of configuration file. Input stream is handled internally
	std::string configFileName;

	/// \brief Flag if the input stream is handled internally. The file name is given by \ref configFileName.
	/// The internal input file stream \ref inputFileStream is used as inut.
	bool configFileManagedInternally = true;

	/** \brief External input stream. If this pointer is set also configFileManagedInternally is set true
	 *
	 * The input stream must be managed by the owner of the pointer including opening it before calling \ref readConfiguration()
	 * and closing afterwards.
	 * The stream can also be a stringstream or any other kind of subclass of std::istream.
	 */
	std::istream *inputStream = 0;

	/** \brief Internally managed input file stream
	 *
	 * It is used when the configuration file name is defined either by the constructor or \ref setFileName
	 * Then \ref configFileManagedInternally is set true.
	 */
	std::ifstream inputFileStream;

	/// \brief std::map containing all properties. Key is std::string containing the property name.
	PropertyMap propertyMap;

};

// A few conversion helpers for the scanner

/** \brief Convert a decimal integer value into a long long.
 *
 * The form is guaranteed by the scanner to be:
 * [+-][1-9][0-9]*, i.e. 1234 or +234 or -20994
 *
 * @param str
 * @return converted value as long long (int)
 */
long long strToLL (char const *str);

/** \brief Convert a octal integer value into a long long.
 *
 * The form is guaranteed by the scanner to be:
 * 0[0-7]*, i.e. 0 or 0234. 000345 is also valid
 *
 * @param str
 * @return converted value as long long (int)
 */
long long strOctToLL (char const *str);

/** \brief Convert a binary integer value into a long long.
 *
 * The form is guaranteed by the scanner to be:
 * 0[bB][01]+, i.e. 0b101001 or 0B001001101
 *
 * @param str
 * @return converted value as long long (int)
 */
long long strBinToLL (char const *str);

/** \brief Convert a hexadecimal integer value into a long long.
 *
 * The form is guaranteed by the scanner to be:
 * 0[xX][0-9a-fA-F]+, i.e. 0x123abc or 0XABDE or 0xAbcDeF .0x0 is also valid
 *
 * @param str
 * @return converted value as long long (int)
 */
long long strHexToLL (char const *str);

/** \brief Convert a float number into long double.
 *
 * The form is guaranteed by the scanner to be:
 *
 * First form of a double: Pure integer with an exponent is a double
 * Floats like 1e10, 1e-5L, +1e+10, -1e-5
 *
 * Second form: Digit sequence with a dot '.'. Exponent optional
 * Floats like 1., 1.e-2, -1., +1.e-2,  -1.e+2
 *
 * Third form: Decimal digits after the dot '.'.
 * Digits before the dot and the Exponent are optional.
 * Floats like 3.14, -.1, +0.1e-1
 *
 * @param str the numeric string
 * @return converted value as long double
 */
long double strToLD (char const *str);

/** Convert a double value to a std::string
 * independent from the locale
 *
 * This function implements a string conversion that is independent from the
 * locale dependent decimal separator (e.g. '.' or '.') but always uses '.'.
 */
std::string dToStr (double val);

}; // namespace Properties4CXX {

/** \brief Output stream operator for \ref Properties4CXX::Properties objects.
 *
 *
 * @param os Output stream
 * @param properties \ref Properties4CXX::Properties to be printed/streamed.
 * @return Reference to [std::ostream](http://en.cppreference.com/w/cpp/io/basic_ostream)
 */
static inline std::ostream &operator << (std::ostream &os,const Properties4CXX::Properties &properties) {
	properties.writeOut(os);

	return os;
}

#endif /* #ifndefINCLUDE_PROPERTIES4CXX_PROPERTies_H_ */
