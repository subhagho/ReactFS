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
   #include "parsers/includes/schema.h"

   #define DEFAULT_BUFFER_SIZE 64

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

void debug_r(const char *s, ...);
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
%right 			SVALUE
%right			IVALUE
%right			DVALUE
%token 			VARNAME
%token 			TYPE		
%token 			DATATYPE		
%token			BYTE	
%token			CHAR
%token			BOOL
%token			SHORT
%token			INTEGER
%token			LONG
%token			FLOAT
%token			DOUBLE
%token			TIMESTAMP
%token			DATETIME
%token			STRING
%token			TEXT
%token			ARRAY
%token			LIST
%token			MAP
%token 			LTYPEBRACE
%token			RTYPEBRACE
%token			LINBRACE
%token			RINBRACE
%token			LSZBRACE
%token			RSZBRACE
%token			LINTYPBRACE
%token			RINTYPBRACE
%token			COMMA
%token			DEFAULT
%token			TYPE_END
%token			KEY_FIELDS
%token			INDEX	
%token			SCHEMA
%left			CONSTRAINT
%token			REGEX	
%token			IN
%token			BETWEEN
%token			NOT
%token			LT
%token			GT
%token			ASC
%token			DESC
%token			COLON
%token 			NEWLINE
%type<dval>		DVALUE
%type<lval>		IVALUE size_def
%type<str>		SVALUE VARNAME STRING DOUBLE BYTE CHAR BOOL SHORT INTEGER LONG FLOAT TIMESTAMP DATETIME  TEXT ARRAY LIST MAP ASC DESC
%type<str>		value values datatype variable opt_sort column columns key_fields sort
%type<str>		declare declare_native declare_ref declare_native_arr declare_ref_arr declare_native_list declare_ref_list declare_netive_map declare_ref_map


%locations

%%

parse:
		opt_types schema
	;

schema:
		schema_declare LTYPEBRACE declarations RTYPEBRACE opt_key_fields declare_finish
	;

schema_declare:	
		SCHEMA variable			{
							std::string ss($2);
							debug_r("new schema [%s]", ss.c_str());
							driver.create_schema(ss);
							FREE_PTR($2);
						}

opt_key_fields:
		/* null */				
	|	key_fields
	;

key_fields:
		KEY_FIELDS LINBRACE columns RINBRACE	{
								debug_r("KEY FIELDS: (%s)", $3);
								std::string keys($3);
								driver.set_primary_key(keys);
								FREE_PTR($3);
							}
	;

columns:
		column				{
							$$ = strdup($1);
						}
	|	columns COMMA column 		{
							int s = strlen($1) + strlen($3) + 3;
							char *p = (char *)malloc(sizeof(char) * s);
							CHECK_ALLOC(p, TYPE_NAME(char));
							memset(p, 0, s);
							sprintf(p, "%s,%s", $1, $3);	
							$$ = p;
							FREE_PTR($1);
							FREE_PTR($3);
							debug_r("COLUMNS [%s]", $$);
							
						}
	;
column:
		variable opt_sort		{
							debug_r("COLUMN [%s] SORT[%s]", $1, $2);
							int s = strlen($1 + 1);
							if (NOT_NULL($2)) {	
								s += strlen($2);
								char *p = (char *)malloc(s * sizeof(char));
								CHECK_ALLOC(p, TYPE_NAME(char));
								memset(p, 0, s);
								sprintf(p, "%s %s", $1, $2);
								FREE_PTR($2);
								$$ = p;
							} else {
								$$ = strdup($1);
							}
							FREE_PTR($1);
						}
	;

opt_sort:
		/* null */			{
							$$ = nullptr;
						}
	|	sort				{ 
							$$ = strdup($1);
							FREE_PTR($1);
						}
	;

sort:
		ASC				{
							$$ = strdup($1);
						}
	|	DESC				{
							$$ = strdup($1);
						}
	;	

opt_types:
		/* null */
	|	types
	;

types:
		type
	|	types type
	;
type:
		type_declare LTYPEBRACE declarations RTYPEBRACE declare_finish { 
											
										};
type_declare:	
		TYPE variable			{
							std::string ss($2);
							debug_r("new type [%s]", ss.c_str());
							driver.add_type(ss); 
							FREE_PTR($2);
						}
declarations:
		declare	 
	|	declarations COMMA declare 
	;


