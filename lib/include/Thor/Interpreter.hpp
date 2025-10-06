#pragma once

#include "Expr.hpp"
#include "Logger.hpp"
#include "Stmt.hpp"
#include "Tokens.hpp"

#include <utility>
#include <vector>

namespace Interpreter {
    class Interpreter : Expr::Visitor<Token::Literal>, Stmt::Visitor<void> {
      public:

        Interpreter() = default;

        void interpret(const std::vector<Stmt::Stmt>& statments) const;

      private:

        [[nodiscard]] auto visit(const Expr::Variable& expr) const
            -> Token::Literal final;
        [[nodiscard]] auto visit(const Expr::InfixExpr& expr) const
            -> Token::Literal final;
        [[nodiscard]] auto visit(const Expr::GroupExpr& expr) const
            -> Token::Literal final;
        [[nodiscard]] auto visit(const Expr::LiteralExpr& expr) const
            -> Token::Literal final;
        [[nodiscard]] auto visit(const Expr::PrefixExpr& expr) const
            -> Token::Literal final;
        [[nodiscard]] auto visit(const Expr::PostfixExpr& expr) const
            -> Token::Literal final;
        [[nodiscard]] auto visit(const Expr::TernaryExpr& expr) const
            -> Token::Literal final;

        auto visit(const Stmt::Expression& stmt) const -> void final;
        auto visit(const Stmt::Variable& stmt) const -> void final;
        auto visit(const Stmt::Print& stmt) const -> void final;

        void execute(const Stmt::Stmt& stmt) const;

        [[nodiscard]] auto evaluate(const Expr::Expr& expr) const
            -> Token::Literal;

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
}  // namespace Interpreter
