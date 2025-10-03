#include "Interpreter.hpp"

#include "Exceptions.hpp"

namespace Interpreter {

    void Interpreter::interpret(
        const std::vector<Stmt::Stmt>& statments) const {
        try {
            for (const auto& stmt : statments) {
                execute(stmt);
            }
        } catch (Error::RuntimeException& e) {
            logger_.error(e.what());
        }
    }

    void Interpreter::execute(const Stmt::Stmt& stmt) const {
        stmt->accept(*this);
    }

    auto Interpreter::evaluate(const Expr::Expr& expr) const -> Token::Literal {
        if (expr == nullptr) {
            logger_.error("Interpreter : Expr type is null");
            return {};
        }
        return expr->accept(*this);
    }

    auto Interpreter::isTruthy(Token::Literal literal) -> bool {
        if (literal.isNil()) {
            return false;
        }
        if (literal.isBool()) {
            return literal.asBool();
        }
        if (literal.isNumber()) {
            return literal.asNumber() != 0.0;
        }
        return !literal.asString().empty();
    }

    auto Interpreter::isEqual(Token::Literal left, Token::Literal right)
        -> bool {
        if (left.isNil() && right.isNil()) {
            return true;
        }
        if (left.isNil() || right.isNil()) {
            return false;
        }
        return left.value == right.value;
    }

