#pragma once

#include "Logger.hpp"
#include "Tokens.hpp"

#include <sstream>
#include <utility>

namespace Expr {

    class ExprBase;

    using Expr = std::shared_ptr<const ExprBase>;

    template <typename T, typename... Args>
    auto makeExpr(Args&&... args) -> Expr {
        return std::make_shared<ExprBase>(T{std::forward<Args>(args)...});
    }

    struct VariableExpr {
        const Token::Token name;
        Token::Literal     literal;

        explicit VariableExpr(Token::Token name) : name(std::move(name)) {}

        explicit VariableExpr(Token::Token               name,
                              Token::Literal::LiteralVal value)
            : name(std::move(name)), literal(value) {}

        explicit VariableExpr(Token::Token name, Token::Literal value)
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

    template <class T, class R>
    struct VisitorBase {
        virtual ~VisitorBase()                                     = default;
        [[nodiscard]] virtual auto visit(const T& expr) const -> R = 0;
    };

#define OVERRIDE_VISITOR_TYPE                                                  \
    [[nodiscard]] auto visit(const VariableExpr& expr) const                   \
        -> ReturnType  final;                                                  \
    [[nodiscard]] auto visit(const InfixExpr& expr) const -> ReturnType final; \
    [[nodiscard]] auto visit(const GroupExpr& expr) const -> ReturnType final; \
    [[nodiscard]] auto visit(const LiteralExpr& expr) const                    \
        -> ReturnType  final;                                                  \
    [[nodiscard]] auto visit(const PrefixExpr& expr) const                     \
        -> ReturnType  final;                                                  \
    [[nodiscard]] auto visit(const PostfixExpr& expr) const                    \
        -> ReturnType  final;                                                  \
    [[nodiscard]] auto visit(const TernaryExpr& expr) const -> ReturnType final;

    template <class R>
    struct Visitor : VisitorBase<VariableExpr, R>,
                     VisitorBase<InfixExpr, R>,
                     VisitorBase<GroupExpr, R>,
                     VisitorBase<LiteralExpr, R>,
                     VisitorBase<PrefixExpr, R>,
                     VisitorBase<PostfixExpr, R>,
                     VisitorBase<TernaryExpr, R> {
        using ReturnType = R;
    };

    class ExprBase {
      public:

        using ExprVariant =
            std::variant<VariableExpr, InfixExpr, GroupExpr, LiteralExpr,
                         PrefixExpr, PostfixExpr, TernaryExpr>;

        template <typename T>
        explicit ExprBase(T&& expr) : expr_(std::forward<T>(expr)) {}

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

    class AstPrinter : Visitor<std::string> {
      public:

        AstPrinter() = default;

        auto print(Expr expr) const -> void;

      private:

        OVERRIDE_VISITOR_TYPE

        template <typename... ExprPtrs>
        auto parenthesize(std::string name, ExprPtrs&&... exprs) const
            -> ReturnType {
            constexpr bool AllValid =
                ((std::is_same_v<std::decay_t<ExprPtrs>, Expr> ||
                  std::is_same_v<std::decay_t<ExprPtrs>, std::string>) &&
                 ...);
            static_assert(AllValid, "Each type must be Expr or std::string");

            std::stringstream ss;
            ss << " (" << name;

            (
                [&]() {
                    using T = std::decay_t<decltype(exprs)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        ss << " " << exprs;
                    } else {
                        ss << exprs->accept(*this);
                    }
                }(),
                ...);
            ss << ")";

            return ss.str();
        }

        Logger::Logger& logger_ = Logger::Logger::instance();
    };

    class Interpreter : Visitor<Token::Literal> {
      public:

        OVERRIDE_VISITOR_TYPE
        Interpreter() = default;
        void interpret(const Expr& expr) const;

      private:

        [[nodiscard]] auto evaluate(const Expr& expr) const -> ReturnType;

        static auto isTruthy(Token::Literal literal) -> bool;
        static auto isEqual(Token::Literal left, Token::Literal right) -> bool;
        [[nodiscard]] static auto validateAndGetInts(
            const Token::Literal& left, const Token::Literal& right,
            const Token::Token& op) -> std::pair<int, int>;

        static void assertBothNumber(const Token::Literal& left,
                                     const Token::Literal& right,
                                     const Token::Token&   op);

        Logger::Logger& logger_ = Logger::Logger::instance();
    };

}  // namespace Expr
