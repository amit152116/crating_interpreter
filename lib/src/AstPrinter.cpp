#include "Thor/AstPrinter.hpp"

namespace AstPrinter {

    auto AstPrinter::print(Expr::Expr expr) const -> void {
        if (expr == nullptr) {
            logger_.error("AstPrinter : Expr type is null");
            return;
        }
        auto result = expr->accept(*this);
        logger_.info("AST Expression: {}", result);
    }

    auto AstPrinter::visit(const Expr::Variable& expr) const -> std::string {
        return parenthesize(expr.name.toString());
    }

    auto AstPrinter::visit(const Expr::InfixExpr& expr) const -> std::string {
        return parenthesize("", expr.left, std::string(expr.operator_.lexeme),
                            expr.right);
    }

    auto AstPrinter::visit(const Expr::GroupExpr& expr) const -> std::string {
        return parenthesize(std::string("("), expr.expr, std::string(")"));
    }

    auto AstPrinter::visit(const Expr::PrefixExpr& expr) const -> std::string {
        return parenthesize(expr.operator_.lexeme, expr.right);
    }

    auto AstPrinter::visit(const Expr::PostfixExpr& expr) const -> std::string {
        return parenthesize("", expr.left, std::string(expr.operator_.lexeme));
    }

    auto AstPrinter::visit(const Expr::TernaryExpr& expr) const -> std::string {
        return parenthesize("If", expr.condition, std::string("then"),
                            expr.trueExpr, std::string("or"), expr.falseExpr);
    }

    auto AstPrinter::visit(const Expr::LiteralExpr& expr) const -> std::string {
        return parenthesize(expr.literal.stringify());
    }
}  // namespace AstPrinter
