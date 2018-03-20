
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
#include "parser.hh"


static void yy_countlines (char const* text);
static char *scanQuotedString (char const *quotedText);

extern long  yy_curr_line;
extern long  yy_curr_column;


%}

/* Important to support UTF-8 */
%option 8bit 
/* Support fully reentrant parser and scanner */
%option reentrant bison-bridge
%option header-file="lexer.h"

/* ------------------------------------------------------------------------- */
/* --  Some useful abbreviations  ----------------------------------------- */
/* ------------------------------------------------------------------------- */

hexdig   [0-9a-fA-F]
decnum   ([1-9][0-9]*)
hexnum   (0[xX]{hexdig}+)
exp      ([eE][+-]?([0-9]+("."[0-9]*)?)|("."[0-9]+))


/* ---------------------------------------------------------------<string>---------- */
/* --  The rule section  --------------------------------------------------- */
/* ------------------------------------------------------------------------- */
%%




 /* CR-LF according to Windows and DOS custom */
\r\n    {
             yy_curr_line++;
             yy_curr_column = 0;
             
             return LEX_END_OF_LINE;
         }

 /* LF-CR reverse to Windows custom. Unusual, but who knows :) */
\n\r    {
             yy_curr_line++;
             yy_curr_column = 0;
             
             return LEX_END_OF_LINE;
         }

 /* Single LF as in UNIX, Linux, and text mode I/O channels in C and C++ */
\n       {
             yy_curr_line++;
             yy_curr_column = 0;
             
             return LEX_END_OF_LINE;
         }

 /* Single CR like some Windows multi-line edits return */
\r     {
             yy_curr_line++;
             yy_curr_column = 0;
             
             return LEX_END_OF_LINE;
         }

"{"		{
			yy_curr_column++;
			return LEX_BRACKETOPEN;
		}
		 
"}"		{
			yy_curr_column++;
			return LEX_BRACKETOPEN;
		}
		 
","		{
			yy_curr_column++;
			return LEX_COMMA;
		}
		 
"="		{
			yy_curr_column++;
			return LEX_ASSIGN;
		}

                                        
[ \t\xc]    { /* Blank, Tab, Form feed */
             yy_curr_column += strlen(yytext);
            }


[+-]?{decnum}                         { /* Simple integer */
                                        yylval->numVal = strtol((char *)(yytext),NULL,0);
                                        yy_curr_column += strlen(yytext);
                                        return (LEX_INTEGER); 
                                      }


0[0-7]*                               { /* Octal number (incl. 0) */
                                        yylval->numVal = strtoul((char *)(yytext),NULL,0);
                                        yy_curr_column += strlen(yytext);
                                        return (LEX_INTEGER); 
                                      }


{hexnum}                              { /* hexadecimal number */
                                        yylval->numVal = strtoul((char *)(yytext),NULL,0);
                                        yy_curr_column += strlen(yytext);
                                        return (LEX_INTEGER); 
                                      }


[+-]?[0-9]+"."[0-9]*                  { /* floats like -123. or 123.23 */
                                        yylval->numVal = strtod((char *)(yytext),NULL);
                                        yy_curr_column += strlen(yytext);
                                        return (LEX_DOUBLE); 
                                      }


[+-]?[0-9]+("."[0-9]*)?{exp}          { /* floats like -123E12 or 123.23e.2 or +023E-1.1 */
                                        yylval->numVal = strtod((char *)(yytext),NULL);
                                        yy_curr_column += strlen(yytext);
                                        return (LEX_DOUBLE); 
                                      }


[+-]?"."[0-9]+{exp}?                  { /* floats like -.123 or .23e.2 or +.023E-1.1 */
                                        yylval->numVal = strtod((char *)(yytext),NULL);
                                        yy_curr_column += strlen(yytext);
                                        return (LEX_DOUBLE); 
                                      }


([yY][eE][sS])|([tT][rR][uU][eE])|([oO][nN]) { /* yes, true, on case insensitive */
                                        yylval->boolVal = true;
                                        yy_curr_column += strlen(yytext);
                                        return (LEX_BOOL); 
                                      }


([nN][oO])|([fF][aA][lL][sS][eE])|([oO][fF][fF]) { /* no, false, off case insensitive */
                                        yylval->boolVal = false;
                                        yy_curr_column += strlen(yytext);
                                        return (LEX_BOOL); 
                                      }



\"([^\"]|(\\\"))*\"                    { /* A quoted string */
                                        yylval->string = new char[strlen(yytext) + 1];
                                        strcpy(yylval->string, yytext+1);

                                        /* Hack den " am Ende ab. */
                                        yylval->string[strlen(yylval->string) - 1] = '\0';

										yy_countlines (yytext);

                                        return (LEX_STRING); 
                                       }




[^\r\n \xc\t\"\{\},=]+	              {
                                        yylval->string = new char[strlen(yytext)+1];
                                        strcpy(yylval->string, yytext);
                                        yy_curr_column += strlen(yytext);
                                        return (LEX_IDENTIFIER);
                                       }

									   
%%
 /* ------------------------------------------------------------------------- */
 /* --  The function section  ----------------------------------------------- */
 /* ------------------------------------------------------------------------- */


static void yy_countlines (char const* text)
{

   while (*text)
      {
      if (*text == '\n')
         {
         yy_curr_line ++;
         yy_curr_column = 0;
         }
      else {
         yy_curr_column ++;
      }
      text ++;
      }
   
}

static char *scanQuotedString (char const *quotedText) {
char* outString = new char[strlen(quotedText)];
int i, k=0;
char c;
   
    // Run through the string from the 1st character, i.e. leave the initial double-quote out
	for (i=1; *quotedText != '"'; i++) {
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
		  k++;
		}
		
	    quotedText ++;
	} 


}
