%{
/* ------------------------------------------------------------------------- */
/* --  The C interface and declaration section  ---------------------------- */
/* ------------------------------------------------------------------------- */

#include <iostream>
#include <sstream>
#include <string>

#include "parser.hh"
#include "lexer.h"

long  yy_curr_line   = 1;
long  yy_curr_column = 0;


/**********************************************************/
/* Function prototypes */

int yyerror (void *scanner, const char* parseMsg);

// Be chatty at error messages
#define YYERROR_VERBOSE

%}

/* ------------------------------------------------------------------------- */
/* --  The parser declaration section  ------------------------------------- */
/* ------------------------------------------------------------------------- */

%define api.pure full
%lex-param {void *scanner}
%parse-param {void *scanner}

%start properties

%union {
char*	string;
double	numVal;
long	intVal;
bool    boolVal;
}


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
%type <string>		stringVal
%type <string>		property
%type <string>		simpleProperty
%type <numVal>		numProperty
%type <intVal>		intProperty
%type <intVal>		boolProperty


%%
/* ------------------------------------------------------------------------- */
/* --  The rule section  --------------------------------------------------- */
/* ------------------------------------------------------------------------- */

singleProperty : simpleProperty | numProperty | intProperty | boolProperty | propertyList | propertyStruct
	;

properties : emptyLine | singleProperty 
	| properties emptyLine
	| properties singleProperty
	;

emptyLine : LEX_END_OF_LINE
	;
 
stringVal : LEX_IDENTIFIER | LEX_STRING
		{ $$ = $1; }
	;
 
simpleProperty : property LEX_END_OF_LINE 
		{ $$ = $1; }
	;

propertyList : propertyListList LEX_END_OF_LINE;
		
propertyListList : property LEX_COMMA stringVal
	| propertyListList LEX_COMMA stringVal
	;
		
property : LEX_IDENTIFIER LEX_ASSIGN stringVal LEX_END_OF_LINE
		{ $$ = $3; }
	;

numProperty : LEX_IDENTIFIER LEX_ASSIGN LEX_DOUBLE LEX_END_OF_LINE
		{ $$ = $3; }
	;

intProperty : LEX_IDENTIFIER LEX_ASSIGN LEX_INTEGER LEX_END_OF_LINE
		{ $$ = $3; }
	;

boolProperty : LEX_IDENTIFIER LEX_ASSIGN LEX_BOOL LEX_END_OF_LINE
		{ $$ = $3; }
	;

propertyStruct : LEX_IDENTIFIER LEX_ASSIGN LEX_BRACKETOPEN propertyList LEX_BRACKETCLOSE
	;
	
%%
/* ------------------------------------------------------------------------- */
/* --  The function section  ----------------------------------------------- */
/* ------------------------------------------------------------------------- */


using namespace std;

int yyerror (void *scanner, const char* parseMsg)
{

  cerr << "Parse error in line " << yy_curr_line
    << " in column " << yy_curr_column << " is \"" << parseMsg << "\"" << endl;

  ostringstream strLine;
  ostringstream strCol;
  string lineStr;
  string colStr;

  strLine << yy_curr_line   << ends;
  strCol << yy_curr_column << ends;

  lineStr = strLine.str();
  colStr = strCol.str();


  const char* strArray [] = {
    (const char*)parseMsg,
    lineStr.c_str(),
    colStr.c_str(),
    yyget_text(scanner)
    };

  return 0;
}



// --------------------------------------------------------------------------
