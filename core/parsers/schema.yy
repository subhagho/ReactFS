%skeleton "lalr1.cc"
%require  "3.0"
%debug 
%defines 
%define api.namespace {com::wookler::reactfs::core::parsers}
%define parser_class_name {schema_parser}

%code requires{
	namespace com {
		namespace wookler {
			namespace reactfs {
				namespace core {
					namespace parsers {
						class schema_driver;
						class schema_parser;
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

%parse-param { schema_scanner  &scanner  }
%parse-param { schema_driver  &driver  }

%code{
   #include <iostream>
   #include <cstdlib>
   #include <fstream>
   
   /* include for all driver functions */
   #include "core/includes/prasers/schema_driver.h"
   #include "core/includes/types/type_defs.h"
   #include "core/includes/types/type_errors.h"

#undef yylex
#define yylex scanner.yylex
}

%define api.value.type variant
%define parse.assert
%define api.token.prefix {TOK_}

%token 			END	0	"end of file"
%token 	<std::string>	VARNAME
%token 			TYPE		"TYPE"
%token			BYTE		"BYTE"
%token			CHAR		"CHAR"
%token			BOOL		"BOOLEAN"
%token			SHORT		"SHORT"
%token			INT		"INTEGER"
%token			LONG		"LONG"
%token			FLOAT		"FLOAT"
%token			DOUBLE		"DOUBLE"
%token			TIMESTAMP	"TIMESTAMP"
%token			DATETIME	"DATETIME"
%token			STRING		"STRING"
%token			TEXT		"TEXT"
%token			ARRAY		"ARRAY"
%token			LIST		"LIST"
%token			MAP		"MAP"
%token 			LTYPEBRACE	"{"
%token			RTYPEBRACE	"}"
%token			LINBRACE	"("
%token			RINBRACE	")"
%token			LSZBRACE	"["
%token			RSZBRACE	"]"
%token			COMMA		","
%token			DEFAULT		"DEFAULT"
%token			REF		"REF"
%token			TYPE_END	";"
%token			PRIMARY_KEY	"PRIMARY KEY"
%token			INDEX		"INDEX"
%token			SCHEMA		"SCHEMA"
%token			CONSTRAINT	"CONSTRAINT"
%token			REGEX		"REGEX"
%token			VALUE		"VALUE"
%token			BETWEEN		"BETWEEN"
%token			NOT		"NOT"
%token			LT		"LESS THAN"
%token			GT		"GREATER THAN"

%locations

%%

%%

void com::wookler::reactfs::core::parsers::schema_parser::error( const location_type &l, const std::string &err_message ) {
	throw TYPE_PARSER_ERROR("Parser error occurred. [error=%s][location=%s]", err_message.c_str(), l);
}
