%{
#include <string>

#include "core/includes/parsers/schema_scanner.h"
#undef YY_DECL
#define YY_DECL int com::wookler::reactfs::core::types::schema_scanner::yylex( \
					com::wookler::reactfs::core::parsers::schema_parser::semantic_type * const lval, \
                                   	com::wookler::reactfs::core::parsers::schema_parser::location_type *location)

/* typedef to make the returns for the tokens shorter */
using token = com::wookler::reactfs::core::types::schema_scanner::token

/* define yyterminate as this instead of NULL */
#define yyterminate() return( token::END )

/* update location on matching */
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);
	
%}

%option debug
%option nodefault
%option yyclass="com::wookler::reactfs::core::types::schema_scanner"
%option noyywrap
%option c++

%%
%{          /** Code executed at the beginning of yylex **/
            yylval = lval;
%}

[_a-zA-Z0-9]+ 	{
		yylval->build<std::string>( yytext );
		return (token::VARNAME);
	}

\n		{
		location->lines();
		return (token::NEWLINE);
	}

%%
