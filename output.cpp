

#include <string>
#include <list>
#include <utility>
#include <regex>
#include <cassert>
#include <iostream>
#include <fstream>

std::string PARSER_INPUT;

class tree {
    std::string node;
    std::list<tree> children;

public:
    tree() = default;
    tree(std::string node_);
    std::string get_node() const;
    std::list<tree> get_children() const;
    void add_child(tree t);
};

tree::tree(std::string node_) : node(std::move(node_)) {}

std::string tree::get_node() const {
    return node;
}

std::list<tree> tree::get_children() const {
    return children;
}

void tree::add_child(tree t) {
    children.push_back(std::move(t));
}

using t_ptr = tree*;

union {
t_ptr t_ptr_field;
} cur_value;
enum token_type {
NAME,
NUMBER,
COMMA,
SEMICOLON,
STAR,
RPAREN,
LPAREN,
MY_EOF,
} cur_token;
bool consume(std::string pattern, std::string& s) {
        auto re = std::regex("^" + pattern, std::regex::basic);
        std::string res = std::regex_replace(s, re, "");
        if (res == s) return false;
        s = res; return true;
        }
token_type get_next_token() {
std::string input = PARSER_INPUT;
while (true) {
if (input.empty()) return MY_EOF;
if (consume("[a-zA-Z_][a-zA-Z0-9_]*", input))  cur_value.tree = tree(cur_match); return NAME; if (consume("[0-9]+", input))  cur_value.tree = tree(cur_match); return NUMBER; if (consume("","", input))  cur_value.tree = tree(","); return COMMA; if (consume("";"", input))  cur_value.tree = tree(";"); return SEMICOLON; if (consume(""*"", input))  cur_value.tree = tree("*"); return STAR; if (consume(""]"", input))  cur_value.tree = tree("]"); return RPAREN; if (consume(""["", input))  cur_value.tree = tree("["); return LPAREN; }
}
void next_token() {
cur_token = get_next_token();
}
t_ptr S() {
t_ptr value_0;
if (cur_token == NAME) {
assert(cur_token == NAME);
auto value_1 = cur_value.t_ptr_field;
next_token();
auto value_2 = P();
assert(cur_token == NAME);
auto value_3 = cur_value.t_ptr_field;
next_token();
auto value_4 = A();
auto value_5 = V();
assert(cur_token == SEMICOLON);
auto value_6 = cur_value.t_ptr_field;
next_token();
{
    value_0 = new tree("S");
    value_0->add_child(*value_1);
    value_0->add_child(*value_2);
    value_0->add_child(*value_3);
    value_0->add_child(*value_4);
    value_0->add_child(*value_5);
    value_0->add_child(*value_6);
    delete value_1; delete value_2; delete value_3; delete value_4; delete value_5; delete value_6;
}}
}
t_ptr P() {
t_ptr value_0;
if (cur_token == STAR) {
assert(cur_token == STAR);
auto value_1 = cur_value.t_ptr_field;
next_token();
auto value_2 = P();
{
    value_0 = new tree("P");
    value_0->add_child(*value_1);
    value_0->add_child(*value_2);
    delete value_1; delete value_2;
}}
if (cur_token == NAME) {
EPS();
{
    value_0 = new tree("P");
    value_0->add_child(tree("eps"));
}}
}
t_ptr A() {
t_ptr value_0;
if (cur_token == LPAREN) {
assert(cur_token == LPAREN);
auto value_1 = cur_value.t_ptr_field;
next_token();
assert(cur_token == NUMBER);
auto value_2 = cur_value.t_ptr_field;
next_token();
assert(cur_token == RPAREN);
auto value_3 = cur_value.t_ptr_field;
next_token();
auto value_4 = A();
{
    value_0 = new tree("A");
    value_0->add_child(*value_1);
    value_0->add_child(*value_2);
    value_0->add_child(*value_3);
    value_0->add_child(*value_4);
    delete value_1; delete value_2; delete value_3; delete value_4;
}}
if (cur_token == SEMICOLON || cur_token == COMMA) {
EPS();
{
    value_0 = new tree("A");
    value_0->add_child(tree("eps"));
}}
}
t_ptr V() {
t_ptr value_0;
if (cur_token == COMMA) {
assert(cur_token == COMMA);
auto value_1 = cur_value.t_ptr_field;
next_token();
auto value_2 = P();
assert(cur_token == NAME);
auto value_3 = cur_value.t_ptr_field;
next_token();
auto value_4 = A();
auto value_5 = V();
{
    value_0 = new tree("V");
    value_0->add_child(*value_1);
    value_0->add_child(*value_2);
    value_0->add_child(*value_3);
    value_0->add_child(*value_4);
    value_0->add_child(*value_5);
    delete value_1; delete value_2; delete value_3; delete value_4; delete value_5;
}}
if (cur_token == SEMICOLON) {
EPS();
{
    value_0 = new tree("V"); value_0->add_child(tree("eps"));
}}
}
t_ptr parse() {
next_token();
return S();
}


static std::string get_vertex(int n, const std::string& name) {
    return std::to_string(n) + "[label=\"" + name + "\"]\n";
}

static std::string get_edge(int v, int u) {
    return std::to_string(v) + " -> " + std::to_string(u) + "\n";
}

static void dfs(const tree& v, std::ofstream& fout, int& cnt) {
    int v_n = cnt;
    if (v_n == 0)
        fout << get_vertex(v_n, v.get_node());
    for (const tree& u: v.get_children()) {
        int u_n = ++cnt;
        fout << get_vertex(u_n, u.get_node());
        fout << get_edge(v_n, u_n);
        dfs(u, fout, cnt);
    }
}

int main(int argc, char* argv[]) {
    std::cin >> PARSER_INPUT;
    int cnt = 0;
    std::ofstream fout("3.txt");
    fout << "digraph G {\n";
    dfs(*parse(), fout, cnt);
    fout << "}";
}