declare:
		declare_native	
	|	declare_ref
	|	declare_native_arr
	|	declare_ref_arr
	|	declare_native_list
	|	declare_ref_list
	|	declare_netive_map
	|	declare_ref_map
	;

declare_native:
		datatype variable opt_constraint opt_default		{ 
							debug_r("[type=%s] varname=%s", $1, $2);
							std::string t($1);
							std::string n($2);
							driver.add_declaration(n, t); 
							FREE_PTR($1);
							FREE_PTR($2);
						}				
	;

declare_ref:
		DATATYPE variable variable opt_constraint opt_default	{ 
							debug_r("[type=%s] varname=%s", $2, $3);
							std::string t($2);
							std::string n($3);
							driver.add_declaration(n, t, true); 
						}
	;

declare_native_arr:
		ARRAY LINTYPBRACE datatype RINTYPBRACE  variable size_def opt_constraint opt_default	{
									debug_r("type=[ARRAY] inner type=%s varname=%s size=%d", $3, $5, $6);
									std::string t($3);
									std::string n($5);
									int s = $6;
									driver.add_array_decl(n, s, t, false);
									FREE_PTR($3);
									FREE_PTR($5);
								}
	;

declare_ref_arr:
		ARRAY LINTYPBRACE DATATYPE variable RINTYPBRACE variable size_def opt_constraint opt_default	{
									debug_r("type=[ARRAY] inner type=%s varname=%s size=%d", $4, $6, $7);
									std::string t($4);
									std::string n($6);
									int s = $7;
									driver.add_array_decl(n, s, t, true);
									FREE_PTR($4);
									FREE_PTR($6);
	
								}
	;

declare_native_list:
		LIST LINTYPBRACE datatype RINTYPBRACE variable opt_constraint opt_default	{
									debug_r("type=[LIST] inner type=%s varname=%s", $3, $5);
									std::string t($3);
									std::string n($5);
									driver.add_list_decl(n, t, false);
									FREE_PTR($3);
									FREE_PTR($5);

								}
	;

declare_ref_list:
		LIST LINTYPBRACE DATATYPE variable RINTYPBRACE variable opt_constraint opt_default	{
									debug_r("type=[LIST] inner type=%s varname=%s", $4, $6);
									std::string t($4);
									std::string n($6);
									driver.add_list_decl(n, t, true);
									FREE_PTR($4);
									FREE_PTR($6);	
								}
	;
declare_netive_map:
		MAP LINTYPBRACE datatype COMMA datatype RINTYPBRACE variable opt_constraint opt_default		{
									debug_r("type=[MAP] key type=%s value type=%s varname=%s", $3, $5, $7);
									std::string kt($3);
									std::string vt($5);
									std::string n($7);
									driver.add_map_decl(n, kt, vt, false);
									FREE_PTR($3);
									FREE_PTR($5);
									FREE_PTR($7);
								}
	;

declare_ref_map:
		MAP LINTYPBRACE datatype COMMA DATATYPE variable RINTYPBRACE variable opt_constraint opt_default	{
									debug_r("type=[MAP] key type=%s value type=%s varname=%s", $3, $6, $8);
									std::string kt($3);
									std::string vt($6);
									std::string n($8);
									driver.add_map_decl(n, kt, vt, true);
									FREE_PTR($3);
									FREE_PTR($6);
									FREE_PTR($8);
								}
	;
opt_default:
		/* null */
	|	DEFAULT	value			{
							debug_r("Defualt : %s", $2);
							std::string v($2);
							driver.set_default_value(v);
							FREE_PTR($2);
						}
opt_constraint:
		/* null */
	| 	constraint
	;


variable:
	VARNAME					{ debug_r("varaible=%s", $1); $$ = strdup($1); };


datatype:
		DOUBLE				{ debug_r("datatype=%s", $1); $$ = strdup($1); }	
	|	BYTE				{ debug_r("datatype=%s", $1); $$ = strdup($1); }	
	|	CHAR				{ debug_r("datatype=%s", $1); $$ = strdup($1); }
	|	BOOL				{ debug_r("datatype=%s", $1); $$ = strdup($1); }
	|	SHORT				{ debug_r("datatype=%s", $1); $$ = strdup($1); }
	|	INTEGER				{ debug_r("datatype=%s", $1); $$ = strdup($1); }
	|	LONG				{ debug_r("datatype=%s", $1); $$ = strdup($1); }
	|	FLOAT				{ debug_r("datatype=%s", $1); $$ = strdup($1); }
	|	TIMESTAMP			{ debug_r("datatype=%s", $1); $$ = strdup($1); }
	|	DATETIME			{ debug_r("datatype=%s", $1); $$ = strdup($1); }
	|	TEXT				{ debug_r("datatype=%s", $1); $$ = strdup($1); }
	| 	STRING 				{ debug_r("datatype=%s", $1); $$ = strdup($1); }
	;

