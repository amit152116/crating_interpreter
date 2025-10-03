
#include "Parser.hpp"

#include "TokenType.hpp"

#include <utility>

namespace Parser {

    auto Parser::parse(std::vector<Token::Token>& tokens) -> Expr::Expr {
        _tokens  = tokens;
        _current = 0;
        try {
            return expression();
        } catch (Error::ParseException& e) {
            Logger::getLogger().error(e.what());
            return nullptr;
        }
    }

    auto Parser::expression() -> Expr::Expr {
        return assignment();
    }

    auto Parser::parsePrecedence(uint8_t minPrec) -> Expr::Expr {
        // 1) Consume a prefix
        auto token = advance();
        auto rule  = getRule(token.type);
        if (!rule.prefix) {
            error(token, "Expected expression.");
        }
        Expr::Expr left = (rule.prefix)();

        // 2) While the next token is an infix/postfix of >= minPrec
        while ((peek().type != Token::Type::EOF_ &&
                getRule(peek().type).precdence > minPrec) ||
               (getRule(peek().type).precdence == minPrec &&
                getRule(peek().type).rightAssoc)) {
            Token::Token op      = advance();
            auto         infRule = getRule(op.type);
            left                 = (infRule.infix)(std::move(left));
        }

        return left;
    }

    auto Parser::parseUnary() -> Expr::Expr {
        Token::Token op = previous();  // We already consumed the operator token

        // Use UNARY precedence so it binds tightly to the right
        Expr::Expr right = parsePrecedence(10);

        return Expr::makeExpr<Expr::PrefixExpr>(op, std::move(right));
    }

    auto Parser::parsePostfix(Expr::Expr left) -> Expr::Expr {
        Token::Token op = previous();  // the postfix token (++ or --)

        return Expr::makeExpr<Expr::PostfixExpr>(std::move(left), op);
    }

    auto Parser::parseBinary(Expr::Expr left) -> Expr::Expr {
        Token::Token op   = previous();
        const auto&  rule = getRule(op.type);
        uint8_t      rightPrec =
            rule.rightAssoc ? rule.precdence : rule.precdence + 1;

        Expr::Expr right = parsePrecedence(rightPrec);

        return Expr::makeExpr<Expr::InfixExpr>(std::move(left), op,
                                               std::move(right));
    }

    auto Parser::parseTernary(Expr::Expr condition) -> Expr::Expr {
        // '?' has already been consumed
        Expr::Expr thenBranch = parsePrecedence(1);

        // Require and consume ':'
        consume(Token::Type::COLON,
                "Expect ':' after then-branch of ternary operator.");

        Expr::Expr elseBranch = parsePrecedence(1);

        return Expr::makeExpr<Expr::TernaryExpr>(
            std::move(condition), std::move(thenBranch), std::move(elseBranch));
    }

    auto Parser::parsePrimary() -> Expr::Expr {
        switch (previous().type) {
            case Token::Type::NUMBER:
            case Token::Type::STRING:
                return Expr::makeExpr<Expr::LiteralExpr>(
                    previous().literal);  // or string value

            case Token::Type::TRUE:
                return Expr::makeExpr<Expr::LiteralExpr>(true);

            case Token::Type::FALSE:
                return Expr::makeExpr<Expr::LiteralExpr>(false);

            case Token::Type::NIL:
                return Expr::makeExpr<Expr::LiteralExpr>(nullptr);

            case Token::Type::IDENTIFIER:
                return parseVariable();

            case Token::Type::LEFT_PAREN: {
                Expr::Expr expr = parsePrecedence(0);  // or lowest

                consume(Token::Type::RIGHT_PAREN,
                        "Expect ')' after expression.");
                return Expr::makeExpr<Expr::GroupExpr>(std::move(expr));
            }

            default:
                throw error(peek(), "Expect expression.");
        }
    }

