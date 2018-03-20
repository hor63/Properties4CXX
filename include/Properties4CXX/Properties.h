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

#ifndef PROPERTIES4CXX_H_
#define PROPERTIES4CXX_H_

#include <memory>
#include <sstream>
#include <istream>
#include <map>
#include <string>


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




namespace Properties4CXX {

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
 * The locale is always "C", i.e. the decimal speparator is always the dot '.'. Hundred separators are not allowed.
 *
 * Thus a number in German locale 123,456 is not valid. Neither is a number 123,456.2323 valid input.
 *
 *
 */
class Properties {
public:

	/**
	 * Constructor. Before reading a configuration you must either set the configuration file name,
	 * or set the input stream
	 */
	PROPERTIES4CXX_PUBLIC
    Properties ();

    /** \brief Constructor defining the input file name
     * Constructor defining the input file name.
     * The file will be opened not earlier when you call \ref readConfiguration
     *
     * @param configFileName: Name of the configuration file. Can either be an absolute or relative path. The path
     * specification must comply with the OS conventions
     *
     */
	PROPERTIES4CXX_PUBLIC
    Properties (char const *configFileName);

    /** \brief Constructor defining the input file name
     * Constructor defining the input file name.
     * The file will be opened not earlier when you call \ref readConfiguration
     *
     * @param configFileName Name of the configuration file. Can either be an absolute or relative path. The path
     * specification must comply with the OS conventions.
     *
     */
	PROPERTIES4CXX_PUBLIC
    Properties (std::string const &configFileName);

    /** \brief Constructor defining the input stream
     *
     * @param inputStream Pointer to an input stream (typically an istringstream, i.e. reading out of a memory buffer.
     * \see setInputStream
     *
     */
	PROPERTIES4CXX_PUBLIC
    Properties (std::istream *inputStream);

    /**
     * The destructor does not care for the inputstream set either by \ref setInputStream or by \ref Properties(std::istream *inputStream)
     * The owner of the inputStream object must close and release it herself.
     */
	PROPERTIES4CXX_PUBLIC
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
	PROPERTIES4CXX_PUBLIC
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
	PROPERTIES4CXX_PUBLIC
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
	PROPERTIES4CXX_PUBLIC
    void setInputStream (std::istream *inputStream);

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
	PROPERTIES4CXX_PUBLIC
    void readConfiguration();


};

}; // namespace Properties4CXX {

#endif /* #ifndef PROPERTIES4CXX_H_ */