size_def:
	LSZBRACE IVALUE RSZBRACE 		{ debug_r("size=%d", $2); $$=$2; };

declare_finish:
	TYPE_END	{ driver.finish_def(); }

constraint:
	CONSTRAINT LINBRACE constraint_type RINBRACE		

constraint_type:
		REGEX COLON STRING				{ 
									const std::string p("REGEX"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, $3);
									FREE_PTR($3);
								}
	|	NOT REGEX COLON STRING				{ 
									const std::string p("REGEX"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, $4);
									FREE_PTR($4);
								}
	|	IN LSZBRACE values RSZBRACE 			{ 
									const std::string p("IN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, $3);
									FREE_PTR($3);
								}
	|	NOT IN LSZBRACE values RSZBRACE 		{ 
									const std::string p("IN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, $4);
									FREE_PTR($4);
								}
	|	BETWEEN LSZBRACE values RSZBRACE 		{ 
									const std::string p("BETWEEN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, $3);
									FREE_PTR($3);
								}
	|	NOT BETWEEN LSZBRACE values RSZBRACE 		{ 
									const std::string p("BETWEEN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, $4);
									FREE_PTR($4);
								}
	|	LT LSZBRACE value RSZBRACE 			{ 
									const std::string p("LT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, $3);
									FREE_PTR($3);
								}
	|	NOT LT LSZBRACE value RSZBRACE 			{ 
									const std::string p("LT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, $4);
									FREE_PTR($4);
								}
	|	GT LSZBRACE value RSZBRACE 			{ 
									const std::string p("GT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, $3);
									FREE_PTR($3);
								}
	|	NOT GT LSZBRACE value RSZBRACE 			{ 
									const std::string p("GT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, $4);
									FREE_PTR($4);
								}
	;

values:
		value 			{ 
						int size = strlen($1);
						char *ptr = (char *)malloc(sizeof(char) * size);
						CHECK_ALLOC(ptr, TYPE_NAME(char));
						memset(ptr, 0, size);
						sprintf(ptr, "%s", $1);
						FREE_PTR($1);
						$$ = ptr;
						debug_r("VALUES {%s}", $$); 
 					}	
	|	values COMMA value	{ 
						int size = strlen($1) + strlen($3) + 4;
						char *ptr = (char *)malloc(sizeof(char) * size);
						CHECK_ALLOC(ptr, TYPE_NAME(char));
						memset(ptr, 0, size);
						sprintf(ptr, "%s,%s", $1, $3);
						FREE_PTR($1);
						FREE_PTR($3);
						$$ = ptr;
						debug_r("VALUES {%s}", $$); 
 					}
	;


value:
		IVALUE 		{ 	
					char *ptr = (char *)malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);
					CHECK_ALLOC(ptr, TYPE_NAME(char));
					memset(ptr, 0, DEFAULT_BUFFER_SIZE);
					sprintf(ptr, "%ld", $1);
					$$ = ptr; 
				}
	| 	DVALUE 		{ 	
					char *ptr = (char *)malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);
					CHECK_ALLOC(ptr, TYPE_NAME(char));
					memset(ptr, 0, DEFAULT_BUFFER_SIZE);
					sprintf(ptr, "%f", $1);
					$$ = ptr; 
				}
	| 	SVALUE		{ debug_r("String value [%s]", $1); $$ = strdup($1); }
	;


%%

void com::wookler::reactfs::core::parsers::schema_parser::error( const location_type &l, const std::string &err_message ) {
	throw TYPE_PARSER_ERROR("Parser error occurred. [error=%s][location=%s]", err_message.c_str(), l);
}

void debug_r(const char *s, ...) {
    va_list ap;
    va_start(ap, s);
    printf("debug: ");
    vfprintf(stdout, s, ap);
    printf("\n");
}
