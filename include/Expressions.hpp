#pragma once

#include "Logger.hpp"
#include "Tokens.hpp"

#include <utility>

namespace Expression {
    class ExprBase;
    class Binary;
    class Grouping;
    class Literal;
    class Unary;

    using Expr = std::shared_ptr<ExprBase>;

    template <typename T, typename... Args>
    auto makeExpr(Args&&... args) -> Expr {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template <class T>
    class VisitorBase {
      public:

        VisitorBase()                             = default;
        virtual ~VisitorBase()                    = default;
        virtual auto visit(const T& expr) -> void = 0;
    };

    class Visitor : public VisitorBase<Binary>,
                    public VisitorBase<Grouping>,
                    public VisitorBase<Literal>,
                    public VisitorBase<Unary> {
      public:

        Visitor() = default;
    };

    class ExprBase {
      public:

        ExprBase()          = default;
        virtual ~ExprBase() = default;

        virtual void accept(Visitor& visitor) = 0;

      protected:

        template <typename T>
        static auto acceptDecoupled(T& element, Visitor* visitor) -> void {
            if (auto* casted = dynamic_cast<VisitorBase<T>*>(visitor)) {
                casted->visit(element);
            }
        }
    };

#define DEFINE_ACCEPT                        \
    void accept(Visitor& visitor) override { \
        acceptDecoupled(*this, &visitor);    \
    }

    class Binary : public ExprBase {
      public:

        Expr               _left;
        const Token::Token _operator;
        Expr               _right;

        explicit Binary(Expr left, Token::Token operator_, Expr right)
            : _left(std::move(left)),
              _operator(std::move(operator_)),
              _right(std::move(right)) {}

        ~Binary() override = default;

        DEFINE_ACCEPT;
    };

    class Grouping : public ExprBase {
      public:

        Expr _expr;

        explicit Grouping(Expr expr) : _expr(std::move(expr)) {}

        ~Grouping() override = default;

        DEFINE_ACCEPT;
    };

    class Unary : public ExprBase {
      public:

        const Token::Token _operator;
        Expr               _right;

        explicit Unary(Token::Token operator_, Expr right)
            : _operator(std::move(operator_)), _right(std::move(right)) {}

        ~Unary() override = default;

        DEFINE_ACCEPT;
    };

    class Literal : public ExprBase {
      public:

        const Token::Literal _literal;

        explicit Literal(Token::Literal::LiteralVal literal)
            : _literal(std::move(literal)) {}

        ~Literal() override = default;

        DEFINE_ACCEPT;
    };

#define OVERRIDE_VISITORS                              \
    auto visit(const Binary& expr) -> void override;   \
    auto visit(const Grouping& expr) -> void override; \
    auto visit(const Literal& expr) -> void override;  \
    auto visit(const Unary& expr) -> void override;

    class AstPrinter : public Visitor {
      public:

        OVERRIDE_VISITORS

        auto print(Expr expr) -> void {
            _result = {};
            expr->accept(*this);
            _logger.info("Expression: {}", _result);
        }

        AstPrinter() = default;

        ~AstPrinter() override = default;

      private:

        std::string     _result;
        Logger::Logger& _logger = Logger::Logger::instance();

        template <typename... ExprPtrs>
        auto parenthesize(const std::string& name, const ExprPtrs&... exprs)
            -> void {
            _result.push_back('(');
            _result.append(name);
            (
                [&] {
                    static_assert(std::is_same_v<std::decay_t<ExprPtrs>, Expr>,
                                  "Arguments must be of type ExprDecl");

                    _result.append(" ");
                    exprs->accept(*this);
                }(),
                ...);
            _result.append(")");
        }
    };

}  // namespace Expression
