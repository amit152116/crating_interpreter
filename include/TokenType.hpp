
#pragma once
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <string>
#include <unordered_map>

namespace Token {

    enum class Type : std::uint8_t {

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

    inline auto getKeywordMap() -> std::unordered_map<std::string, Type> {
        static const std::unordered_map<std::string, Type> keywords = {
            {"if", Type::IF},           {"or", Type::OR},
            {"var", Type::VAR},         {"val", Type::VAL},
            {"and", Type::AND},         {"for", Type::FOR},
            {"nil", Type::NIL},         {"try", Type::TRY},
            {"case", Type::CASE},       {"else", Type::ELSE},
            {"true", Type::TRUE},       {"this", Type::THIS},
            {"init", Type::INIT},       {"while", Type::WHILE},
            {"break", Type::BREAK},     {"catch", Type::CATCH},
            {"class", Type::CLASS},     {"const", Type::CONST},
            {"false", Type::FALSE},     {"print", Type::PRINT},
            {"throw", Type::THROW},     {"super", Type::SUPER},
            {"switch", Type::SWITCH},   {"return", Type::RETURN},
            {"finally", Type::FINALLY}, {"continue", Type::CONTINUE},
            {"func", Type::FUNCTION}};
        return keywords;
    }

    inline auto toString(Type type) -> std::string {
        std::string type_str;
        switch (type) {
            case Type::NUMBER:
                type_str = "NUMBER";
                break;
            case Type::IDENTIFIER:
                type_str = "IDENTIFIER";
                break;
            case Type::STRING:
                type_str = "STRING";
                break;
            case Type::LEFT_PAREN:
                type_str = "LEFT_PAREN";
                break;
            case Type::RIGHT_PAREN:
                type_str = "RIGHT_PAREN";
                break;
            case Type::LEFT_BRACE:
                type_str = "LEFT_BRACE";
                break;
            case Type::RIGHT_BRACE:
                type_str = "RIGHT_BRACE";
                break;
            case Type::LEFT_BRACKET:
                type_str = "LEFT_BRACKET";
                break;
            case Type::RIGHT_BRACKET:
                type_str = "RIGHT_BRACKET";
                break;
            case Type::COMMA:
                type_str = "COMMA";
                break;
            case Type::DOT:
                type_str = "DOT";
                break;
            case Type::MINUS:
                type_str = "MINUS";
                break;
            case Type::PLUS:
                type_str = "PLUS";
                break;
            case Type::SEMICOLON:
                type_str = "SEMICOLON";
                break;
            case Type::SLASH:
                type_str = "SLASH";
                break;
            case Type::STAR:
                type_str = "STAR";
                break;
            case Type::QUESTION:
                type_str = "QUESTION";
                break;
            case Type::COLON:
                type_str = "COLON";
                break;
            case Type::PERCENT:
                type_str = "PERCENT";
                break;
            case Type::BANG:
                type_str = "BANG";
                break;
            case Type::BANG_EQUAL:
                type_str = "BANG_EQUAL";
                break;
            case Type::EQUAL:
                type_str = "EQUAL";
                break;
            case Type::EQUAL_EQUAL:
                type_str = "EQUAL_EQUAL";
                break;
            case Type::GREATER:
                type_str = "GREATER";
                break;
            case Type::GREATER_EQUAL:
                type_str = "GREATER_EQUAL";
                break;
            case Type::LESS:
                type_str = "LESS";
                break;
            case Type::LESS_EQUAL:
                type_str = "LESS_EQUAL";
                break;
            case Type::PLUS_PLUS:
                type_str = "PLUS_PLUS";
                break;
            case Type::MINUS_MINUS:
                type_str = "MINUS_MINUS";
                break;
            case Type::STAR_STAR:
                type_str = "STAR_STAR";
                break;
            case Type::AND:
                type_str = "AND";
                break;
            case Type::CLASS:
                type_str = "CLASS";
                break;
            case Type::ELSE:
                type_str = "ELSE";
                break;
            case Type::FALSE:
                type_str = "FALSE";
                break;
            case Type::FUNCTION:
                type_str = "FUNCTION";
                break;
            case Type::FOR:
                type_str = "FOR";
                break;
            case Type::IF:
                type_str = "IF";
                break;
            case Type::NIL:
                type_str = "NIL";
                break;
            case Type::OR:
                type_str = "OR";
                break;
            case Type::PRINT:
                type_str = "PRINT";
                break;
            case Type::RETURN:
                type_str = "RETURN";
                break;
            case Type::SUPER:
                type_str = "SUPER";
                break;
            case Type::THIS:
                type_str = "THIS";
                break;
            case Type::TRUE:
                type_str = "TRUE";
                break;
            case Type::INIT:
                type_str = "INIT";
                break;
            case Type::VAR:
                type_str = "VAR";
                break;
            case Type::VAL:
                type_str = "VAL";
                break;
            case Type::WHILE:
                type_str = "WHILE";
                break;
            case Type::BREAK:
                type_str = "BREAK";
                break;
            case Type::CONTINUE:
                type_str = "CONTINUE";
                break;
            case Type::EOF_:
                type_str = "EOF";
                break;
            case Type::ERROR:
                type_str = "ERROR";
                break;
            case Type::DOLLAR:
                type_str = "DOLLAR";
                break;
            case Type::CARET:
                type_str = "CARET";
                break;
            case Type::TILDE:
                type_str = "TILDE";
                break;
            case Type::GREATER_GREATER:
                type_str = "GREATER_GREATER";
                break;
            case Type::LESS_LESS:
                type_str = "LESS_LESS";
                break;
            case Type::AMPERSAND:
                type_str = "AMPERSAND";
                break;
            case Type::AMPERSAND_AMPERSAND:
                type_str = "AMPERSAND_AMPERSAND";
                break;
            case Type::PIPE:
                type_str = "PIPE";
                break;
            case Type::PIPE_PIPE:
                type_str = "PIPE_PIPE";
                break;
            case Type::PLUS_EQUAL:
                type_str = "PLUS_EQUAL";
                break;
            case Type::MINUS_EQUAL:
                type_str = "MINUS_EQUAL";
                break;
            case Type::STAR_EQUAL:
                type_str = "STAR_EQUAL";
                break;
            case Type::SLASH_EQUAL:
                type_str = "SLASH_EQUAL";
                break;
            case Type::TRY:
                type_str = "TRY";
                break;
            case Type::CATCH:
                type_str = "CATCH";
                break;
            case Type::FINALLY:
                type_str = "FINALLY";
                break;
            case Type::THROW:
                type_str = "THROW";
                break;
            case Type::SWITCH:
                type_str = "SWITCH";
                break;
            case Type::CASE:
                type_str = "CASE";
                break;
            case Type::CONST:
                type_str = "CONST";
                break;
            default:
                type_str = "UNKNOWN";
                break;
        }
        return type_str;
    }
}  // namespace Token
