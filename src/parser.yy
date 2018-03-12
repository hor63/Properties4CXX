%{
/* ------------------------------------------------------------------------- */
/* --  The C interface and declaration section  ---------------------------- */
/* ------------------------------------------------------------------------- */

#include <iostream>
#include <strstream>

long  yy_curr_line   = 1;
long  yy_curr_column = 0;


/**********************************************************/
/* Function prototypes */

/* Output of parse errors */
int yyerror (const char* parseMsg);

/* The lexical scanner */
int yylex(void);

extern char* yytext;

// Routinen fuer Pufferverwaltung des Scanners, damit der aus dem String
// scannt.
void yy_setup_string_buffer (char* str);
void yy_free_string_buffer ();



// Be chatty at error messages
#define YYERROR_VERBOSE

%}

/* ------------------------------------------------------------------------- */
/* --  The parser declaration section  ------------------------------------- */
/* ------------------------------------------------------------------------- */

%start properties

%union {
char*	string;
double	numVal;
long	intVal;
}


%token <string>           LEX_IDENTIFIER
%token <string>           LEX_STRING
%token <numVal>           LEX_DOUBLE
%token <intVal>           LEX_INTEGER


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


%%
/* ------------------------------------------------------------------------- */
/* --  The rule section  --------------------------------------------------- */
/* ------------------------------------------------------------------------- */

properties : emptyLine | simpleProperty | numProperty | intProperty | propertyList | propertyStruct
	| properties emptyLine
	| properties simpleProperty
	| properties numProperty
	| properties intProperty
	| properties propertyList
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

propertyStruct : LEX_IDENTIFIER LEX_ASSIGN LEX_BRACKETOPEN propertyList LEX_BRACKETCLOSE
	;
	
%%
/* ------------------------------------------------------------------------- */
/* --  The function section  ----------------------------------------------- */
/* ------------------------------------------------------------------------- */


using namespace std;

int yyerror (const char* parseMsg)
{

  cerr << "Parse error in line " << yy_curr_line
    << " in column " << yy_curr_column << " is \"" << parseMsg << "\"" << endl;

  ostrstream strLine;
  ostrstream strCol;

  strLine << yy_curr_line   << ends;
  strCol << yy_curr_column << ends;


  const char* strArray [] = {
    (const char*)parseMsg,
    strLine.str(),
    strCol.str(),
    yytext
    };

	strLine.freeze(false);
	strCol.freeze(false);

  return 0;
}



// --------------------------------------------------------------------------