    auto Interpreter::visit(const Expr::InfixExpr& expr) const
        -> Token::Literal {
        auto left  = evaluate(expr.left);
        auto right = evaluate(expr.right);

        switch (expr.operator_.type) {
            case Token::Type::LOGICAL_OR: {
                return Token::Literal{isTruthy(left) || isTruthy(right)};
            }

            case Token::Type::LOGICAL_AND: {
                return Token::Literal{isTruthy(left) && isTruthy(right)};
            }

            case Token::Type::BIT_OR: {
                if (!left.isNumber() || !right.isNumber()) {
                    throw Error::RuntimeException(
                        expr.operator_, "operator can't work on this type");
                }
                int l = left.toInt();
                int r = right.toInt();

                if (l < 0 || r < 0 || std::floor(l) != l ||
                    std::floor(r) != r) {
                    throw Error::RuntimeException(
                        expr.operator_,
                        "Operands must be non-negative integers");
                }
                return Token::Literal{static_cast<double>(l | r)};
            }

            case Token::Type::BIT_XOR: {
                if (!left.isNumber() || !right.isNumber()) {
                    throw Error::RuntimeException(
                        expr.operator_, "operator can't work on this type");
                }
                int l = left.toInt();
                int r = right.toInt();

                if (l < 0 || r < 0 || std::floor(l) != l ||
                    std::floor(r) != r) {
                    throw Error::RuntimeException(
                        expr.operator_,
                        "Operands must be non-negative integers");
                }
                return Token::Literal{static_cast<double>(l ^ r)};
            }

            case Token::Type::BIT_AND: {
                if (!left.isNumber() || !right.isNumber()) {
                    throw Error::RuntimeException(
                        expr.operator_, "operator can't work on this type");
                }
                int l = left.toInt();
                int r = right.toInt();

                if (l < 0 || r < 0 || std::floor(l) != l ||
                    std::floor(r) != r) {
                    throw Error::RuntimeException(
                        expr.operator_,
                        "Operands must be non-negative integers");
                }
                return Token::Literal(static_cast<double>(l & r));
            }

            case Token::Type::EQUAL_EQUAL:
                return Token::Literal{isEqual(left, right)};

            case Token::Type::BANG_EQUAL:
                return Token::Literal{!isEqual(left, right)};

            case Token::Type::GREATER: {
                if (left.isNumber() && right.isNumber()) {
                    return Token::Literal(left.asNumber() > right.asNumber());
                }

                if (left.isString() && right.isString()) {
                    return Token::Literal(left.asString() > right.asString());
                }
                throw Error::RuntimeException(
                    expr.operator_, "operator can't work on this type");
            }

            case Token::Type::GREATER_EQUAL: {
                if (left.isNumber() && right.isNumber()) {
                    return Token::Literal(left.asNumber() >= right.asNumber());
                }

                if (left.isString() && right.isString()) {
                    return Token::Literal(left.asString() >= right.asString());
                }
                throw Error::RuntimeException(
                    expr.operator_, "operator can't work on this type");
            }
            case Token::Type::LESS: {
                if (left.isNumber() && right.isNumber()) {
                    return Token::Literal(left.asNumber() < right.asNumber());
                }

                if (left.isString() && right.isString()) {
                    return Token::Literal(left.asString() < right.asString());
                }
                throw Error::RuntimeException(
                    expr.operator_, "operator can't work on this type");
            }
            case Token::Type::LESS_EQUAL: {
                if (left.isNumber() && right.isNumber()) {
                    return Token::Literal(left.asNumber() <= right.asNumber());
                }

                if (left.isString() && right.isString()) {
                    return Token::Literal(left.asString() <= right.asString());
                }
                throw Error::RuntimeException(
                    expr.operator_, "operator can't work on this type");
            }
            case Token::Type::LEFT_SHIFT: {
                if (!left.isNumber() || !right.isNumber()) {
                    throw Error::RuntimeException(
                        expr.operator_, "operator can't work on this type");
                }
                int l = left.toInt();
                int r = right.toInt();

                if (l < 0 || r < 0 || std::floor(l) != l ||
                    std::floor(r) != r) {
                    throw Error::RuntimeException(
                        expr.operator_,
                        "Operands must be non-negative integers");
                }
                return Token::Literal(static_cast<double>(l << r));
            }
            case Token::Type::RIGHT_SHIFT: {
                if (!left.isNumber() || !right.isNumber()) {
                    throw Error::RuntimeException(
                        expr.operator_, "operator can't work on this type");
                }
                int l = left.toInt();
                int r = right.toInt();

                if (l < 0 || r < 0 || std::floor(l) != l ||
                    std::floor(r) != r) {
                    throw Error::RuntimeException(
                        expr.operator_,
                        "Operands must be non-negative integers");
                }
                return Token::Literal(static_cast<double>(l >> r));
            }
            case Token::Type::MINUS:

                if (!left.isNumber() || !right.isNumber()) {
                    throw Error::RuntimeException(
                        expr.operator_, "operator can't work on this type");
                }
                return Token::Literal(left.asNumber() - right.asNumber());
            case Token::Type::PLUS:

                if (left.isNumber() && right.isNumber()) {
                    return Token::Literal(left.asNumber() + right.asNumber());
                }

                if (left.isString() && right.isString()) {
                    return Token::Literal(left.asString() + right.asString());
                }

                if (left.isString() || right.isString()) {
                    return Token::Literal(left.stringify() + right.stringify());
                }
                throw Error::RuntimeException(
                    expr.operator_, "operator can't work on these types");
            case Token::Type::SLASH:
                if (!left.isNumber() || !right.isNumber()) {
                    throw Error::RuntimeException(
                        expr.operator_, "operator can't work on this type");
                }
                return Token::Literal(left.asNumber() / right.asNumber());
            case Token::Type::STAR:

                if (!left.isNumber() || !right.isNumber()) {
                    throw Error::RuntimeException(
                        expr.operator_, "operator can't work on this type");
                }
                return Token::Literal(left.asNumber() * right.asNumber());
            case Token::Type::PERCENT:
                if (!left.isNumber() || !right.isNumber()) {
                    throw Error::RuntimeException(
                        expr.operator_, "operator can't work on this type");
                }
                return Token::Literal(static_cast<int>(left.asNumber()) %
                                      static_cast<int>(right.asNumber()));

            case Token::Type::STAR_STAR:
                if (!left.isNumber() || !right.isNumber()) {
                    throw Error::RuntimeException(
                        expr.operator_, "operator can't work on this type");
                }
                return Token::Literal{pow(left.asNumber(), right.asNumber())};
            default:
                return {};
        }
        return {};
    }

