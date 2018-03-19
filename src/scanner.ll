
%{
/* ------------------------------------------------------------------------- */
/* --  The C interface and declaration section  ---------------------------- */
/* ------------------------------------------------------------------------- */

#include <cstring>
#include <cstdlib>
#include <istream>
#include "parser.hh"


static void yy_countlines (char const* text);

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
decnum   [1-9][0-9]*
hexnum   0[xX]{hexdig}+
exp      [eE][+-]?[0-9]+


/* ---------------------------------------------------------------<string>---------- */
/* --  The rule section  --------------------------------------------------- */
/* ------------------------------------------------------------------------- */
%%




 /* CR-LF wie in DOS-Texten */
\r\n    {
             yy_curr_line++;
             yy_curr_column = 0;
             
             return LEX_END_OF_LINE;
         }

 /* Umgekehrt: Nicht ueblich, aber wer weiss */
\n\r    {
             yy_curr_line++;
             yy_curr_column = 0;
             
             return LEX_END_OF_LINE;
         }

 /* Einfaches Linefeed wie in ANSI-C und UNIX ueblich */
\n       {
             yy_curr_line++;
             yy_curr_column = 0;
             
             return LEX_END_OF_LINE;
         }

 /* einfaches CR wie es in manchen Multiline-Edits verwendet wird */
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

[+-]?{decnum}                              {
                                        yylval->numVal = strtol((char *)(yytext),NULL,0);
                                        yy_curr_column += strlen(yytext);
                                        return (LEX_INTEGER); 
                                      }

0[0-7]*                               {
                                        yylval->numVal = strtoul((char *)(yytext),NULL,0);
                                        yy_curr_column += strlen(yytext);
                                        return (LEX_INTEGER); 
                                      }

{hexnum}                              {
                                        yylval->numVal = strtoul((char *)(yytext),NULL,0);
                                        yy_curr_column += strlen(yytext);
                                        return (LEX_INTEGER); 
                                      }

[+-]?[0-9]*"."[0-9]+                       {
                                        yylval->numVal = strtod((char *)(yytext),NULL);
                                        yy_curr_column += strlen(yytext);
                                        return (LEX_DOUBLE); 
                                      }

[+-]?[0-9]+"."[0-9]*                       {
                                        yylval->numVal = strtod((char *)(yytext),NULL);
                                        yy_curr_column += strlen(yytext);
                                        return (LEX_DOUBLE); 
                                      }

[+-]?[0-9]+("."[0-9]*)?{exp}               {
                                        yylval->numVal = strtod((char *)(yytext),NULL);
                                        yy_curr_column += strlen(yytext);
                                        return (LEX_DOUBLE); 
                                      }

[+-]?"."[0-9]+{exp}                        {
                                        yylval->numVal = strtod((char *)(yytext),NULL);
                                        yy_curr_column += strlen(yytext);
                                        return (LEX_DOUBLE); 
                                      }

[yY][eE][sS]|[tT][rR][uU][eE]|[oO][nN] {
                                        yylval->boolVal = true;
                                        yy_curr_column += strlen(yytext);
                                        return (LEX_BOOL); 
                                      }

[nN][oO]|[fF][aA][lL][sS][eE]|[oO][fF][fF] {
                                        yylval->boolVal = false;
                                        yy_curr_column += strlen(yytext);
                                        return (LEX_BOOL); 
                                      }



\"([^\"]|(\\\"))*\"                    {
                                        yylval->string = new char[strlen(yytext) + 1];
                                        strcpy(yylval->string, yytext+1);

                                        /* Hack den " am Ende ab. */
                                        yylval->string[strlen(yylval->string) - 1] = '\0';

										yy_countlines (yytext);

                                        return (LEX_STRING); 
                                       }




[^\r\n \xc\t\"\{\},=]+	                      {
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
