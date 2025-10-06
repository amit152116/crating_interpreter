#pragma once

#include "Logger.hpp"
#include "Tokens.hpp"

#include <cctype>
#include <string>
#include <vector>

namespace Thor {

    class Lexer {
      public:

        explicit Lexer();
        ~Lexer();

        // Disable copy constructor and copy assignment
        Lexer(const Lexer&)                    = delete;
        auto operator=(const Lexer&) -> Lexer& = delete;

        // Disable move constructor and move assignment
        Lexer(Lexer&&)                    = delete;
        auto operator=(Lexer&&) -> Lexer& = delete;

        auto tokenize(std::string& source) -> std::vector<Token::Token>;

      private:

        // Helper methods for scanning
        auto               scanToken() -> Token::Token;
        auto               scanChar(char ch) -> Token::Token;
        void               addToken(Token::Token token);
        [[nodiscard]] auto makeToken(
            Token::Type                type,
            Token::Literal::LiteralVal literal = nullptr) const -> Token::Token;
        template <typename... Args>
        auto errorToken(fmt::format_string<Args...> fmt, Args&&... args) const
            -> Token::Token;

        // Scanning methods
        auto               advance(int steps = 1) -> char;
        void               nextLine();
        auto               matchNext(char expected) -> bool;
        [[nodiscard]] auto peek() const -> char;
        [[nodiscard]] auto peekNext() const -> char;
        [[nodiscard]] auto isAtEnd() const -> bool;

        // Specific token scanners
        auto                      getStringLiteral() -> Token::Token;
        auto                      getNumberLiteral() -> Token::Token;
        auto                      getIdentifier() -> Token::Token;
        [[nodiscard]] static auto checkKeyword(const std::string& identifier)
            -> Token::Type;

        // Utility methods
        static auto isDigit(char ch) -> bool {
            return isdigit(ch) != 0;
        }

        static auto isAlpha(char ch) -> bool {
            return (isalpha(ch) != 0) || ch == '_';
        }

        static auto isAlphaNumeric(char ch) -> bool {
            return isDigit(ch) || isAlpha(ch);
        }

        [[nodiscard]] static auto lineInfo(uint start, uint end)
            -> std::string {
            return fmt::format("[line {}:{}]", start, end);
        }

        [[nodiscard]] auto lineInfo() const -> std::string {
            return fmt::format("[line {}:{}]", line_, column_);
        }

        std::vector<Token::Token> tokens_;
        std::string               source_;
        uint                      start_;    // Start of the current lexeme
        uint                      current_;  // Current position in the source
        uint                      line_;     // Current line number
        uint column_;     // Total Lenght till the previous line
        uint lineStart_;  // Start of the current line

        Logger::Logger& logger_;
    };
}  // namespace Thor
