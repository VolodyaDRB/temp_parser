#include "utils.h"

std::string print_type(int type) {
    switch (type) {
        case 0:
            return "int";
        case 1:
            return "double";
        default:
            return "string";
    }
}

std::string print_vars_definition(std::vector<var_t> vars) {
    std::string res;
    std::sort(vars.begin(), vars.end());
    vars.erase(unique(vars.begin(), vars.end()), vars.end());
    for (auto x: vars) {
        res += print_type(x.type) + " " + x.name + ";\n";
    }
    return res;
}

std::string nest_block(std::string s, int nesting) {
    std::string res;
    for (size_t q = 0; q < nesting; ++q)
        res += '\t';
    for (size_t i = 0; i < s.size(); ++i) {
        res += s[i];
        if (i != s.size() - 1 && s[i] == '\n')
            for (size_t q = 0; q < nesting; ++q)
                res += '\t';
    }
    return res;
}