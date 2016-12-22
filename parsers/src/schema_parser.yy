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
   #include "types/includes/type_defs.h"
   #include "types/includes/type_errors.h"
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
%token			LIST
%token			MAP
%token 			LTYPEBRACE
%token			RTYPEBRACE
%token			LINBRACE
%token			RINBRACE
%token			LSZBRACE
%token			RSZBRACE
%token			COMMA
%token			ON
%token			DEFAULT
%token			TYPE_END
%token			KEY_FIELDS
%token			INDEX	
%token			SCHEMA
%left			CONSTRAINT
%token			REGEX	
%token			IN
%token			BETWEEN
%token			NULLABLE
%token			NAMESPACE
%token			NOT
%token			LT
%token			GT
%token			LTEQ
%token			GTEQ
%token			ASC
%token			DESC
%token			COLON
%token			DOT
%token 			NEWLINE
%token 			FULLTEXT_INDEX
%token 			HASH_INDEX
%token 			TREE_INDEX
%type<dval>		DVALUE
%type<lval>		IVALUE opt_nullable
%type<str>		FULLTEXT_INDEX HASH_INDEX TREE_INDEX
%type<str>		SVALUE VARNAME STRING DOUBLE BYTE CHAR BOOL SHORT INTEGER LONG FLOAT TIMESTAMP DATETIME  TEXT LIST MAP ASC DESC
%type<str>		value values datatype variable nested_variable opt_sort column columns key_fields sort
%type<str>		declare declare_native declare_ref declare_native_list declare_ref_list declare_netive_map declare_ref_map


%locations

%%

parse:
		opt_namespace opt_types schema opt_indexes
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
opt_namespace:
		/* null */
	|	namespace
	;

namespace:
		NAMESPACE nested_variable TYPE_END	{
							std::string ss($2);
							debug_r("namespace [%s]", ss.c_str());
							driver.set_namespace(ss);
							FREE_PTR($2);

						}
	;
opt_indexes:
		/* null */
	|	indexes
	;

indexes:
		index
	| indexes index
	;

index:
		index_declare index_fields opt_index_type declare_finish	{

						}
	;

index_fields:
		LINBRACE columns RINBRACE	{
								debug_r("KEY FIELDS: (%s)", $2);
								std::string keys($2);
								driver.set_index_fields(keys);
								FREE_PTR($2);
							}
	;
opt_index_type:
		/* null */
	|	index_type
	;

index_type:
		FULLTEXT_INDEX			{ driver.set_index_type($1); }
	|	HASH_INDEX			{ driver.set_index_type($1); }
	|	TREE_INDEX			{ driver.set_index_type($1); }
	;

index_declare:
		INDEX variable ON variable	{
							std::string n($2);
							std::string s($4);
							debug_r("Creating index [%s] on schema [%s]", n.c_str(), s.c_str());
							driver.create_index(n, s);
							FREE_PTR($2);
							FREE_PTR($4);
						}
	;

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
		nested_variable opt_sort		{
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
	|	declare_native_list
	|	declare_ref_list
	|	declare_netive_map
	|	declare_ref_map
	;

declare_native:
		datatype variable opt_constraint opt_default opt_nullable		{ 
							debug_r("[type=%s] varname=%s nullable=%d", $1, $2, $5);
							std::string t($1);
							std::string n($2);
							driver.add_declaration(n, t, false, $5); 
							FREE_PTR($1);
							FREE_PTR($2);
						}				
	;

declare_ref:
		DATATYPE variable variable opt_constraint opt_default opt_nullable	{ 
							debug_r("[type=%s] varname=%s", $2, $3);
							debug_r("varname=%s nullable=%d", $3, $6);
							std::string t($2);
							std::string n($3);
							driver.add_declaration(n, t, true, $6); 
						}
	;

declare_native_list:
		LIST LT datatype GT variable opt_constraint opt_default opt_nullable	{
									debug_r("type=[LIST] inner type=%s varname=%s", $3, $5);
									debug_r("varname=%s nullable=%d", $5, $8);
									std::string t($3);
									std::string n($5);
									driver.add_list_decl(n, t, false, $8);
									FREE_PTR($3);
									FREE_PTR($5);

								}
	;

declare_ref_list:
		LIST LT DATATYPE variable GT variable opt_constraint opt_default opt_nullable	{
									debug_r("type=[LIST] inner type=%s varname=%s", $4, $6);
									debug_r("varname=%s nullable=%d", $6, $9);
									std::string t($4);
									std::string n($6);
									driver.add_list_decl(n, t, true, $9);
									FREE_PTR($4);
									FREE_PTR($6);	
								}
	;
declare_netive_map:
		MAP LT datatype COMMA datatype GT variable opt_constraint opt_default	opt_nullable	{
									debug_r("type=[MAP] key type=%s value type=%s varname=%s", $3, $5, $7);
									debug_r("varname=%s nullable=%d", $7, $10);
									std::string kt($3);
									std::string vt($5);
									std::string n($7);
									driver.add_map_decl(n, kt, vt, false, $10);
									FREE_PTR($3);
									FREE_PTR($5);
									FREE_PTR($7);
								}
	;

declare_ref_map:
		MAP LT datatype COMMA DATATYPE variable GT variable opt_constraint opt_default opt_nullable	{
									debug_r("type=[MAP] key type=%s value type=%s varname=%s", $3, $6, $8);
									debug_r("varname=%s nullable=%d", $8, $11);
									std::string kt($3);
									std::string vt($6);
									std::string n($8);
									driver.add_map_decl(n, kt, vt, true, $11);
									FREE_PTR($3);
									FREE_PTR($6);
									FREE_PTR($8);
								}
	;

opt_nullable:
		/* null */			{ 	$$ = 0; }
	|	NULLABLE			{ 	$$ = 1; driver.set_nullable(); }
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

nested_variable:
		variable			{ 
							$$ = strdup($1);
							FREE_PTR($1);
						}
	|	nested_variable DOT variable 	{
							int s = strlen($1) + strlen($3) + 2;
							char *p = (char *)malloc(s * sizeof(char));
							CHECK_ALLOC(p, TYPE_NAME(char));
							memset(p, 0, s);
							sprintf(p, "%s.%s", $1, $3);
							FREE_PTR($1);
							FREE_PTR($3);
							$$ = p;
						}
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
	|	LT value 					{ 
									const std::string p("LT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, $2);
									FREE_PTR($2);
								}
	|	NOT LT value 					{ 
									const std::string p("LT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, $3);
									FREE_PTR($3);
								}
	|	LTEQ value 					{ 
									const std::string p("LTEQ"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, $2);
									FREE_PTR($2);
								}
	|	NOT LTEQ value 					{ 
									const std::string p("LTEQ"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, $3);
									FREE_PTR($3);
								}

	|	GT value 					{ 
									const std::string p("GT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, $2);
									FREE_PTR($2);
								}
	|	NOT GT value 					{ 
									const std::string p("GT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, $3);
									FREE_PTR($3);
								}
	|	GTEQ value 					{ 
									const std::string p("GTEQ"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, $2);
									FREE_PTR($2);
								}
	|	NOT GTEQ value 					{ 
									const std::string p("GTEQ"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, $3);
									FREE_PTR($3);
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
