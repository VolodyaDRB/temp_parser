#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

const std::string eps = "EPS";
const std::string eof = "EOF";

using strings = std::unordered_set<std::string>;
template <typename T>
using string_map = std::unordered_map<std::string, T>;
using production = std::vector<std::string>;

struct rule {
    struct rule_definition {
        production prod;
        std::vector<std::string> args;
        std::string code = "";
    };

    std::string nonterm;
    rule_definition rule_def;
};

struct grammar {
    grammar(std::string start, strings tokens, std::vector<rule> rules,
            string_map<std::string> types,
            string_map<std::string> lexer_rules,
            string_map<std::string> nonterm_args);

    strings first(const production& s) const;
    const strings& follow(const std::string& nonterm) const;
    strings first1(const std::string& nonterm, const production& prod) const;

    strings nonterminals() const;

    const std::string start;
    const strings tokens;
    const std::vector<rule> rules;
    const string_map<std::string> types;
    const string_map<std::string> lexer_rules;
    const string_map<std::string> nonterm_args;
private:
    string_map<strings> _first;
    string_map<strings> _follow;

    void build_first();
    void build_follow();
};