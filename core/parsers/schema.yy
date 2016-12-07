%skeleton "lalr1.cc"
%require  "3.0"
%debug 
%defines 
%define api.namespace {com::wookler::reactfs::core::parsers}
%define parser_class_name {schema_parser}

%code requires{
   #include <iostream>
   #include <cstdlib>
   #include <fstream>
   
   /* include for all driver functions */
   #include "core/includes/types/type_defs.h"
   #include "core/includes/types/type_errors.h"
   #include "schema.h"

   using namespace std;
	namespace com {
		namespace wookler {
			namespace reactfs {
				namespace core {
					namespace parsers {
						class schema_driver;
						class schema_scanner;
					}
				}
			}
		}
	}
	// The following definitions is missing when %locations isn't used
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

}

%code {
#include "schema_driver.h"

#undef yylex
#define yylex scanner.yylex
}

%parse-param { schema_scanner  &scanner  }
%parse-param { schema_driver  &driver  }


%define parse.assert
/* %define api.token.prefix {TOK_} */

%union {
	long lval;
	double dval;
	char* str;
}

%token 			END	0	"end of file"
%token 			SVALUE
%token 			VARNAME
%token			IVALUE
%token			DVALUE
%left 			TYPE		
%left 			DATATYPE		
%left			BYTE	
%left			CHAR
%left			BOOL
%left			SHORT
%left			INTEGER
%left			LONG
%left			FLOAT
%left			DOUBLE
%left			TIMESTAMP
%left			DATETIME
%left			STRING
%left			TEXT
%left			ARRAY
%left			LIST
%left			MAP
%left 			LTYPEBRACE
%left			RTYPEBRACE
%left			LINBRACE
%left			RINBRACE
%left			LSZBRACE
%left			RSZBRACE
%left			COMMA
%left			DEFAULT
%left			REF
%left			TYPE_END
%left			PRIMARY_KEY
%left			INDEX	
%left			SCHEMA
%left			CONSTRAINT
%left			REGEX	
%left			IN
%left			BETWEEN
%left			NOT
%left			LT
%left			GT
%left			COLON
%left			SEMICOLON
%left 			NEWLINE
%type<dval>		DVALUE
%type<lval>		IVALUE
%type<str>		SVALUE VARNAME STRING DOUBLE BYTE CHAR BOOL SHORT INTEGER LONG FLOAT TIMESTAMP DATETIME  TEXT ARRAY LIST MAP
%type<str>		value values datatype


%locations

%%


type:
		TYPE VARNAME LTYPEBRACE declarations RTYPEBRACE SEMICOLON	{ 
											std::string ss($2);
											driver.add_type(ss); 
										};
declarations:
		declare				
	|	declarations COMMA declare	
	;


declare:
		datatype VARNAME		{ 
							std::string t($2);
							std::string n($1);
							driver.add_declaration(t, n); 
						}							
	|	DATATYPE VARNAME VARNAME	{ 
							std::string t($3);
							std::string n($2);
							driver.add_declaration(t, n); 
						}	
	;


datatype:
		DOUBLE		
	|	BYTE		
	|	CHAR	
	|	BOOL
	|	SHORT
	|	INTEGER
	|	LONG
	|	FLOAT
	|	TIMESTAMP
	|	DATETIME
	|	STRING
	|	TEXT
	|	ARRAY
	|	LIST
	|	MAP
	;

/*

constraint:
	CONSTRAINT LINBRACE constraint_type RINBRACE		{ driver.create_constraint(); };
	

constraint_type:
		REGEX COLON STRING				{ driver.set_constraint(false, "REGEX", $3);}
	|	NOT REGEX COLON STRING				{ driver.set_constraint(true, "REGEX", $4);}
	|	IN LSZBRACE values RSZBRACE 			{ driver.set_constraint(false, "IN", $3);} 
	|	NOT IN LSZBRACE values RSZBRACE 		{ driver.set_constraint(true, "IN", $4);}
	|	BETWEEN LSZBRACE values RSZBRACE 		{ driver.set_constraint(false, "BETWEEN", $3);}
	|	NOT BETWEEN LSZBRACE values RSZBRACE 		{ driver.set_constraint(true, "BETWEEN", $4);}
	|	LT LSZBRACE value RSZBRACE 			{ driver.set_constraint(false, "LT", $3);}
	|	NOT LT LSZBRACE value RSZBRACE 			{ driver.set_constraint(true, "LT", $4);}
	|	GT LSZBRACE value RSZBRACE 			{ driver.set_constraint(false, "GT", $3);}
	|	NOT GT LSZBRACE value RSZBRACE 			{ driver.set_constraint(true, "GT", $4);}
	;

*/

values:
		value 			{ $$ = $1; }	
	|	values COMMA value	{ 
						$1.append(", "); 
						$1.append($3);
						$$ = $1;
					}
	;


value:
		IVALUE 		{ $$ = to_string($1); }
	| 	DVALUE 		{ $$ = to_string($1); }
	| 	SVALUE		{ $$ = $1; }
	;


%%

void com::wookler::reactfs::core::parsers::schema_parser::error( const location_type &l, const std::string &err_message ) {
	throw TYPE_PARSER_ERROR("Parser error occurred. [error=%s][location=%s]", err_message.c_str(), l);
}

