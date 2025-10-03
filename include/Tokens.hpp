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

    struct Literal {
        using LiteralVal =
            std::variant<double, std::string, bool, std::nullptr_t>;
        LiteralVal value;

        Literal() : value(nullptr) {}

        explicit Literal(LiteralVal val) : value(std::move(val)) {}

        explicit Literal(int int_val) : value(static_cast<double>(int_val)) {}

        explicit Literal(double double_val) : value(double_val) {}

        explicit Literal(bool bool_val) : value(bool_val) {}

        explicit Literal(std::string string_val) : value(string_val) {}

        [[nodiscard]] auto toInt() const -> int {
            if (std::holds_alternative<double>(value)) {
                return static_cast<int>(std::get<double>(value));
            }
            throw std::runtime_error("Expected number for shift.");
        }

        template <typename T>
        [[nodiscard]] auto is() const -> bool {
            return std::holds_alternative<T>(value);
        }

        template <typename T>
        [[nodiscard]] auto as() const -> const T& {
            return std::get<T>(value);
        }

        template <typename T>
        [[nodiscard]] auto as() -> T& {
            return std::get<T>(value);
        }

        template <typename T>
        void setValue(T&& val) {
            value = std::forward<T>(val);
        }

        [[nodiscard]] auto isNil() const -> bool {
            return std::holds_alternative<std::nullptr_t>(value);
        }

        [[nodiscard]] auto isString() const -> bool {
            return std::holds_alternative<std::string>(value);
        }

        [[nodiscard]] auto isNumber() const -> bool {
            return std::holds_alternative<double>(value);
        }

        [[nodiscard]] auto isInt() const -> bool {
            return isNumber() && std::floor(asNumber()) == asNumber();
        }

        [[nodiscard]] auto isBool() const -> bool {
            return std::holds_alternative<bool>(value);
        }

        // Getter for number
        [[nodiscard]] auto asNumber() const -> double {
            if (!isNumber()) {
                throw std::runtime_error("Literal is not a number.");
            }
            return std::get<double>(value);
        }

        // Getter for string
        [[nodiscard]] auto asString() const -> const std::string& {
            if (!isString()) {
                throw std::runtime_error("Literal is not a string.");
            }
            return std::get<std::string>(value);
        }

        // Getter for bool
        [[nodiscard]] auto asBool() const -> bool {
            if (!isBool()) {
                throw std::runtime_error("Literal is not a bool.");
            }
            return std::get<bool>(value);
        }

        // Getter for nil (useful for checking and completeness)
        [[nodiscard]] auto asNil() const -> std::nullptr_t {
            if (!isNil()) {
                throw std::runtime_error("Literal is not nil.");
            }
            return std::get<std::nullptr_t>(value);
        }

        [[nodiscard]] auto stringify() const -> std::string {
            return std::visit(
                [](const auto& val) -> std::string {
                    using T = std::decay_t<decltype(val)>;

                    if constexpr (std::is_same_v<T, double> ||
                                  std::is_same_v<T, std::string>) {
                        return fmt::format("{}", val);

                    } else if constexpr (std::is_same_v<T, bool>) {
                        return val ? "true" : "false";
                    } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
                        return "nil";
                    }
                },
                value);
        }
    };

    struct Token {
        Type        type;
        std::string lexeme;
        uint        line{};
        uint        start{};
        uint        end{};
        Literal     literal;

        Token(Type type, std::string lexeme, Literal::LiteralVal literal,
              uint start, uint end, uint line)
            : type(type),
              lexeme(std::move(lexeme)),
              line(line),
              start(start),
              end(end),
              literal(std::move(literal)) {}

        Token() = default;

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
