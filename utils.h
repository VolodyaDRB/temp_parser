#pragma once

#include <string>
#include <utility>
#include <vector>
#include <algorithm>

struct expr_t {
    std::string data;
    int type;

    expr_t(std::string s, int t) : data(std::move(s)), type(t) {}
};

struct var_t {
    std::string name;
    int type{};

    var_t(std::string s, int t) : name(std::move(s)), type(t) {}

    bool operator< (const var_t& rhs) const {
        return name < rhs.name;
    }

    bool operator== (const var_t& rhs) const {
        return name == rhs.name;
    }
};

struct block_t {
    std::vector<var_t> vars;
    std::string data;
    int nesting{};

    block_t(std::vector<var_t> v, std::string s, int n) : vars(std::move(v)), data(std::move(s)), nesting(n) {}
};

std::string print_type(int type);
std::string nest_block(std::string s, int nesting);
std::string print_vars_definition(std::vector<var_t> vars);
