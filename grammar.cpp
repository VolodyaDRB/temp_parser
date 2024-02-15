#include "grammar.h"
#include <cassert>

grammar::grammar(std::string start, strings tokens, std::vector<rule> rules,
                 string_map<std::string> types, string_map<std::string> lexer_rules, string_map<std::string> nonterm_args):
        start(std::move(start)), tokens(std::move(tokens)), rules(std::move(rules)),
        types(std::move(types)), lexer_rules(std::move(lexer_rules)),
        nonterm_args(std::move(nonterm_args)) {
    build_first();
    build_follow();
}

strings grammar::first(const production& s) const {
    if (tokens.contains(s[0]) || s[0] == eps) {
        return {s[0]};
    }
    auto new_first = _first.at(s[0]);
    if (new_first.erase(eps)) {
        auto first_b = first(production{next(s.begin()), s.end()});
        new_first.insert(first_b.begin(), first_b.end());
    }
    return new_first;
}

const strings& grammar::follow(const std::string& nonterm) const {
    return _follow.at(nonterm);
}

strings grammar::first1(const std::string& nonterm, const production& prod) const {
    auto res = first(prod);
    if (res.erase(eps)) {
        auto follow_A = follow(nonterm);
        res.insert(follow_A.begin(), follow_A.end());
    }
    return res;
}

strings grammar::nonterminals() const {
    strings result;
    for (const auto& [k, v]: _follow) {
        assert(!tokens.contains(k));
        result.insert(k);
    }
    return result;
}

static bool unite_sets(strings& a, const strings& b) {
    size_t old_size = a.size();
    a.insert(b.begin(), b.end());
    return a.size() != old_size;
}

void grammar::build_first() {
    bool changed;
    do {
        changed = false;
        for (const rule& r: rules) {
            _first[r.rule_def.prod[0]];
            changed |= unite_sets(_first[r.nonterm], first(r.rule_def.prod));
        }
    } while (changed);
}

void grammar::build_follow() {
    _follow[start] = {eof};
    bool changed;
    do {
        changed = false;
        for (const rule& r: rules) {
            std::vector<std::string> gamma = r.rule_def.prod;
            for (const std::string& B: r.rule_def.prod) {
                if (tokens.contains(B)) continue;
                gamma.erase(gamma.begin());
                if (gamma.empty()) gamma.push_back(eps);
                changed |= unite_sets(_follow[B], first1(r.nonterm, gamma));
            }
        }
    } while (changed);
}