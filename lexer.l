%{

#include <string>
#include <iostream>

using namespace std;

#include "parser.hpp"

%}

%option noyywrap

%%

[ \t\n]+

"%start" { return START; }

"%token" { return TOKEN; }

"%type" { return TYPE; }

"<" { return ARROW_LPAREN; }

">" { return ARROW_RPAREN; }

"(" { return LPAREN; }

")" { return RPAREN; }

"%%" { return PERC_SEP; }

":" { return COLON; }

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.str = new std::string();
    *yylval.str = std::string(yytext, yyleng);
    return NAME;
}

"%{"[^%]*"%}" {
    yylval.str = new std::string();
    *yylval.str = std::string(yytext, yyleng).substr(2, yyleng - 4);
    return LEXER_CODE;
}

"#{"[^#]*"#}" {
    yylval.str = new std::string();
    *yylval.str = std::string(yytext, yyleng).substr(1, yyleng - 3) + "}";
    return PARSER_CODE;
}

"\\".*"\\" {
    yylval.str = new std::string();
    *yylval.str = std::string(yytext, yyleng).substr(1, yyleng - 2);
    return REGEXP;
}

%%