    auto Parser::parseInfixLeft(std::function<Expr::Expr()>        next,
                                std::initializer_list<Token::Type> ops)
        -> Expr::Expr {
        auto expr = next();

        while (match(ops)) {
            auto operator_ = previous();
            auto right     = next();
            expr           = Expr::makeExpr<Expr::InfixExpr>(
                std::move(expr), std::move(operator_), std::move(right));
        }
        return expr;
    }

    auto Parser::parseInfixRight(std::function<Expr::Expr()>        next,
                                 std::initializer_list<Token::Type> ops)
        -> Expr::Expr {
        auto expr = next();

        while (match(ops)) {
            auto operator_ = previous();
            auto right     = parseInfixRight(next, ops);
            expr           = Expr::makeExpr<Expr::InfixExpr>(
                std::move(expr), std::move(operator_), std::move(right));
        }
        return expr;
    }

    auto Parser::assignment() -> Expr::Expr {
        return parseInfixLeft(
            [this] { return ternaryOperator(); },
            {Token::Type::EQUAL, Token::Type::PLUS_EQUAL,
             Token::Type::MINUS_EQUAL, Token::Type::SLASH_EQUAL,
             Token::Type::STAR_EQUAL});
    }

    auto Parser::ternaryOperator() -> Expr::Expr {
        auto condition = logicalOr();

        while (match({Token::Type::QUESTION})) {
            auto trueExpr = logicalOr();
            consume(Token::Type::COLON,
                    "Expected ':' after true branch of ternary.");
            auto falseExpr = assignment();  // recursive to support nested
            condition = Expr::makeExpr<Expr::TernaryExpr>(std::move(condition),
                                                          std::move(trueExpr),
                                                          std::move(falseExpr));
        }
        return condition;
    }

    auto Parser::logicalOr() -> Expr::Expr {
        return parseInfixLeft([this] { return logicalAnd(); },
                              {Token::Type::LOGICAL_OR});
    }

    auto Parser::logicalAnd() -> Expr::Expr {
        return parseInfixLeft([this] { return bitOr(); },
                              {Token::Type::LOGICAL_AND});
    }

    auto Parser::bitOr() -> Expr::Expr {
        return parseInfixLeft([this] { return bitXor(); },
                              {Token::Type::BIT_OR});
    }

    auto Parser::bitXor() -> Expr::Expr {
        return parseInfixLeft([this] { return bitAnd(); },
                              {Token::Type::BIT_XOR});
    }

    auto Parser::bitAnd() -> Expr::Expr {
        return parseInfixLeft([this] { return equality(); },
                              {Token::Type::BIT_AND});
    }

    auto Parser::equality() -> Expr::Expr {
        return parseInfixLeft(
            [this] { return comparison(); },
            {Token::Type::EQUAL_EQUAL, Token::Type::BANG_EQUAL});
    }

    auto Parser::comparison() -> Expr::Expr {
        return parseInfixLeft([this] { return bitShift(); },
                              {Token::Type::GREATER, Token::Type::GREATER_EQUAL,
                               Token::Type::LESS_EQUAL, Token::Type::LESS});
    }

    auto Parser::bitShift() -> Expr::Expr {
        return parseInfixLeft(
            [this] { return term(); },
            {Token::Type::LEFT_SHIFT, Token::Type::RIGHT_SHIFT});
    }

    auto Parser::term() -> Expr::Expr {
        return parseInfixLeft([this] { return factor(); },
                              {Token::Type::MINUS, Token::Type::PLUS});
    }

    auto Parser::factor() -> Expr::Expr {
        return parseInfixLeft([this] { return exponent(); },
                              {Token::Type::SLASH, Token::Type::STAR,
                               Token::Type::PERCENT, Token::Type::SLASH_SLASH});
    }

    auto Parser::exponent() -> Expr::Expr {
        return parseInfixRight([this] { return prefix(); },
                               {Token::Type::STAR_STAR});
    }

