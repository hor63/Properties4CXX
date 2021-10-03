

%{
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

/* ------------------------------------------------------------------------- */
/* --  The C++ interface and declaration section  ---------------------------- */
/* ------------------------------------------------------------------------- */

#include <iostream>
#include <sstream>
#include <string>

#include "parserTypes.h"
#include "Properties4CXX/Properties.h"
#include "Properties4CXX/Property.h"

#include "parser.hh"
#include "lexer.h"


/***********************/
/* Function prototypes */

void yyerror (void *scanner, Properties4CXX::Properties *props, const char* parseMsg);


%}

/* ------------------------------------------------------------------------- */
/* --  The parser declaration section  ------------------------------------- */
/* ------------------------------------------------------------------------- */

%define parse.error verbose

%define parse.trace
%verbose


%define api.pure full
%lex-param {void *scanner}
%parse-param {void *scanner} {Properties4CXX::Properties *props}

%start topLevelProperties

%union {
tStrVal		*string;
tNumVal		*numVal;
tIntVal		*intVal;
tBoolVal	*boolVal;
Properties4CXX::Property	*property;
Properties4CXX::Properties	*properties;
Properties4CXX::PropertyValueList *propertyValueList;
}

%destructor { delete $$; } <string>
%destructor { delete $$; } <numVal>
%destructor { delete $$; } <intVal>
%destructor { delete $$; } <boolVal>
%destructor { delete $$; } <property>
%destructor { delete $$; } <properties>
%destructor { delete $$; } <propertyValueList>

%token <string>           LEX_IDENTIFIER
%token <string>           LEX_STRING
%token <numVal>           LEX_DOUBLE
%token <intVal>           LEX_INTEGER
%token <boolVal>          LEX_BOOL


/* brackets, separators etc. */
%token             LEX_BRACKETOPEN
%token             LEX_BRACKETCLOSE
%token             LEX_COMMA
%token             LEX_ASSIGN
%token  		   LEX_END_OF_LINE



/* The types of the the rules return */
%type <properties>			properties
%type <property>			singleProperty
%type <property>			stringProperty
%type <property>			numProperty
%type <property>			intProperty
%type <property>			boolProperty
%type <property>			propertyList
%type <property>			propertyStruct
%type <propertyValueList>	propertyListList
%type <string>				stringVal

%%
/* ------------------------------------------------------------------------- */
/* --  The rule section  --------------------------------------------------- */
/* ------------------------------------------------------------------------- */

topLevelProperties : properties { 
	props->getPropertyMap() = $1->getPropertyMap();
	delete $1; 
	} 

properties : emptyLine { $$ = new Properties4CXX::Properties; } 
    | singleProperty 
    { $$ = new Properties4CXX::Properties;
      if ($1) { // Error property returns NULL pointer
      	$$->addProperty($1);
      } }
	| properties emptyLine { $$ = $1; }
	| properties singleProperty 
	{ $$ = $1; 
	  if ($2) { // Error property returns NULL pointer
	    $$->addProperty($2); 
	  } }
	;

emptyLine : LEX_END_OF_LINE
	;

singleProperty : 
	  errorProperty { $$ = 0; /* Pseudo property to continue parsing */ }  
	| stringProperty	{ $$ = $1; } 
	| numProperty		{ $$ = $1; } 
	| intProperty		{ $$ = $1; }
	| boolProperty		{ $$ = $1; }
	| propertyList		{ $$ = $1; }
	| propertyStruct	{ $$ = $1; }
	;

errorProperty :  error 	 LEX_END_OF_LINE
	;

stringProperty : LEX_IDENTIFIER LEX_ASSIGN stringVal LEX_END_OF_LINE
	{ $$ = new Properties4CXX::Property ( $1->str.c_str(),$3->str.c_str(),$3->isQuotedString);
	  delete $1; $1 = 0; delete $3; $3 = 0; }
	;

numProperty : LEX_IDENTIFIER LEX_ASSIGN LEX_DOUBLE LEX_END_OF_LINE
	{ $$ = new Properties4CXX::PropertyDouble ( $1->str.c_str(),$3->numStr.c_str(),$3->numVal); 
	  delete $1; $1 = 0; delete $3; $3 = 0; }
	;

intProperty : LEX_IDENTIFIER LEX_ASSIGN LEX_INTEGER LEX_END_OF_LINE
	{ $$ = new Properties4CXX::PropertyInt ( $1->str.c_str(),$3->intStr.c_str(),$3->intVal);  
	  delete $1; $1 = 0; delete $3; $3 = 0; }
	;

boolProperty : LEX_IDENTIFIER LEX_ASSIGN LEX_BOOL LEX_END_OF_LINE
	{ $$ = new Properties4CXX::PropertyBool ( $1->str.c_str(),$3->boolStr.c_str(),$3->boolVal);   
	  delete $1; $1 = 0; delete $3; $3 = 0; }
	;

propertyList : LEX_IDENTIFIER LEX_ASSIGN propertyListList LEX_END_OF_LINE
	{ $$ = new Properties4CXX::PropertyList ($1->str.c_str(),*$3);
	  delete $1; $1 = 0; delete $3; $3 = 0; }
		
propertyStruct : LEX_IDENTIFIER LEX_ASSIGN LEX_BRACKETOPEN properties LEX_BRACKETCLOSE LEX_END_OF_LINE
	{ $$ = new Properties4CXX::PropertyStruct ($1->str.c_str(),*$4);
	  delete $1; $1 = 0; delete $4; $4 = 0; }
	| LEX_IDENTIFIER LEX_ASSIGN LEX_BRACKETOPEN error LEX_BRACKETCLOSE  LEX_END_OF_LINE
	{ $$ = 0; // erroneous structure
	  delete $1; $1 = 0;
	}
	| LEX_IDENTIFIER LEX_ASSIGN LEX_BRACKETOPEN properties
	{
	    $$ = new Properties4CXX::PropertyStruct ($1->str.c_str(),*$4);
	 	delete $1; $1 = 0; delete $4; $4 = 0; 
		yyerror (scanner, props, "Found opening '{' without closing '}'");
		YYERROR;
	}
	;

stringVal : LEX_IDENTIFIER | LEX_STRING
		{ $$ = $1; }
	;
 
propertyListList : 
	stringVal LEX_COMMA stringVal { 
		$$ = new Properties4CXX::PropertyValueList;
		$$->push_back($1->str);
		delete $1; $1 = 0;
		$$->push_back($3->str);
		delete $3; $3 = 0;
		}
	| propertyListList LEX_COMMA stringVal {
		$$ = $1;
		$$->push_back($3->str);
		delete $3; $3 = 0;
		}
	;
		

	
%%
/* ------------------------------------------------------------------------- */
/* --  The function section  ----------------------------------------------- */
/* ------------------------------------------------------------------------- */


using namespace std;

void yyerror (yyscan_t scanner, Properties4CXX::Properties *props, const char* parseMsg)
{

  cerr << "Parse error in line " << yyget_lineno(scanner)
    << " in column " << yyget_column(scanner) << " is \"" << parseMsg << "\"" << endl;

}



// --------------------------------------------------------------------------
