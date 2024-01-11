%{
#include <string>
#include <iostream>

#include "parser.hpp"
%}

%option noyywrap

%%

[ \t]+
\r?\n
#.*$

<<EOF>>    { return 0; }

[0-9]+ {
    yylval.exp = new expr_t(std::string(yytext, yyleng), 0);
    return INT;
}

[0-9]+"."[0-9]* {
    yylval.exp = new expr_t(std::string(yytext, yyleng), 1);
    return DOUBLE;
}

"if"    { return IF; }

"elif"  { return ELIF; }

"else"  { return ELSE; }

"for"   { return FOR; }

"in"    { return IN; }

"range" { return RANGE; }

"while" { return WHILE; }

"break" { return BREAK; }

"continue" { return CONTINUE; }

"def"   { return DEF; }

"return" { return RETURN; }

"="     { return ASSIGN; }

"("     { return LPAREN; }

")"     { return RPAREN; }

","     { return COMMA; }

":"     { return COLON; }

"+"     { return ADD; }

"-"     { return SUB; }

"*"     { return MUL; }

"%"     { return MOD; }

"//"    { return DIV; }

"/"     { return FDIV; }

"not"   { return NOT; }

"and"   { return AND; }

"or"    { return OR; }

"=="    { return EQ; }

"!="    { return NEQ; }

">"     { return GT; }

"<"     { return LT; }

">="    { return GET; }

"<="    { return LET; }

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.var = new var_t(std::string(yytext, yyleng), 0);
    return VAR;
}

%%