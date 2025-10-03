#pragma once

#include "Expr.hpp"
#include "Logger.hpp"

#include <utility>

namespace Stmt {

    class StmtBase;

    using Stmt = std::shared_ptr<const StmtBase>;

    struct Expression {
        Expr::Expr expression;

        explicit Expression(Expr::Expr expression)
            : expression(std::move(expression)) {}
    };

    struct Print {
        Expr::Expr expression;

        explicit Print(Expr::Expr expression)
            : expression(std::move(expression)) {}
    };

    struct Variable {
        Expr::Expr         initializer;
        const Token::Token name;
        const Token::Token type;

        explicit Variable(Token::Token name, Expr::Expr initializer,
                          Token::Token type)
            : initializer(std::move(initializer)),
              name(std::move(name)),
              type(std::move(type)) {};
    };

    template <class R>
    struct Visitor : VisitorBase<Expression, R>,
                     VisitorBase<Variable, R>,
                     VisitorBase<Print, R> {};

#define OVERRIDE_STMT_VISITOR                                             \
    [[nodiscard]] auto visit(const Expression& stmt) const -> void final; \
    [[nodiscard]] auto visit(const Variable& stmt) const -> void final;   \
    [[nodiscard]] auto visit(const Print& stmt) const -> void final;

    class StmtBase {
      public:

        using StmtVariant = std::variant<Expression, Variable, Print>;

        explicit StmtBase(StmtVariant variant) : stmt_(std::move(variant)) {}

        template <typename R>
        [[nodiscard]] auto accept(const Visitor<R>& visitor) const -> R {
            if (stmt_.valueless_by_exception()) {
                Logger::getLogger().error("Valueless variant in StmtBase");
            }
            return std::visit(
                [&visitor](const auto& inner) -> R {
                    using T = std::decay_t<decltype(inner)>;
                    return static_cast<const VisitorBase<T, R>*>(&visitor)
                        ->visit(inner);
                },
                stmt_);
        }

        template <typename T>
        [[nodiscard]] auto is() const -> bool {
            return std::holds_alternative<T>(stmt_);
        }

        template <typename T>
        [[nodiscard]] auto as() const -> const T& {
            return std::get<T>(stmt_);
        }

        template <typename T>
        [[nodiscard]] auto as() -> T& {
            return std::get<T>(stmt_);
        }

        template <typename T>
        void set(T&& val) {
            stmt_ = std::forward<T>(val);
        }

      private:

        StmtVariant stmt_;
    };

    template <typename T>
    auto makeStmt(T&& stmt) -> Stmt {
        return std::make_shared<StmtBase>(
            StmtBase::StmtVariant{std::forward<T>(stmt)});
    }
}  // namespace Stmt