    auto Interpreter::visit(const Expr::PrefixExpr& expr) const
        -> Token::Literal {
        auto value = evaluate(expr.right);
        switch (expr.operator_.type) {
            case Token::Type::MINUS:

                if (value.isNumber()) {
                    value.setValue(-value.asNumber());
                } else {
                    throw Error::RuntimeException(
                        expr.operator_, "operator can't work on this type");
                }
                break;
            case Token::Type::PLUS:
                if (!value.isNumber()) {
                    throw Error::RuntimeException(
                        expr.operator_, "operator can't work on this type");
                }
                break;
            case Token::Type::PLUS_PLUS:
                if (value.isNumber()) {
                    value.setValue(value.asNumber() + 1);
                } else {
                    throw Error::RuntimeException(
                        expr.operator_, "operator can't work on this type");
                }
            case Token::Type::MINUS_MINUS:
                if (value.isNumber()) {
                    value.setValue(value.asNumber() - 1);
                } else {
                    throw Error::RuntimeException(
                        expr.operator_, "operator can't work on this type");
                }
            case Token::Type::BANG:
                return Token::Literal(!isTruthy(value));
            default:
                throw Error::RuntimeException(
                    expr.operator_, "Interpreter: operator is not valid");
        }
        return value;
    }

    auto Interpreter::visit(const Expr::PostfixExpr& expr) const
        -> Token::Literal {
        auto value = evaluate(expr.left);
        if (!value.isNumber()) {
            throw Error::RuntimeException(expr.operator_,
                                          "operator can't work on this type");
        }

        switch (expr.operator_.type) {
            case Token::Type::PLUS_PLUS:
                value.setValue(value.asNumber() + 1);
            case Token::Type::MINUS_MINUS:
                value.setValue(value.asNumber() - 1);
            default:
                throw Error::RuntimeException(
                    expr.operator_, "Interpreter: operator is not valid");
        }
        return value;
    }

    auto Interpreter::visit(const Expr::TernaryExpr& expr) const
        -> Token::Literal {
        auto condition = evaluate(expr.condition);
        if (isTruthy(condition)) {
            return evaluate(expr.trueExpr);
        }
        return evaluate(expr.falseExpr);
    }

    auto Interpreter::visit(const Expr::GroupExpr& expr) const
        -> Token::Literal {
        return evaluate(expr.expr);
    }

    auto Interpreter::visit(const Expr::LiteralExpr& expr) const
        -> Token::Literal {
        return expr.literal;
    }

    auto Interpreter::visit(const Expr::Variable& expr) const
        -> Token::Literal {
        return expr.literal;
    }

    auto Interpreter::visit(const Stmt::Expression& stmt) const -> void {
        auto value = evaluate(stmt.expression);
        logger_.debug("Expression result: ", value.stringify());
    }

    auto Interpreter::visit(const Stmt::Variable& stmt) const -> void {
        auto value = evaluate(stmt.initializer);
        logger_.debug("Variable Declartion:  {},{}: {}", stmt.name, stmt.type,
                      value.stringify());
    }

    auto Interpreter::visit(const Stmt::Print& stmt) const -> void {
        auto value = evaluate(stmt.expression);
        fmt::print("{}\n", value.stringify());
    }

    auto Interpreter::validateAndGetInts(const Token::Literal& left,
                                         const Token::Literal& right,
                                         const Token::Token&   op)
        -> std::pair<int, int> {
        if (!left.isNumber() || !right.isNumber()) {
            throw Error::RuntimeException(op, "Operands must be numbers.");
        }
        int l = left.toInt();
        int r = right.toInt();

        if (l < 0 || r < 0 || std::floor(left.asNumber()) != l ||
            std::floor(right.asNumber()) != r) {
            throw Error::RuntimeException(
                op, "Operands must be non-negative integers.");
        }
        return {l, r};
    }

    void Interpreter::assertBothNumber(const Token::Literal& left,
                                       const Token::Literal& right,
                                       const Token::Token&   op) {
        if (!left.isNumber() || !right.isNumber()) {
            throw Error::RuntimeException(op, "Operands must be numbers.");
        }
    }
}  // namespace Interpreter
