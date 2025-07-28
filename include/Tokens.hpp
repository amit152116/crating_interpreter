#pragma once
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <string>
#include <unordered_map>
#include <utility>
#include <variant>

namespace Krypton {
    using Literal = std::variant<std::string, double, std::nullptr_t>;

    enum class TokenType : std::uint8_t {

        // Literals
        NUMBER,      // Numeric literal
        IDENTIFIER,  // Identifier (variable, function name, etc.)
        STRING,      // String literal

        // Single-character tokens.
        LEFT_PAREN,     // '('
        RIGHT_PAREN,    // ')'
        LEFT_BRACE,     // '{'
        RIGHT_BRACE,    // '}'
        LEFT_BRACKET,   // '['
        RIGHT_BRACKET,  // ']'
        COMMA,          // ','
        DOT,            // '.'
        SEMICOLON,      // ';'
        QUESTION,       // '?'
        COLON,          // ':'
        PERCENT,        // '%'
        DOLLAR,         // '$'
        CARET,          // '^'
        TILDE,          // '~'

        // One or two character tokens.
        BANG,                 // '!'
        BANG_EQUAL,           // '!='
        EQUAL,                // '='
        EQUAL_EQUAL,          // '=='
        GREATER,              // '>'
        GREATER_GREATER,      // '>>'
        GREATER_EQUAL,        // '>='
        LESS,                 // '<'
        LESS_LESS,            // '<<'
        LESS_EQUAL,           // '<='
        AMPERSAND,            // '&'
        AMPERSAND_AMPERSAND,  // '&&'
        PIPE,                 // '|'
        PIPE_PIPE,            // '||'
        PLUS,                 // '+'
        PLUS_PLUS,            // '++'
        PLUS_EQUAL,           // '+='
        MINUS,                // '-'
        MINUS_MINUS,          // '--'
        MINUS_EQUAL,          // '-='
        STAR,                 // '*'
        STAR_STAR,            // '**'
        STAR_EQUAL,           // '*='
        SLASH,                // '/'
        SLASH_EQUAL,          // '/='

        // Keywords.
        IF,        // 'if'
        OR,        // 'or'
        VAR,       // 'var'
        VAL,       // 'val'
        AND,       // 'and'
        FOR,       // 'for'
        NIL,       // 'nil'
        TRY,       // 'try'
        CASE,      // 'case'
        ELSE,      // 'else'
        TRUE,      // 'true'
        THIS,      // 'this'
        INIT,      // 'init'
        WHILE,     // 'while'
        BREAK,     // 'break'
        CATCH,     // 'catch'
        CLASS,     // 'class'
        CONST,     // 'const'
        FALSE,     // 'false'
        PRINT,     // 'print'
        THROW,     // 'throw'
        SUPER,     // 'super'
        SWITCH,    // 'switch'
        RETURN,    // 'return'
        FINALLY,   // 'finally'
        CONTINUE,  // 'continue'
        FUNCTION,  // 'function'

        // Special tokens
        EOF_,  // End of file
        ERROR  // Error token
    };

    inline auto getKeywordMap() -> std::unordered_map<std::string, TokenType> {
        static const std::unordered_map<std::string, TokenType> keywords = {
            {"if", TokenType::IF},           {"or", TokenType::OR},
            {"var", TokenType::VAR},         {"val", TokenType::VAL},
            {"and", TokenType::AND},         {"for", TokenType::FOR},
            {"nil", TokenType::NIL},         {"try", TokenType::TRY},
            {"case", TokenType::CASE},       {"else", TokenType::ELSE},
            {"true", TokenType::TRUE},       {"this", TokenType::THIS},
            {"init", TokenType::INIT},       {"while", TokenType::WHILE},
            {"break", TokenType::BREAK},     {"catch", TokenType::CATCH},
            {"class", TokenType::CLASS},     {"const", TokenType::CONST},
            {"false", TokenType::FALSE},     {"print", TokenType::PRINT},
            {"throw", TokenType::THROW},     {"super", TokenType::SUPER},
            {"switch", TokenType::SWITCH},   {"return", TokenType::RETURN},
            {"finally", TokenType::FINALLY}, {"continue", TokenType::CONTINUE},
            {"func", TokenType::FUNCTION}};
        return keywords;
    }

