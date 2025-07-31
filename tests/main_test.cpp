

#include "Expressions.hpp"
#include "expression.hpp"

namespace {
    void testDemo() {
        using namespace Parser2;

        // Tokens
        auto plus  = Token::Token(Token::Type::PLUS, "+", nullptr, 0, 0, 0);
        auto minus = Token::Token(Token::Type::MINUS, "-", nullptr, 0, 0, 0);
        auto star  = Token::Token(Token::Type::STAR, "*", nullptr, 0, 0, 0);
        auto slash = Token::Token(Token::Type::SLASH, "/", nullptr, 0, 0, 0);
        auto bang  = Token::Token(Token::Type::BANG, "!", nullptr, 0, 0, 0);

        // Complex expression:
        // ((-123 + 45.67) * (!true)) / ("some string" + 100)

        auto expr = makeExpr<Binary>(
            makeExpr<Binary>(
                makeExpr<Binary>(
                    makeExpr<Unary>(minus, makeExpr<Literal>("123")), plus,
                    makeExpr<Literal>("45.67")),
                star, makeExpr<Unary>(bang, makeExpr<Literal>(824))),
            slash,
            makeExpr<Binary>(makeExpr<Literal>("some string"), plus,
                             makeExpr<Literal>(100)));

        AstPrinter astPrinter;
        RPNPrinter rpnPrinter;
        // astPrinter.print(std::move(expr));
        rpnPrinter.print(std::move(expr));
    }

    void testExpressions() {
        using namespace Parser;

        // Tokens
        auto plus  = Token::Token(Token::Type::PLUS, "+", nullptr, 0, 0, 0);
        auto minus = Token::Token(Token::Type::MINUS, "-", nullptr, 0, 0, 0);
        auto star  = Token::Token(Token::Type::STAR, "*", nullptr, 0, 0, 0);
        auto slash = Token::Token(Token::Type::SLASH, "/", nullptr, 0, 0, 0);
        auto bang  = Token::Token(Token::Type::BANG, "!", nullptr, 0, 0, 0);

        // Complex expression:
        // ((-123 + 45.67) * (!true)) / ("some string" + 100)

        auto expr = makeExpr<Binary>(
            makeExpr<Binary>(
                makeExpr<Binary>(
                    makeExpr<Unary>(minus, makeExpr<Literal>("123")), plus,
                    makeExpr<Literal>("45.67")),
                star, makeExpr<Unary>(bang, makeExpr<Literal>(824))),
            slash,
            makeExpr<Binary>(makeExpr<Literal>("some string"), plus,
                             makeExpr<Literal>(100)));

        AstPrinter printer;
        printer.print(expr);
    }
}  // namespace

auto main(int argc, char* argv[]) -> int {
    testDemo();
    testExpressions();
}
