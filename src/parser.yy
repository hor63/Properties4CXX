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



// Sei Schwatzhaft mit Fehlermeldungen
#define YYERROR_VERBOSE

%}

/* ------------------------------------------------------------------------- */
/* --  The parser declaration section  ------------------------------------- */
/* ------------------------------------------------------------------------- */

%start properties

%union {
char                 chr;
char*                string;
double               numVal;
}


%token <string>           LEX_IDENTIFIER
%token <string>           LEX_STRING
%token <chr>              LEX_CHAR
%token <numVal>           LEX_DOUBLE
%token <numVal>           LEX_INTEGER


/* parenthesis, separators etc. */
%token             LEX_ILLCHAR
%token             LEX_BRACKETOPEN
%token             LEX_BRACKETCLOSE
%token             LEX_COMMA
%token             LEX_ASSIGN
%token  <string>   LEX_END_OF_LINE



/* The types of the the rules return */
%type  <string>        property


%%
/* ------------------------------------------------------------------------- */
/* --  The rule section  --------------------------------------------------- */
/* ------------------------------------------------------------------------- */

properties : property 
 | properties property;

property : LEX_END_OF_LINE
    { $$ = $1; }


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


  return 0;
}



// --------------------------------------------------------------------------
