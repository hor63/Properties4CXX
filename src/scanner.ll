
%{
/*
 * scanner.ll
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

#include <cstring>
#include <cstdlib>
#include <istream>

#include "parserTypes.h"
#include "Properties4CXX/Properties.h"
#include "Properties4CXX/Property.h"


#include "parser.hh"

static void yy_countlines (char const* text, yyscan_t yyscanner);
static char *scanQuotedString (char const *quotedText);

// Overwrite the input macro to read from the configuration input stream.
#if defined YY_INPUT
#undef YY_INPUT
#endif /* if defined YY_INPUT */
#define YY_INPUT(buf,result,max_size) { \
	result = yyget_extra(yyscanner)->readConfigIntoBuffer (buf, max_size); \
	} \





%}

/* Important to support UTF-8 */
%option 8bit
/* Support fully reentrant parser and scanner */
%option reentrant bison-bridge
%option header-file="lexer.h"
/* When the input stream ends, it ends... */
%option noyywrap

%option extra-type="Properties4CXX::Properties *"

/* %option debug */
%option verbose

/* ------------------------------------------------------------------------- */
/* --  Some useful abbreviations  ----------------------------------------- */
/* ------------------------------------------------------------------------- */

hexdig   [0-9a-fA-F]
decnum   ([1-9][0-9]*)
hexnum   (0[xX]{hexdig}+)
exp      ([eE][+-]?[0-9]+)


/* ---------------------------------------------------------------<string>---------- */
/* --  The rule section  --------------------------------------------------- */
/* ------------------------------------------------------------------------- */
%%



"#"[^\n\r]* { /* Line comments are swallowed by the scanner */
			  yyset_column ( yyget_column(yyscanner) + strlen(yytext),yyscanner);
			}

\r\n    {  /* CR-LF according to Windows and DOS custom */
             yyset_lineno(yyget_lineno(yyscanner)+1,yyscanner);
             yyset_column ( 0,yyscanner);

             return LEX_END_OF_LINE;
         }


\n\r    { /* LF-CR reverse to Windows custom. Unusual, but who knows :) */
             yyset_lineno(yyget_lineno(yyscanner)+1,yyscanner);
             yyset_column ( 0,yyscanner);

             return LEX_END_OF_LINE;
         }


\n       { /* Single LF as in UNIX, Linux, and text mode I/O channels in C and C++ */
             yyset_lineno(yyget_lineno(yyscanner)+1,yyscanner);
             yyset_column ( 0,yyscanner);

             return LEX_END_OF_LINE;
         }


\r     { /* Single CR like some Windows multi-line edits return */
             yyset_lineno(yyget_lineno(yyscanner)+1,yyscanner);
             yyset_column ( 0,yyscanner);

             return LEX_END_OF_LINE;
         }

"{"		{
            yyset_column ( yyget_column(yyscanner) + 1,yyscanner);
			return LEX_BRACKETOPEN;
		}

"}"		{
            yyset_column ( yyget_column(yyscanner) + 1,yyscanner);
			return LEX_BRACKETCLOSE;
		}

","		{
            yyset_column ( yyget_column(yyscanner) + 1,yyscanner);
			return LEX_COMMA;
		}

"="		{
            yyset_column ( yyget_column(yyscanner) + 1,yyscanner);
			return LEX_ASSIGN;
		}


[ \t\xc]    { /* Blank, Tab, Form feed are swallowed in the scanner */
             yyset_column ( yyget_column(yyscanner) + strlen(yytext),yyscanner);

            }


[+-]?{decnum}                         { /* Simple integer */
                                        yylval->intVal = new tIntVal;
                                        yylval->intVal->intVal = Properties4CXX::strToLL(yytext);
                                        yylval->intVal->intStr = new char[strlen (yytext) + 1];
                                        strcpy(yylval->intVal->intStr ,yytext);
                                        yyset_column ( yyget_column(yyscanner) + strlen(yytext),yyscanner);
                                        return (LEX_INTEGER);
                                      }


0[0-7]*                               { /* Octal number (incl. 0) */
                                        yylval->intVal = new tIntVal;
                                        yylval->intVal->intVal = Properties4CXX::strOctToLL(yytext);
                                        yylval->intVal->intStr = new char[strlen (yytext) + 1];
                                        strcpy(yylval->intVal->intStr ,yytext);
                                        yyset_column ( yyget_column(yyscanner) + strlen(yytext),yyscanner);
                                        return (LEX_INTEGER);
                                      }

0[bB][01]+                               { /* Binary number */
                                        yylval->intVal = new tIntVal;
                                        yylval->intVal->intVal = Properties4CXX::strBinToLL(yytext);
                                        yylval->intVal->intStr = new char[strlen (yytext) + 1];
                                        strcpy(yylval->intVal->intStr ,yytext);
                                        yyset_column ( yyget_column(yyscanner) + strlen(yytext),yyscanner);
                                        return (LEX_INTEGER);
                                      }


{hexnum}                              { /* hexadecimal number */
                                        yylval->intVal = new tIntVal;
                                        yylval->intVal->intVal = Properties4CXX::strHexToLL(yytext);
                                        yylval->intVal->intStr = new char[strlen (yytext) + 1];
                                        strcpy(yylval->intVal->intStr ,yytext);
                                        yyset_column ( yyget_column(yyscanner) + strlen(yytext),yyscanner);
                                        return (LEX_INTEGER);
                                      }



