%{
#include <iostream>
#include <fstream>

#include "utils.h"
#include "lexer.h"

std::fstream fout;

extern int yylex(void);
void yyerror (const char *s);

#define UN_IMPL_OVERLOAD(op, op_type) \
yyval.exp = new expr_t(std::string(op) + " " + yyvsp[0].exp->data, std::max(yyvsp[0].exp->type, op_type)); \
delete yyvsp[0].exp;

#define UN_IMPL(op, op_type) \
yyval.exp = new expr_t(std::string(op) + " " + yyvsp[0].exp->data, op_type); \
delete yyvsp[0].exp;

#define BIN_IMPL_OVERLOAD(op, op_type) \
yyval.exp = new expr_t(yyvsp[-2].exp->data + " " + op + " " + yyvsp[0].exp->data, std::max(std::max(yyvsp[0].exp->type, yyvsp[-2].exp->type), op_type)); \
delete yyvsp[0].exp; delete yyvsp[-2].exp;

#define BIN_IMPL(op, op_type) \
yyval.exp = new expr_t(yyvsp[-2].exp->data + " " + op + " " + yyvsp[0].exp->data, op_type); \
delete yyvsp[0].exp; delete yyvsp[-2].exp;
%}

%code requires {
    #include "utils.h"
}

%union {
    expr_t*  exp;
    var_t*   var;
    block_t* blk;
}

%type <exp> expr
%type <blk> state block

%token COLON COMMA RPAREN LPAREN ASSIGN RETURN DEF CONTINUE BREAK WHILE RANGE IN FOR ELSE ELIF IF
%token <var> VAR
%token <exp> INT DOUBLE STRING

%left OR
%left AND
%left NOT
%precedence LT GT LET GET EQ NEQ
%left SUB ADD
%left MUL FDIV MOD DIV
%left NEG POS

%start start

%%

start :
    block {
        fout << "#include <stdio.h>\n\n" << print_vars_definition($1->vars) + "\n";

        std::string data = nest_block($1->data, 1);

        fout << "int main(void) {\n" << data << "\treturn 0;\n}\n\n";

        delete $1;
    }

block :
    IF expr COLON block {
        $$ = new block_t({},
            "if (" + $2->data + ") {\n" + nest_block($4->data, $4->nesting) + "}\n",
             0);
        delete $2; delete $4;
    }
    | ELIF expr COLON block {}
    | ELSE COLON block {}
    | WHILE expr COLON block {}
    | FOR VAR IN RANGE LPAREN expr RPAREN COLON block {}
    | block state {
        auto vec = $1->vars;
        vec.insert(vec.end(), $2->vars.begin(), $2->vars.end());
        $$ = new block_t(vec, $1->data + $2->data, 0);
        delete $1; delete $2;
    }
    | state { $$ = $1; }

state :
    expr {
        $$ = new block_t({}, $1->data + ";\n" , 0);
        delete $1;
    }
    | VAR ASSIGN expr {
        var_t v($1->name, $3->type);
        $$ = new block_t(std::vector{v}, $1->name + " = " + $3->data + ";\n" , 0);
        delete $1; delete $3;
    }

expr :
    INT                     { $$ = $1; }
    | DOUBLE                { $$ = $1; }
    | STRING                { $$ = $1; }
    | VAR                   {
        $$ = new expr_t($1->name, $1->type);
        delete $1;
    }
    | expr OR expr          { BIN_IMPL("||", 0) }
    | expr AND expr         { BIN_IMPL("&&", 0) }
    | NOT expr              { UN_IMPL("!", 0) }
    | expr LT expr          { BIN_IMPL("<", 0) }
    | expr GT expr          { BIN_IMPL(">", 0) }
    | expr LET expr         { BIN_IMPL("<=", 0) }
    | expr GET expr         { BIN_IMPL(">=", 0) }
    | expr EQ expr          { BIN_IMPL("==", 0) }
    | expr NEQ expr         { BIN_IMPL("!=", 0) }
    | expr SUB expr         { BIN_IMPL_OVERLOAD("-", 0) }
    | expr ADD expr         { BIN_IMPL_OVERLOAD("+", 0) }
    | expr MUL expr         { BIN_IMPL_OVERLOAD("*", 0) }
    | expr FDIV expr        { BIN_IMPL_OVERLOAD("/", 1) }
    | expr MOD expr         { BIN_IMPL_OVERLOAD("%", 0) }
    | expr DIV expr         { BIN_IMPL_OVERLOAD("/", 0) }
    | SUB expr %prec NEG    { UN_IMPL_OVERLOAD("-", 0) }
    | ADD expr %prec POS    { UN_IMPL_OVERLOAD("+", 0) }
    | LPAREN expr RPAREN    {
        $$ = new expr_t("(" + $2->data + ")", $2->type);
        delete $2;
    }
%%

void yyerror(const char *s){
    fprintf(stderr, "%s\n", s);
}

int main(int argc, char* argv[]) {
    int err = 0;
    FILE *in_f = NULL;

    if (argc < 3) {
        std::cerr << "Wrong arguments.\n";
        return 1;
    }

    if (argc > 1 && (in_f = freopen(argv[1], "r", stdin)) == NULL) {
        fprintf(stderr, "Can't open input file.\n");
        err = 1;
        goto close;
    }

    fout.open(argv[2], std::fstream::out);

    if (err = yyparse())
        goto close;

close:
    fclose(in_f);
    return err;
}