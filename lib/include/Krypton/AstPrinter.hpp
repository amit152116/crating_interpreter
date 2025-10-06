#pragma once

#include "Expr.hpp"
#include "Logger.hpp"

#include <sstream>
#include <string>

namespace AstPrinter {
    class AstPrinter : Expr::Visitor<std::string> {
      public:

        AstPrinter() = default;

        auto print(Expr::Expr expr) const -> void;

      private:

        [[nodiscard]] auto visit(const Expr::Variable& expr) const
            -> std::string final;
        [[nodiscard]] auto visit(const Expr::InfixExpr& expr) const
            -> std::string final;
        [[nodiscard]] auto visit(const Expr::GroupExpr& expr) const
            -> std::string final;
        [[nodiscard]] auto visit(const Expr::LiteralExpr& expr) const
            -> std::string final;
        [[nodiscard]] auto visit(const Expr::PrefixExpr& expr) const
            -> std::string final;
        [[nodiscard]] auto visit(const Expr::PostfixExpr& expr) const
            -> std::string final;
        [[nodiscard]] auto visit(const Expr::TernaryExpr& expr) const
            -> std::string final;

        template <typename... ExprPtrs>
        auto parenthesize(std::string name, ExprPtrs&&... exprs) const
            -> std::string {
            constexpr bool AllValid =
                ((std::is_same_v<std::decay_t<ExprPtrs>, Expr::Expr> ||
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
}  // namespace AstPrinter
