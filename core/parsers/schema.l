%{
#include <string>
#include "schema.tab.hh"
#include "schema_scanner.h"
#include "schema.h"

#undef YY_DECL
#define YY_DECL int com::wookler::reactfs::core::parsers::schema_scanner::yylex( \
				com::wookler::reactfs::core::parsers::schema_parser::semantic_type * const lval, \
				com::wookler::reactfs::core::parsers::schema_parser::location_type *location)

/* typedef to make the returns for the tokens shorter */
using token = com::wookler::reactfs::core::parsers::schema_parser::token;
using namespace std;

/* define yyterminate as this instead of NULL */
#define yyterminate() return( token::END )

/* update location on matching */
#define YY_USER_ACTION location->step(); location->columns(yyleng);
	
%}

%option debug
%option nodefault
%option yyclass="com::wookler::reactfs::core::parsers::schema_scanner"
%option noyywrap
%option c++

%%
%{          /** Code executed at the beginning of yylex **/
%}

[0-9]+ 		{
		yylval->lval = atol(yytext);
		return (token::IVALUE);
	}

(([0-9]+(\.[0-9]*)?)|([0-9]*\.[0-9]+)) {
					yylval->dval = atof(yytext);
					return (token::DVALUE);
	}
\".*\"		{
		char *c = strdup(yytext);
		yylval->str = c;
		return (token::SVALUE);

	}
[_a-zA-Z0-9][_a-zA-Z0-9]+ 	{
		char *c = strdup(yytext);
		yylval->str = c;
		return (token::VARNAME);
	}

\n		{
		location->lines();
		return (token::NEWLINE);
	}

"TYPE"		return (token::TYPE);
"DATATYPE"	return (token::DATATYPE);
"BYTE" 		return (token::BYTE);
"CHAR"		return (token::CHAR);
"BOOLEAN"	return (token::BOOL);
"SHORT"		return (token::SHORT);
"INTEGER"	return (token::INTEGER);
"LONG"		return (token::LONG);
"FLOAT"		return (token::FLOAT);
"DOUBLE"	return (token::DOUBLE);
"TIMESTAMP"	return (token::TIMESTAMP);
"DATETIME"	return (token::DATETIME);
"STRING"	return (token::STRING);
"TEXT"		return (token::TEXT);
"ARRAY"		return (token::ARRAY);
"LIST"		return (token::LIST);
"MAP"		return (token::MAP);
"{" 		return (token::LTYPEBRACE);
"}"		return (token::RTYPEBRACE);
"("		return (token::LINBRACE);
")"		return (token::RINBRACE);
"["		return (token::LSZBRACE);
"]"		return (token::RSZBRACE);
","		return (token::COMMA);
"DEFAULT"	return (token::DEFAULT);
"REF"		return (token::REF);
";"		return (token::TYPE_END);
"PRIMARY KEY"	return (token::PRIMARY_KEY);
"INDEX"		return (token::INDEX);
"SCHEMA"	return (token::SCHEMA);	
"CONSTRAINT"	return (token::CONSTRAINT);
"REGEX"		return (token::REGEX);
"IN"		return (token::IN);
"BETWEEN"	return (token::BETWEEN);
"NOT"		return (token::NOT);
"LESS THAN"	return (token::LT);
"GREATER THAN"	return (token::GT);
":"		return (token::COLON);
";"		return (token::SEMICOLON);
"\n"		return (token::NEWLINE);

[ \t]+                  /* ignore whitespace */;
%%