    inline auto toString(TokenType type) -> std::string {
        std::string type_str;
        switch (type) {
            case TokenType::NUMBER:
                type_str = "NUMBER";
                break;
            case TokenType::IDENTIFIER:
                type_str = "IDENTIFIER";
                break;
            case TokenType::STRING:
                type_str = "STRING";
                break;
            case TokenType::LEFT_PAREN:
                type_str = "LEFT_PAREN";
                break;
            case TokenType::RIGHT_PAREN:
                type_str = "RIGHT_PAREN";
                break;
            case TokenType::LEFT_BRACE:
                type_str = "LEFT_BRACE";
                break;
            case TokenType::RIGHT_BRACE:
                type_str = "RIGHT_BRACE";
                break;
            case TokenType::LEFT_BRACKET:
                type_str = "LEFT_BRACKET";
                break;
            case TokenType::RIGHT_BRACKET:
                type_str = "RIGHT_BRACKET";
                break;
            case TokenType::COMMA:
                type_str = "COMMA";
                break;
            case TokenType::DOT:
                type_str = "DOT";
                break;
            case TokenType::MINUS:
                type_str = "MINUS";
                break;
            case TokenType::PLUS:
                type_str = "PLUS";
                break;
            case TokenType::SEMICOLON:
                type_str = "SEMICOLON";
                break;
            case TokenType::SLASH:
                type_str = "SLASH";
                break;
            case TokenType::STAR:
                type_str = "STAR";
                break;
            case TokenType::QUESTION:
                type_str = "QUESTION";
                break;
            case TokenType::COLON:
                type_str = "COLON";
                break;
            case TokenType::PERCENT:
                type_str = "PERCENT";
                break;
            case TokenType::BANG:
                type_str = "BANG";
                break;
            case TokenType::BANG_EQUAL:
                type_str = "BANG_EQUAL";
                break;
            case TokenType::EQUAL:
                type_str = "EQUAL";
                break;
            case TokenType::EQUAL_EQUAL:
                type_str = "EQUAL_EQUAL";
                break;
            case TokenType::GREATER:
                type_str = "GREATER";
                break;
            case TokenType::GREATER_EQUAL:
                type_str = "GREATER_EQUAL";
                break;
            case TokenType::LESS:
                type_str = "LESS";
                break;
            case TokenType::LESS_EQUAL:
                type_str = "LESS_EQUAL";
                break;
            case TokenType::PLUS_PLUS:
                type_str = "PLUS_PLUS";
                break;
            case TokenType::MINUS_MINUS:
                type_str = "MINUS_MINUS";
                break;
            case TokenType::STAR_STAR:
                type_str = "STAR_STAR";
                break;
            case TokenType::AND:
                type_str = "AND";
                break;
            case TokenType::CLASS:
                type_str = "CLASS";
                break;
            case TokenType::ELSE:
                type_str = "ELSE";
                break;
            case TokenType::FALSE:
                type_str = "FALSE";
                break;
            case TokenType::FUNCTION:
                type_str = "FUNCTION";
                break;
            case TokenType::FOR:
                type_str = "FOR";
                break;
            case TokenType::IF:
                type_str = "IF";
                break;
            case TokenType::NIL:
                type_str = "NIL";
                break;
            case TokenType::OR:
                type_str = "OR";
                break;
            case TokenType::PRINT:
                type_str = "PRINT";
                break;
            case TokenType::RETURN:
                type_str = "RETURN";
                break;
            case TokenType::SUPER:
                type_str = "SUPER";
                break;
            case TokenType::THIS:
                type_str = "THIS";
                break;
            case TokenType::TRUE:
                type_str = "TRUE";
                break;
            case TokenType::INIT:
                type_str = "INIT";
                break;
            case TokenType::VAR:
                type_str = "VAR";
                break;
            case Krypton::TokenType::VAL:
                type_str = "VAL";
                break;
            case TokenType::WHILE:
                type_str = "WHILE";
                break;
            case TokenType::BREAK:
                type_str = "BREAK";
                break;
            case TokenType::CONTINUE:
                type_str = "CONTINUE";
                break;
            case TokenType::EOF_:
                type_str = "EOF";
                break;
            case TokenType::ERROR:
                type_str = "ERROR";
                break;
            case TokenType::DOLLAR:
                type_str = "DOLLAR";
                break;
            case TokenType::CARET:
                type_str = "CARET";
                break;
            case TokenType::TILDE:
                type_str = "TILDE";
                break;
            case TokenType::GREATER_GREATER:
                type_str = "GREATER_GREATER";
                break;
            case TokenType::LESS_LESS:
                type_str = "LESS_LESS";
                break;
            case TokenType::AMPERSAND:
                type_str = "AMPERSAND";
                break;
            case TokenType::AMPERSAND_AMPERSAND:
                type_str = "AMPERSAND_AMPERSAND";
                break;
            case TokenType::PIPE:
                type_str = "PIPE";
                break;
            case TokenType::PIPE_PIPE:
                type_str = "PIPE_PIPE";
                break;
            case TokenType::PLUS_EQUAL:
                type_str = "PLUS_EQUAL";
                break;
            case TokenType::MINUS_EQUAL:
                type_str = "MINUS_EQUAL";
                break;
            case TokenType::STAR_EQUAL:
                type_str = "STAR_EQUAL";
                break;
            case TokenType::SLASH_EQUAL:
                type_str = "SLASH_EQUAL";
                break;
            case TokenType::TRY:
                type_str = "TRY";
                break;
            case TokenType::CATCH:
                type_str = "CATCH";
                break;
            case TokenType::FINALLY:
                type_str = "FINALLY";
                break;
            case TokenType::THROW:
                type_str = "THROW";
                break;
            case TokenType::SWITCH:
                type_str = "SWITCH";
                break;
            case TokenType::CASE:
                type_str = "CASE";
                break;
            case TokenType::CONST:
                type_str = "CONST";
                break;
            default:
                type_str = "UNKNOWN";
                break;
        }
        return type_str;
    }

