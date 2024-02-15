#include "grammar.h"

class parser_generator {
    const grammar& g;

    std::string get_type(std::string s) const {
        auto it = g.types.find(s);
        return it != g.types.end() ? it->second: "void";
    }

    std::string gen_declaration(const std::string& nonterminal) const {
        return get_type(nonterminal) + " " + nonterminal + "(" + gen_args(nonterminal) + ")";
    }

    std::string gen_nonterminal_parser(const std::string& nonterminal,
                                       const std::vector<rule::rule_definition>& rule_defs) const {
        std::string res = gen_declaration(nonterminal) + " {\n";
        if (g.types.contains(nonterminal)) {
            res += g.types.at(nonterminal) + " value_0;\n";
        }
        for (const auto& rule_def: rule_defs) {
            auto options = g.first1(nonterminal, rule_def.prod);
            res += "if (";
            for (const auto& opt: options) {
                res += "cur_token == " + opt + " || ";
            }
            res.erase(prev(res.end(), 4), res.end());
            res += ") {\n";
            res += process_rule(nonterminal, rule_def);
            res += "}\n";
        }
        if (g.types.contains(nonterminal)) {
            res += "return value_0;\n";
        }
        res += "}\n";
        return res;
    }

    std::string gen_args(const std::string& nt) const {
        size_t i = 0;
        if (!g.nonterm_args.contains(nt)) return "";
        return g.nonterm_args.at(nt) + " acc";
    }

    static std::string replace_all(std::string str, const std::string& from, const std::string& to) {
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return str;
    }

    std::string process_rule(const std::string& nonterminal, const rule::rule_definition& rule_def) const {
        std::string res;
        size_t i = 0;
        for (const auto& s: rule_def.prod) {
            ++i;
            if (g.tokens.contains(s)) {
                res += "assert(cur_token == " + s + ");\n";
                if (g.types.contains(s)) {
                    res += "auto value_" + std::to_string(i) + " = cur_value." + g.types.at(s) + "_field;\n";
                }
                res += "next_token();\n";
            } else {
                if (g.types.contains(s)) {
                    res += "auto value_" + std::to_string(i) + " = ";
                }
                auto args = replace_all(replace_all(rule_def.args[i - 1], "$$", "acc"), "$", "value_");
                res += s + "(" + args + ");\n";
            }
        }
        res += replace_all(replace_all(rule_def.code, "$$", "acc"), "$", "value_");
        return res;
    }

public:
    parser_generator(const grammar& g): g(g) {}

    std::string operator()() const {
        string_map<std::vector<rule::rule_definition>> rule_defs;
        for (const auto& [nt, def]: g.rules) {
            rule_defs[nt].push_back(def);
        }
        std::string result;
        for (const std::string& nonterminal: g.nonterminals()) {
            result += gen_declaration(nonterminal) + ";\n";
        }
        for (const std::string& nonterminal: g.nonterminals()) {
            result += gen_nonterminal_parser(nonterminal, rule_defs.at(nonterminal));
        }
        result += get_type(g.start) + " parse() {\n";
        result += "next_token();\n";
        if (g.types.contains(g.start)) {
            result += "return ";
        }
        result += g.start + "();\n}\n";
        return result;
    }
};