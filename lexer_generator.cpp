#include "grammar.h"

class lexer_generator {
    const grammar& g;

    std::string generate_union() const {
        std::string res = "union {\n";
        std::unordered_set<std::string> types;
        for (const auto& [k, t]: g.types) {
            types.insert(t);
        }
        for (const auto& t: types) {
            res += t + " " + t + "_field;\n";
        }
        res += "} cur_value;\n";
        return res;
    }

    std::string generate_enum() const {
        std::string res = "enum token_type {\n";
        for (const std::string& token: g.tokens) {
            res += token + ",\n";
        }
        res += "EOF,\n";
        res += "} cur_token;\n";
        return res;
    }

    std::string gen_next_token() const {
        std::string res = generate_consume();
        res += "token_type get_next_token() {\n";
        res += "std::string input = PARSER_INPUT";
        res += "while (true) {\n";
        res += "cur_match=\"\";\n";
        res += "if (input.empty()) return EOF;\n";
        for (const auto& [pattern, action]: g.lexer_rules) {
            res += "if (consume(\"" + pattern + "\", input)) ";
            res += action;
        }
        res += "}\n}\n";
        return res;
    }

    static std::string generate_consume() {
        return "bool consume(std::string pattern, std::string& s) {\n\
        std::smatch sm;\n\
        auto re = std::regex(\"^\" + pattern, std::regex::basic);\n\
        if (std::regex_search(s, sm, re)) {\n\
        cur_match = sm.str();\n\
        s = sm.suffix();\n\
        return true;\n\
        }\n\
        return false;\n\
        }\n";
    }

public:
    lexer_generator(const grammar& g): g(g) {}

    std::string operator()() const {
        std::string res = "std::string PARSER_INPUT;\n";
        res += "std::string cur_match;\n";
        res += generate_union();
        res += generate_enum();
        res += gen_next_token();
        res += "void next_token() {\n";
        res += "cur_token = get_next_token();\n";
        res += "}\n";
        return res;
    }
};