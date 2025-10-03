#pragma once

#include "Logger.hpp"
#include "Tokens.hpp"
#include "Visitor.hpp"

#include <utility>

namespace Expr {

    class ExprBase;

    using Expr = std::shared_ptr<const ExprBase>;

    struct Variable {
        const Token::Token name;
        Token::Literal     literal;

        explicit Variable(Token::Token name) : name(std::move(name)) {}

        explicit Variable(Token::Token name, Token::Literal::LiteralVal value)
            : name(std::move(name)), literal(value) {}

        explicit Variable(Token::Token name, Token::Literal value)
            : name(std::move(name)), literal(std::move(value)) {}

        [[nodiscard]] auto val() const -> Token::Literal::LiteralVal {
            return literal.value;
        }

        void setVal(Token::Literal literal) {
            this->literal.value = literal.value;
        }

        void setVal(Token::Literal::LiteralVal value) {
            literal.value = value;
        }
    };

    struct InfixExpr {
        Expr               left;
        const Token::Token operator_;
        Expr               right;

        InfixExpr(Expr left, Token::Token operator_, Expr right)
            : left(std::move(left)),
              operator_(std::move(operator_)),
              right(std::move(right)) {}
    };

    struct GroupExpr {
        Expr expr;

        explicit GroupExpr(Expr expr) : expr(std::move(expr)) {}
    };

    struct PrefixExpr {
        const Token::Token operator_;
        Expr               right;

        explicit PrefixExpr(Token::Token operator_, Expr right)
            : operator_(std::move(operator_)), right(std::move(right)) {}
    };

    struct PostfixExpr {
        Expr               left;
        const Token::Token operator_;

        explicit PostfixExpr(Expr left, Token::Token operator_)
            : left(std::move(left)), operator_(std::move(operator_)) {}
    };

    struct LiteralExpr {
        const Token::Literal literal;

        explicit LiteralExpr(Token::Literal::LiteralVal literalVal)
            : literal(std::move(literalVal)) {}

        explicit LiteralExpr(Token::Literal literal)
            : literal(std::move(literal)) {}
    };

    struct TernaryExpr {
        Expr condition;
        Expr trueExpr;
        Expr falseExpr;

        TernaryExpr(Expr condition, Expr trueExpr, Expr falseExpr)
            : condition(std::move(condition)),
              trueExpr(std::move(trueExpr)),
              falseExpr(std::move(falseExpr)) {}
    };

    template <class R>
    struct Visitor : VisitorBase<Variable, R>,
                     VisitorBase<InfixExpr, R>,
                     VisitorBase<GroupExpr, R>,
                     VisitorBase<LiteralExpr, R>,
                     VisitorBase<PrefixExpr, R>,
                     VisitorBase<PostfixExpr, R>,
                     VisitorBase<TernaryExpr, R> {};

    class ExprBase {
      public:

        using ExprVariant =
            std::variant<Variable, InfixExpr, GroupExpr, LiteralExpr,
                         PrefixExpr, PostfixExpr, TernaryExpr>;

        explicit ExprBase(ExprVariant variant) : expr_(std::move(variant)) {}

        template <typename R>
        [[nodiscard]] auto accept(const Visitor<R>& visitor) const -> R {
            if (expr_.valueless_by_exception()) {
                Logger::getLogger().error("Valueless variant in ExprBase");
            }
            return std::visit(
                [&visitor](const auto& inner) -> R {
                    using T = std::decay_t<decltype(inner)>;
                    return static_cast<const VisitorBase<T, R>*>(&visitor)
                        ->visit(inner);
                },
                expr_);
        }

        template <typename T>
        [[nodiscard]] auto is() const -> bool {
            return std::holds_alternative<T>(expr_);
        }

        template <typename T>
        [[nodiscard]] auto as() const -> const T& {
            return std::get<T>(expr_);
        }

        template <typename T>
        [[nodiscard]] auto as() -> T& {
            return std::get<T>(expr_);
        }

        template <typename T>
        void set(T&& val) {
            expr_ = std::forward<T>(val);
        }

      private:

        ExprVariant expr_;
    };

    template <typename T>
    auto makeExpr(T&& expr) -> Expr {
        return std::make_shared<ExprBase>(
            ExprBase::ExprVariant{std::forward<T>(expr)});
    }
}  // namespace Expr