    auto Parser::prefix() -> Expr::Expr {
        if (match({Token::Type::BANG, Token::Type::MINUS, Token::Type::PLUS,
                   Token::Type::PLUS_PLUS, Token::Type::MINUS_MINUS})) {
            auto operator_ = previous();
            auto right     = prefix();
            return Expr::makeExpr<Expr::PrefixExpr>(std::move(operator_),
                                                    std::move(right));
        }
        return postfix();
    }

    auto Parser::postfix() -> Expr::Expr {
        auto expr = primary();

        if (match({Token::Type::PLUS_PLUS, Token::Type::MINUS_MINUS})) {
            auto operator_ = previous();
            expr           = Expr::makeExpr<Expr::PostfixExpr>(std::move(expr),
                                                               std::move(operator_));
        }

        return expr;
    }

    auto Parser::primary() -> Expr::Expr {
        if (match({Token::Type::TRUE})) {
            return Expr::makeExpr<Expr::LiteralExpr>(true);
        }
        if (match({Token::Type::FALSE})) {
            return Expr::makeExpr<Expr::LiteralExpr>(false);
        }
        if (match({Token::Type::NIL})) {
            return Expr::makeExpr<Expr::LiteralExpr>(nullptr);
        }
        if (match({Token::Type::NUMBER, Token::Type::STRING})) {
            return Expr::makeExpr<Expr::LiteralExpr>(
                std::move(previous().literal));
        }
        if (match({Token::Type::IDENTIFIER})) {
            return parseVariable();
        }
        return group();
    }

    auto Parser::group() -> Expr::Expr {
        if (match({Token::Type::LEFT_PAREN})) {
            auto expr = expression();
            consume(Token::Type::RIGHT_PAREN, "Expect ')' after expression.");
            return Expr::makeExpr<Expr::GroupExpr>(std::move(expr));
        }
        throw error(peek(), "Expect expression.");
    }

    auto Parser::parseVariable() -> Expr::Expr {
        return Expr::makeExpr<Expr::VariableExpr>(previous());
    }

    auto Parser::getRule(Token::Type type) -> ParseRule {
        switch (type) {
            case Token::Type::NUMBER:
            case Token::Type::IDENTIFIER:
            case Token::Type::STRING:
            case Token::Type::TRUE:
            case Token::Type::FALSE:
            case Token::Type::NIL:
            case Token::Type::LEFT_PAREN:
                return {1, [this] { return parsePrimary(); }, nullptr};
            case Token::Type::PLUS_PLUS:
            case Token::Type::MINUS_MINUS:
                return {
                    2, [this] { return parseUnary(); },
                    [this](auto&& left) {
                        return parsePostfix(std::forward<decltype(left)>(left));
                    }};
            case Token::Type::BANG:
                return {2, [this] { return parseUnary(); }, nullptr};
            case Token::Type::STAR_STAR:
                return {
                    3, nullptr,
                    [this](auto&& left) {
                        return parseBinary(std::forward<decltype(left)>(left));
                    },
                    true};

                break;
            case Token::Type::STAR:
            case Token::Type::SLASH:
            case Token::Type::PERCENT:
            case Token::Type::SLASH_SLASH:

                return {
                    4, nullptr, [this](auto&& left) {
                        return parseBinary(std::forward<decltype(left)>(left));
                    }};
            case Token::Type::PLUS:
            case Token::Type::MINUS:

                return {
                    5, nullptr, [this](auto&& left) {
                        return parseBinary(std::forward<decltype(left)>(left));
                    }};
            case Token::Type::LEFT_SHIFT:
            case Token::Type::RIGHT_SHIFT:

                return {
                    6, nullptr, [this](auto&& left) {
                        return parseBinary(std::forward<decltype(left)>(left));
                    }};
            case Token::Type::GREATER:
            case Token::Type::GREATER_EQUAL:
            case Token::Type::LESS:
            case Token::Type::LESS_EQUAL:

                return {
                    7, nullptr, [this](auto&& left) {
                        return parseBinary(std::forward<decltype(left)>(left));
                    }};
            case Token::Type::BANG_EQUAL:
            case Token::Type::EQUAL_EQUAL:

                return {
                    8, nullptr, [this](auto&& left) {
                        return parseBinary(std::forward<decltype(left)>(left));
                    }};
            case Token::Type::BIT_AND:

                return {
                    9, nullptr, [this](auto&& left) {
                        return parseBinary(std::forward<decltype(left)>(left));
                    }};
            case Token::Type::BIT_XOR:

                return {
                    10, nullptr, [this](auto&& left) {
                        return parseBinary(std::forward<decltype(left)>(left));
                    }};
            case Token::Type::BIT_OR:

                return {
                    11, nullptr, [this](auto&& left) {
                        return parseBinary(std::forward<decltype(left)>(left));
                    }};
            case Token::Type::LOGICAL_AND:

                return {
                    12, nullptr, [this](auto&& left) {
                        return parseBinary(std::forward<decltype(left)>(left));
                    }};
            case Token::Type::LOGICAL_OR:
                return {
                    13, nullptr, [this](auto&& left) {
                        return parseBinary(std::forward<decltype(left)>(left));
                    }};
            case Token::Type::EQUAL:
            case Token::Type::PLUS_EQUAL:
            case Token::Type::MINUS_EQUAL:
            case Token::Type::SLASH_EQUAL:
            case Token::Type::STAR_EQUAL:
                return {
                    14, nullptr,
                    [this](auto&& left) {
                        return parseBinary(std::forward<decltype(left)>(left));
                    },
                    true};
            case Token::Type::QUESTION:
                return {
                    14, nullptr,
                    [this](auto&& left) {
                        return parseTernary(std::forward<decltype(left)>(left));
                    },
                    true};
            default:
                throw error(peek(), "default error");
        }
    }