    struct Token {
        TokenType   type;
        std::string lexeme;
        uint        line;
        uint        start;
        uint        end;
        Literal     literal;

        Token(TokenType type, std::string lexeme, Literal literal, uint start,
              uint end, uint line)
            : type(type),
              lexeme(std::move(lexeme)),
              line(line),
              start(start),
              end(end),
              literal(std::move(literal)) {}
    };
}  // namespace Krypton

// Add fmt formatting support
template <>
struct fmt::formatter<Krypton::TokenType> {
    static constexpr auto parse(format_parse_context& ctx)
        -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const Krypton::TokenType& type, FormatContext& ctx)
        -> decltype(ctx.out()) {
        std::string type_str = toString(type);
        return fmt::format_to(ctx.out(), "{}", type_str);
    }
};

template <>
struct fmt::formatter<Krypton::Token> {
    static constexpr auto parse(format_parse_context& ctx)
        -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const Krypton::Token& token, FormatContext& ctx)
        -> decltype(ctx.out()) {
        std::string literal_str;
        std::visit(
            [&literal_str](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    literal_str = fmt::format("\"{}\"", val);
                } else if constexpr (std::is_same_v<T, double>) {
                    literal_str = fmt::format("{}", val);
                } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    literal_str = "nil";
                }
            },
            token.literal);

        return fmt::format_to(ctx.out(), "Token({}, `{}`, {}, line {}[{}:{}])",
                              token.type, token.lexeme, literal_str, token.line,
                              token.start, token.end);
    }
};
