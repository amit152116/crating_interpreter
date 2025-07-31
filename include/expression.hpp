#pragma once

#include "Logger.hpp"
#include "Tokens.hpp"

#include <sstream>
#include <utility>

namespace Parser2 {

    struct Binary;
    struct Grouping;
    struct Literal;
    struct Unary;
    class ExprBase;

#define OVERRIDE_VISITOR_TYPE                                                 \
    [[nodiscard]] auto visit(const Binary& expr) const -> ReturnType final;   \
    [[nodiscard]] auto visit(const Grouping& expr) const -> ReturnType final; \
    [[nodiscard]] auto visit(const Literal& expr) const -> ReturnType final;  \
    [[nodiscard]] auto visit(const Unary& expr) const -> ReturnType final;

    using ExprDecl = std::unique_ptr<const ExprBase>;

    template <typename T, typename... Args>
    auto makeExpr(Args&&... args) -> ExprDecl {
        return std::make_unique<ExprBase>(T{std::forward<Args>(args)...});
    }

    struct Binary {
        ExprDecl           _left;
        const Token::Token _operator;
        ExprDecl           _right;

        Binary(ExprDecl left, Token::Token operator_, ExprDecl right)
            : _left(std::move(left)),
              _operator(std::move(operator_)),
              _right(std::move(right)) {}
    };

    struct Grouping {
        ExprDecl _expr;

        explicit Grouping(ExprDecl expr) : _expr(std::move(expr)) {}
    };

    struct Unary {
        const Token::Token _operator;
        ExprDecl           _right;

        explicit Unary(Token::Token operator_, ExprDecl right)
            : _operator(std::move(operator_)), _right(std::move(right)) {}
    };

    struct Literal {
        const Token::LiteralValue _literal;

        explicit Literal(Token::LiteralValue::Literal literal)
            : _literal(std::move(literal)) {}
    };

    template <class T, typename R>
    struct VisitorBase {
        virtual ~VisitorBase()                                     = default;
        [[nodiscard]] virtual auto visit(const T& expr) const -> R = 0;
    };

    template <class R>
    struct Visitor : VisitorBase<Binary, R>,
                     VisitorBase<Grouping, R>,
                     VisitorBase<Literal, R>,
                     VisitorBase<Unary, R> {
        using ReturnType = R;
    };

    class ExprBase {
      public:

        using ExprVariant = std::variant<Binary, Grouping, Literal, Unary>;

        template <typename T>
        explicit ExprBase(T&& expr) : expr_(std::forward<T>(expr)) {}

        template <typename R>
        [[nodiscard]] auto accept(const Visitor<R>& visitor) const -> R {
            return std::visit(
                [&visitor](const auto& inner) -> R {
                    using T = std::decay_t<decltype(inner)>;
                    return static_cast<const VisitorBase<T, R>*>(&visitor)
                        ->visit(inner);
                },
                expr_);
        }

      private:

        ExprVariant expr_;
    };

    class AstPrinter : Visitor<std::string> {
      public:

        AstPrinter() = default;

        auto print(ExprDecl expr) -> void {
            auto result = expr->accept(*this);
            logger_.info("AST Expression: {}", result);
        }

      private:

        OVERRIDE_VISITOR_TYPE

        template <typename... ExprPtrs>
        auto expand_exprs(ExprPtrs&&... exprs) const -> ReturnType {
            static_assert(
                (std::is_same_v<std::decay_t<ExprPtrs>, ExprDecl> && ...),
                "Arguments must be of type ExprDecl");

            if constexpr (sizeof...(ExprPtrs) == 0) {
                return "";
            } else {
                return (... + exprs->accept(*this));
            }
        }

        template <typename... ExprPtrs>
        auto parenthesize(std::string name, ExprPtrs&... exprs) const
            -> ReturnType {
            std::stringstream ss;
            ss << "(" << name;
            ss << " " << expand_exprs(std::forward<ExprPtrs>(exprs)...);

            ss << ")";
            return ss.str();
        }

        Logger::Logger& logger_ = Logger::Logger::instance();
    };

    struct RPNPrinter : Visitor<std::string> {
      public:

        auto print(ExprDecl expr) -> void {
            auto result = expr->accept(*this);
            logger_.info("RPN Expression: {}", result);
        }

      private:

        OVERRIDE_VISITOR_TYPE
        template <typename... ExprPtrs>
        auto expand_exprs(ExprPtrs&&... exprs) const -> ReturnType {
            static_assert(
                (std::is_same_v<std::decay_t<ExprPtrs>, ExprDecl> && ...),
                "Arguments must be of type ExprDecl");

            if constexpr (sizeof...(ExprPtrs) == 0) {
                return "";
            } else {
                return (... + exprs->accept(*this));
            }
        }

        template <typename... ExprPtrs>
        auto parenthesize(std::string name, ExprPtrs&... exprs) const
            -> ReturnType {
            std::stringstream ss;
            ss << expand_exprs(std::forward<ExprPtrs>(exprs)...);

            ss << name;
            ss << " ";
            return ss.str();
        }

        Logger::Logger& logger_ = Logger::Logger::instance();
    };

}  // namespace Parser2
