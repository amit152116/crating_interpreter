#pragma once
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include "TokenType.hpp"

#include <string>
#include <utility>
#include <variant>

namespace Token {

    enum class Type : std::uint8_t;

    struct LiteralValue {
        using Literal = std::variant<int, double, std::string, std::nullptr_t>;
        Literal value;

        LiteralValue() : value(nullptr) {}

        explicit LiteralValue(Literal val) : value(std::move(val)) {}

        explicit LiteralValue(int num) : value(num) {}

        explicit LiteralValue(double num) : value(num) {}

        explicit LiteralValue(std::string str) : value(std::move(str)) {}

        explicit LiteralValue(std::nullptr_t) : value(nullptr) {}

        [[nodiscard]] auto isNil() const -> bool {
            return std::holds_alternative<std::nullptr_t>(value);
        }

        [[nodiscard]] auto isString() const -> bool {
            return std::holds_alternative<std::string>(value);
        }

        [[nodiscard]] auto isNumber() const -> bool {
            return std::holds_alternative<double>(value) ||
                   std::holds_alternative<int>(value);
        }

        [[nodiscard]] auto stringify() const -> std::string {
            return std::visit(
                [](const auto& val) -> std::string {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        return fmt::format("\"{}\"", val);
                    } else if constexpr (std::is_same_v<T, double> ||
                                         std::is_same_v<T, int>) {
                        return fmt::format("{}", val);
                    } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
                        return "nil";
                    }
                },
                value);
        }
    };

    struct Token {
        Type         type;
        std::string  lexeme;
        uint         line;
        uint         start;
        uint         end;
        LiteralValue literal;

        Token(Type type, std::string lexeme, LiteralValue::Literal literal,
              uint start, uint end, uint line)
            : type(type),
              lexeme(std::move(lexeme)),
              line(line),
              start(start),
              end(end),
              literal(std::move(literal)) {}

        [[nodiscard]] auto toString() const -> std::string {
            return fmt::format("{}", *this);
        }
    };

}  // namespace Token

// Add fmt formatting support
template <>
struct fmt::formatter<Token::Type> {
    static constexpr auto parse(format_parse_context& ctx)
        -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const Token::Type& type, FormatContext& ctx)
        -> decltype(ctx.out()) {
        std::string type_str = toString(type);
        return fmt::format_to(ctx.out(), "{}", type_str);
    }
};

template <>
struct fmt::formatter<Token::Token> {
    static constexpr auto parse(format_parse_context& ctx)
        -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const Token::Token& token, FormatContext& ctx)
        -> decltype(ctx.out()) {
        return fmt::format_to(
            ctx.out(), "Token({}, `{}`, {}, [line {}({}:{})])", token.type,
            token.lexeme, token.literal.stringify(), token.line, token.start,
            token.end);
    }
};
