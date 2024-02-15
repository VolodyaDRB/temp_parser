%{
#include "lexer_generator.cpp"
#include "parser_generator.cpp"
#include "grammar.h"

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

std::fstream fout;

extern int yylex(void);
void yyerror (const char *s);

using production = std::vector<std::string>;

string code1, code2;
vector<rule> rules;
unordered_map<string, string> lexer_rules;
unordered_map<string, string> types;
unordered_map<string, string> nonterm_args;
unordered_set<string> tokens;
string start_name;

%}

%code requires {
    #include "grammar.h"
}

%union {
    string* str;
    pair<vector<string>, vector<string>>* rules_vec;
}

%type <rules_vec> rule_
%token <str> NAME LEXER_CODE PARSER_CODE REGEXP
%token PERC_SEP COLON PIPE TOKEN TYPE ARROW_LPAREN ARROW_RPAREN START LPAREN RPAREN

%start start

%%

start :
    LEXER_CODE types PERC_SEP lexer PERC_SEP grammar PERC_SEP LEXER_CODE {
        code1 = *$1;
        code2 = *$8;
        delete $1; delete $8;
    }

types :
    %empty {}
    | type types {}

type :
    TOKEN ARROW_LPAREN NAME ARROW_RPAREN NAME {
        types[*$5] = *$3;
        tokens.insert(*$5);
        delete $3; delete $5;
    }
    | TOKEN NAME {
        tokens.insert(*$2);
        delete $2;
    }
    | TYPE ARROW_LPAREN NAME ARROW_RPAREN NAME {
        types[*$5] = *$3;
        nonterm_args[*$5] = "";
        delete $3; delete $5;
    }
    | TYPE ARROW_LPAREN NAME ARROW_RPAREN NAME LPAREN NAME RPAREN {
        types[*$5] = *$3;
        nonterm_args[*$5] = *$7;
        delete $3; delete $5; delete $7;
    }
    | START NAME {
        start_name = *$2;
        delete $2;
    }

lexer :
    %empty {}
    | lexer REGEXP LEXER_CODE {
        lexer_rules[*$2] = *$3;
        delete $2; delete $3;
    }

grammar :
    %empty {}
    | grammar NAME COLON rule_ PARSER_CODE {
        rules.push_back({*$2, {(*$4).first, *$5, (*$4).second}});
        delete $2; delete $4; delete $5;
    }

rule_ :
    %empty { $$ = new vector<string>(); }
    | rule_ NAME {
        $1->first.push_back(*$2);
        $1->first.push_back("");
        $$ = $1;
        delete $2;
    }
    | rule_ NAME LPAREN NAME RPAREN {
        $1->push_back(*$2);
        $1->push_back(*$4);
        $$ = $1;
        delete $2; delete $4;
    }
%%

void yyerror(const char *s) {}

int main(int argc, char* argv[]) {
    int err = 0;
    FILE *in_f = NULL;

    if (argc < 3) {
        std::cerr << "Wrong arguments.\n";
        return 1;
    }

    if (argc > 1 && (in_f = freopen(argv[1], "r", stdin)) == NULL) {
        fprintf(stderr, "Can't open input file.\n");
        return 1;
    }

    fout.open(argv[2], std::fstream::out);

    if (err = yyparse()) {
        fclose(in_f);
        return err;
    }

    auto g = grammar(start_name, tokens, rules, types, lexer_rules);

    string lexer_src = lexer_generator(g)();
    string parser_src = parser_generator(g)();

    fout << code1 << lexer_src << parser_src << code2;

    fclose(in_f);
    return err;
}