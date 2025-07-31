#include "expression.hpp"

namespace Parser2 {

    auto AstPrinter::visit(const Binary& expr) const -> ReturnType {
        return parenthesize(expr._operator.lexeme, expr._left, expr._right);
    }

    auto AstPrinter::visit(const Grouping& expr) const -> ReturnType {
        return parenthesize("group", expr._expr);
    }

    auto AstPrinter::visit(const Unary& expr) const -> ReturnType {
        return parenthesize(expr._operator.lexeme, expr._right);
    }

    auto AstPrinter::visit(const Literal& expr) const -> ReturnType {
        return parenthesize(expr._literal.stringify());
    }

    auto RPNPrinter::visit(const Binary& expr) const -> ReturnType {
        return parenthesize(expr._operator.lexeme, expr._left, expr._right);
    }

    auto RPNPrinter::visit(const Grouping& expr) const -> ReturnType {
        return parenthesize("group", expr._expr);
    }

    auto RPNPrinter::visit(const Unary& expr) const -> ReturnType {
        return parenthesize(expr._operator.lexeme, expr._right);
    }

    auto RPNPrinter::visit(const Literal& expr) const -> ReturnType {
        return parenthesize(expr._literal.stringify());
    }

}  // namespace Parser2