[+-]?[0-9]+{exp}	                  { /* First form of a double: Pure integer with an exponent is a double
										 * Floats like 1e10, 1e-5L, +1e+10, -1e-5
										 */
                                        yylval->numVal = new tNumVal;
                                        yylval->numVal->numVal = Properties4CXX::strToLD(yytext);
                                        yylval->numVal->numStr = new char[strlen (yytext) + 1];
                                        strcpy(yylval->numVal->numStr ,yytext);
                                        yyset_column ( yyget_column(yyscanner) + strlen(yytext),yyscanner);
                                        return (LEX_DOUBLE);
                                      }


[+-]?[0-9]+"."{exp}?         		 { /* Second form: Digit sequence with a dot '.'. Exponent optional
										* floats like -123E12 or 123.23e.2 or +023E-1.1
									    */
                                        yylval->numVal = new tNumVal;
                                        yylval->numVal->numVal = Properties4CXX::strToLD(yytext);
                                        yylval->numVal->numStr = new char[strlen (yytext) + 1];
                                        strcpy(yylval->numVal->numStr ,yytext);
                                        yyset_column ( yyget_column(yyscanner) + strlen(yytext),yyscanner);
                                        return (LEX_DOUBLE);
                                      }


[+-]?[0-9]*"."[0-9]+{exp}?            { /* Third form: Decimal digits after the dot '.'.
										 * Digits before the dot and Exponent are optional.
										 * floats like 3.14, -.1, +0.1e-1
									    */
                                        yylval->numVal = new tNumVal;
                                        yylval->numVal->numVal = Properties4CXX::strToLD(yytext);
                                        yylval->numVal->numStr = new char[strlen (yytext) + 1];
                                        strcpy(yylval->numVal->numStr ,yytext);
                                        yyset_column ( yyget_column(yyscanner) + strlen(yytext),yyscanner);
                                        return (LEX_DOUBLE);
                                      }


([yY][eE][sS])|([tT][rR][uU][eE])|([oO][nN]) { /* yes, true, on case insensitive */
                                        yylval->boolVal = new tBoolVal;
                                        yylval->boolVal->boolVal = true;
                                        yylval->boolVal->boolStr = new char[strlen (yytext) + 1];
                                        strcpy(yylval->boolVal->boolStr ,yytext);
                                        yyset_column ( yyget_column(yyscanner) + strlen(yytext),yyscanner);
                                        return (LEX_BOOL);
                                      }


([nN][oO])|([fF][aA][lL][sS][eE])|([oO][fF][fF]) { /* no, false, off case insensitive */
                                        yylval->boolVal = new tBoolVal;
                                        yylval->boolVal->boolVal = false;
                                        yylval->boolVal->boolStr = new char[strlen (yytext) + 1];
                                        strcpy(yylval->boolVal->boolStr ,yytext);
                                        yyset_column ( yyget_column(yyscanner) + strlen(yytext),yyscanner);
                                        return (LEX_BOOL);
                                      }


\"([^\"]|(\\\"))*\"                    { /* A quoted string */
                                        yylval->string = new tStrVal;
                                        yylval->string->str = scanQuotedString(yytext);
                                        yylval->string->isQuotedString = true;
										yy_countlines (yytext,yyscanner);

                                        return (LEX_STRING);
                                       }




[^\r\n \xc\t\"\{\},=]+	              {
                                        yylval->string = new tStrVal;
                                        yylval->string->str = new char[strlen(yytext)+1];
                                        strcpy(yylval->string->str, yytext);
                                        yylval->string->isQuotedString = false;
                                        yyset_column ( yyget_column(yyscanner) + strlen(yytext),yyscanner);
                                        return (LEX_IDENTIFIER);
                                       }


%%
 /* ------------------------------------------------------------------------- */
 /* --  The function section  ----------------------------------------------- */
 /* ------------------------------------------------------------------------- */


static void yy_countlines (char const* text, yyscan_t yyscanner)
{

   while (*text) {
      if (*text == '\n') {
         yyset_lineno(yyget_lineno(yyscanner)+1,yyscanner);
         yyset_column ( 0,yyscanner);
         // \n\r is one newline
         if (*(text+1) == '\r') {
            text++;
         } // if (*(text+1) == '\r') {
      } else { // if (*text == '\n') {
	      if (*text == '\r') {
	         yyset_lineno(yyget_lineno(yyscanner)+1,yyscanner);
	         yyset_column ( 0,yyscanner);
	         // \r\n is one newline
	         if (*(text+1) == '\n') {
	            text++;
	         } // if (*(text+1) == '\n') {
	      } else { // else if (*text == '\r') {
	           yyset_column ( yyget_column(yyscanner) + 1,yyscanner);
	      }
	   } // else if (*text == '\n') {
      text ++;
   } // while

}

static char *scanQuotedString (char const *quotedText) {
char* outString = new char[strlen(quotedText)];
int k=0;
char c;

    // Run through the string from the 2nd character, i.e. leave the initial double-quote out
	for (quotedText++; *quotedText != '"'; quotedText ++) {
		if (*quotedText == '\\') {
		  // Here is a masked character
		  quotedText++;
		  switch (*quotedText) {

		  case '"' :
		  	 c = '"';
		  	 break;
		  case '\\' :
		  	 c = '\\';
		  	 break;
		  case 'f' :
		  	 c = '\f';
		  	 break;
		  case 'n' :
		  	 c = '\n';
		  	 break;
		  case 'r' :
		  	 c = '\r';
		  	 break;
		  case 't' :
		  	 c = '\t';
		  	 break;
		  case 'v' :
		  	 c = '\v';
		  	 break;

		  default:
			 c = *quotedText;
		  }

		  outString[k] = c;

		} else {
		  // copy the character
		  outString[k] = *quotedText;
		}

		k++;

	}

	outString[k] = '\0';

	return outString;
}