    auto Parser::synchronize() -> void {
        advance();
        while (!isAtEnd()) {
            if (previous().type == Token::Type::SEMICOLON) {
                return;
            }

            switch (peek().type) {
                case Token::Type::CLASS:
                case Token::Type::FUNCTION:
                case Token::Type::VAR:
                case Token::Type::VAL:
                case Token::Type::FOR:
                case Token::Type::IF:
                case Token::Type::WHILE:
                case Token::Type::PRINT:
                case Token::Type::RETURN:
                    return;
                default:
                    break;
            }
            advance();
        }
    }

    auto Parser::consume(Token::Type type, std::string errorMsg)
        -> Token::Token {
        if (checkType(type)) {
            return advance();
        }
        throw Parser::error(peek(), errorMsg);
    }

    auto Parser::error(Token::Token token, std::string error)
        -> Error::ParseException {
        std::string errorMsg;
        if (token.type == Token::Type::EOF_) {
            errorMsg.append(fmt::format("{} at end {}", token, error));

        } else {
            errorMsg.append(
                fmt::format("{} at `{}` {}", token, token.lexeme, error));
        }
        return Error::ParseException(errorMsg);
    }

    auto Parser::reportEror(std::string err) -> void {}

    auto Parser::advance() -> Token::Token {
        if (!isAtEnd()) {
            _current++;
        }
        return previous();
    }

    auto Parser::checkType(Token::Type type) const -> bool {
        if (isAtEnd()) {
            return false;
        }
        return peek().type == type;
    }

    auto Parser::previous() const -> Token::Token {
        return _tokens.at(_current - 1);
    }

    auto Parser::peek() const -> Token::Token {
        return _tokens.at(_current);
    }

    auto Parser::isAtEnd() const -> bool {
        return peek().type == Token::Type::EOF_;
    }

    auto Parser::match(std::initializer_list<Token::Type> ops) -> bool {
        if (std::any_of(ops.begin(), ops.end(),
                        [this](Token::Type type) { return checkType(type); })) {
            advance();
            return true;
        }
        return false;
    }

}  // namespace Parser
