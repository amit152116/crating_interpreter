#pragma once

#include "AstPrinter.hpp"
#include "Exceptions.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"
#include "TokenType.hpp"
#include "Tokens.hpp"

#include <functional>
#include <utility>
#include <vector>

namespace Parser {

    class Parser {
      public:

        Parser() = default;

        explicit Parser(std::vector<Token::Token> tokens)
            : tokens_(std::move(tokens)) {}

        auto parse(std::vector<Token::Token>& tokens)
            -> std::vector<Stmt::Stmt>;

      private:

        using PrefixFn = std::function<Expr::Expr()>;
        using InfixFn  = std::function<Expr::Expr(Expr::Expr)>;

        struct ParseRule {
            uint8_t  precdence;
            PrefixFn prefix;  // parseUnary, parsePrimary, etc.
            InfixFn
                 infix;  // parseBinaryLeft/Right, parsePostfix, parseTernary…
            bool rightAssoc;

            ParseRule(uint8_t precdence, PrefixFn prefix, InfixFn infix,
                      bool rightAssoc)
                : precdence(precdence),
                  prefix(std::move(prefix)),
                  infix(std::move(infix)),
                  rightAssoc(rightAssoc) {}

            ParseRule(uint8_t precdence, PrefixFn prefix, InfixFn infix)
                : precdence(precdence),
                  prefix(std::move(prefix)),
                  infix(std::move(infix)),
                  rightAssoc(false) {}
        };

        auto getRule(Token::Type type) -> ParseRule;
        auto parseBinary(Expr::Expr left) -> Expr::Expr;
        auto parseUnary() -> Expr::Expr;
        auto parseVariable() -> Expr::Expr;
        auto parsePrimary() -> Expr::Expr;
        auto parseTernary(Expr::Expr condition) -> Expr::Expr;
        auto parsePostfix(Expr::Expr left) -> Expr::Expr;

        auto parsePrecedence(uint8_t minPrec = 0) -> Expr::Expr;

        // Parsing Expressions
        auto expression() -> Expr::Expr;
        auto ternaryOperator() -> Expr::Expr;
        auto logicalOr() -> Expr::Expr;
        auto logicalAnd() -> Expr::Expr;
        auto bitXor() -> Expr::Expr;
        auto bitOr() -> Expr::Expr;
        auto bitAnd() -> Expr::Expr;
        auto equality() -> Expr::Expr;
        auto comparison() -> Expr::Expr;
        auto bitShift() -> Expr::Expr;
        auto term() -> Expr::Expr;
        auto factor() -> Expr::Expr;
        auto exponent() -> Expr::Expr;
        auto prefix() -> Expr::Expr;
        auto postfix() -> Expr::Expr;
        auto primary() -> Expr::Expr;
        auto group() -> Expr::Expr;
        auto parseInfix(std::function<Expr::Expr()>        next,
                        std::initializer_list<Token::Type> ops,
                        bool rightAssoc = false) -> Expr::Expr;

        // Parsing Statements
        auto declartion() -> Stmt::Stmt;
        auto varDeclartion(Token::Token type) -> Stmt::Stmt;
        auto statement() -> Stmt::Stmt;
        auto printStatement() -> Stmt::Stmt;
        auto expressionStatement() -> Stmt::Stmt;

        auto consume(Token::Type type, std::string error) -> Token::Token;
        static auto error(Token::Token token, std::string error)
            -> Error::ParseException;
        auto reportEror(std::string err) -> void;
        auto synchronize() -> void;

        [[nodiscard]] static auto lineInfo(Token::Token token) -> std::string {
            return fmt::format("[line {}:{}]", token.line, token.end);
        }

        // Utility methods
        [[nodiscard]] auto match(std::initializer_list<Token::Type> ops)
            -> bool;

        auto               advance() -> Token::Token;
        [[nodiscard]] auto checkType(Token::Type type) const -> bool;
        [[nodiscard]] auto previous() const -> Token::Token;
        [[nodiscard]] auto peek() const -> Token::Token;
        [[nodiscard]] auto isAtEnd() const -> bool;

        uint                      current_ = 0;
        std::vector<Token::Token> tokens_;

        AstPrinter::AstPrinter astPrinter_;

    };  // namespace Parser
}  // namespace Parser
