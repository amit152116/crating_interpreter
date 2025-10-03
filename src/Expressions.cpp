#include "Expressions.hpp"

#include <utility>

namespace Expression {

    auto AstPrinter::visit(const Binary& expr) -> void {
        parenthesize(expr._operator.lexeme, expr._left, expr._right);
    }

    auto AstPrinter::visit(const Grouping& expr) -> void {
        parenthesize("Grouping", expr._expr);
    }

    auto AstPrinter::visit(const Literal& expr) -> void {
        parenthesize(expr._literal.stringify());
    }

    auto AstPrinter::visit(const Unary& expr) -> void {
        parenthesize(expr._operator.lexeme, expr._right);
    }
}  // namespace Expression
