%{
#include <string>
#include "schema_tokens.h"
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

void debug_r(const char *s, ...);
	
%}

%option debug
%option nodefault
%option yyclass="com::wookler::reactfs::core::parsers::schema_scanner"
%option noyywrap
%option c++

%%

%{          /** Code executed at the beginning of yylex **/
            yylval = lval;
%}

[ \t]+                  /* ignore whitespace */;

[0-9]+ 		{
		yylval->lval = atol(yytext);
		return (token::IVALUE);
	}

(([0-9]+(\.[0-9]*)?)|([0-9]*\.[0-9]+)) {
					yylval->dval = atof(yytext);
					return (token::DVALUE);
	}
\".*\"		{
		yylval->str = yytext;
		return (token::SVALUE);

	}

"TYPE"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::TYPE);

            	}
"DATATYPE"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::DATATYPE);

            	}
"BYTE" 			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::BYTE);

            	}
"CHAR"			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::CHAR);

            	}
"BOOLEAN"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::BOOL);

            	}
"SHORT"			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::SHORT);

            	}
"INTEGER"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::INTEGER);

            	}
"LONG"			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::LONG);

            	}
"FLOAT"			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::FLOAT);

            	}
"DOUBLE"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::DOUBLE);

            	}
"TIMESTAMP"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::TIMESTAMP);

            	}
"DATETIME"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::DATETIME);

            	}
"STRING"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::STRING);

            	}
"TEXT"			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::TEXT);

            	}
"ARRAY"			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::ARRAY);

            	}
"LIST"			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::LIST);

            	}
"MAP"			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::MAP);

            	}
"{" 		{ debug_r("[lexer] token : %s", yytext); return (token::LTYPEBRACE); }
"}"		{ debug_r("[lexer] token : %s", yytext); return (token::RTYPEBRACE); }
"("		{ debug_r("[lexer] token : %s", yytext); return (token::LINBRACE); }
")"		{ debug_r("[lexer] token : %s", yytext); return (token::RINBRACE); }
"["		{ debug_r("[lexer] token : %s", yytext); return (token::LSZBRACE); }
"]"		{ debug_r("[lexer] token : %s", yytext); return (token::RSZBRACE); }
"<" 		{ debug_r("[lexer] token : %s", yytext); return (token::LINTYPBRACE); }
">" 		{ debug_r("[lexer] token : %s", yytext); return (token::RINTYPBRACE); }
","		{ debug_r("[lexer] token : %s", yytext); return (token::COMMA); }
"DEFAULT"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::DEFAULT);

            	}
"REF"			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::REF);

            	}
";"		return (token::TYPE_END);
"PRIMARY KEY"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::PRIMARY_KEY);

            	}
"INDEX"			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::INDEX);

            	}
"SCHEMA"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::SCHEMA);

            	}
"CONSTRAINT"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::CONSTRAINT);

            	}
"REGEX"			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::REGEX);

            	}
"IN"			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::IN);

            	}
"BETWEEN"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::BETWEEN);

            	}
"NOT"			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::NOT);

            	}
"LESS THAN"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::LT);

            	}
"GREATER THAN"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::GT);

            	}
":"		{ debug_r("[lexer] token : %s", yytext); return (token::COLON); }

[_a-zA-Z0-9][_a-zA-Z0-9]+ 	{
				yylval->str = yytext;
				debug_r("[lexer]  varname : %s", yylval->str);
				return (token::VARNAME);
			}

\n		{
		location->lines();
	}

%%
