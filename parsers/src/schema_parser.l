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

-?[0-9]+ 		{
			yylval->lval = atol(yytext);
			return (token::IVALUE);
		}

-?(([0-9]+(\.[0-9]*)?)|([0-9]*\.[0-9]+)) {
			yylval->dval = atof(yytext);
			return (token::DVALUE);
		}
\"(\\.|[^\\"])*\"		{
			yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
			return (token::SVALUE);

		}

"type"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::TYPE);

            	}
"byte" 			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::BYTE);

            	}
"char"			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::CHAR);

            	}
"boolean"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::BOOL);

            	}
"short"			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::SHORT);

            	}
"integer"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::INTEGER);

            	}
"long"			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::LONG);

            	}
"float"			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::FLOAT);

            	}
"double"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::DOUBLE);

            	}
"timestamp"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::TIMESTAMP);

            	}
"datetime"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::DATETIME);

            	}
"string"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::STRING);

            	}
"text"			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::TEXT);

            	}
"array"			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::ARRAY);

            	}
"list"			{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::LIST);

            	}
"map"			{
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
","		{ debug_r("[lexer] token : %s", yytext); return (token::COMMA); }
"default"		{
			debug_r("[lexer]  token : %s", yytext);
            		return (token::DEFAULT);

            	}
"typeref"			{
			debug_r("[lexer]  token : %s", yytext);
            		return (token::DATATYPE);

            	}
"on"			{
			debug_r("[lexer]  token : %s", yytext);
            		return (token::ON);

            	}

";"			return (token::TYPE_END);
"set_key_fields"		{
			debug_r("[lexer]  token : %s", yytext);
            		return (token::KEY_FIELDS);

            	}
"index"			{
			debug_r("[lexer]  token : %s", yytext);
            		return (token::INDEX);

            	}
"schema"		{
			debug_r("[lexer]  token : %s", yytext);
            		return (token::SCHEMA);

            	}
"constraint"		{
			debug_r("[lexer]  token : %s", yytext);
            		return (token::CONSTRAINT);

            	}
"regex"			{
			debug_r("[lexer]  token : %s", yytext);
            		return (token::REGEX);

            	}
"in"			{
			debug_r("[lexer]  token : %s", yytext);
            		return (token::IN);

            	}
"between"		{
			debug_r("[lexer]  token : %s", yytext);
            		return (token::BETWEEN);

            	}
"namespace"		{
			debug_r("[lexer]  token : %s", yytext);
            		return (token::NAMESPACE);

            	}

"!"			{
			debug_r("[lexer]  token : %s", yytext);
            		return (token::NOT);

            	}
"<"		{
			debug_r("[lexer]  token : %s", yytext);
            		return (token::LT);

            	}
"<="		{
			debug_r("[lexer]  token : %s", yytext);
            		return (token::LTEQ);

            	}
">"		{
			debug_r("[lexer]  token : %s", yytext);
            		return (token::GT);

            	}
">="		{
			debug_r("[lexer]  token : %s", yytext);
            		return (token::GTEQ);

            	}
"nullable"		{
			debug_r("[lexer]  token : %s", yytext);
            		return (token::NULLABLE);

            	}
"ASC"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::ASC);

            	}
"DESC"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::DESC);

            	}
"FULLTEXT"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::FULLTEXT_INDEX);

            	}
"HASH" 		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::HASH_INDEX);

            	}
"TREE"		{
            		yylval->str = yytext;
			debug_r("[lexer]  token : %s", yylval->str);
            		return (token::TREE_INDEX);

            	}

":"		{ debug_r("[lexer] token : %s", yytext); return (token::COLON); }

"\."		{ debug_r("[lexer] token : %s", yytext); return (token::DOT); }

"//".*		{ location->lines(); }

\n		{ location->lines(); }

[_a-zA-Z0-9][_a-zA-Z0-9]+ 	{
				yylval->str = yytext;
				debug_r("[lexer]  varname : %s", yylval->str);
				return (token::VARNAME);
			}


%%
