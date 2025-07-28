#pragma once

#include "Logger.hpp"
#include "Tokens.hpp"

#include <cctype>
#include <string>
#include <vector>

namespace Krypton {

    class Lexer {
      public:

        explicit Lexer(std::string source);

        ~Lexer();

        void tokenize();

      private:

        // Helper methods for scanning
        auto               scanToken() -> Token;
        auto               scanChar(char ch) -> Token;
        void               addToken(Token token);
        [[nodiscard]] auto makeToken(TokenType type,
                                     Literal literal = nullptr) const -> Token;
        template <typename... Args>
        auto errorToken(fmt::format_string<Args...> fmt, Args&&... args) const
            -> Token;

        // Scanning methods
        auto               advance(int steps = 1) -> char;
        void               nextLine();
        auto               matchNext(char expected) -> bool;
        [[nodiscard]] auto peek() const -> char;
        [[nodiscard]] auto peekNext() const -> char;
        [[nodiscard]] auto isAtEnd() const -> bool;

        // Specific token scanners
        auto               getStringLiteral() -> Token;
        auto               getNumberLiteral() -> Token;
        auto               getIdentifier() -> Token;
        [[nodiscard]] auto checkKeyword(const std::string& identifier) const
            -> TokenType;

        // ispunct(c)	Returns true if c is a punctuation character (e.g.,
        // !@#$%^&*)

        // isspace(c)	Returns true if c is a whitespace character (' ', \t,
        // \n, etc.)

        // iscntrl(c)	Returns true if c is a control character (e.g., \n, \t,
        // ASCII < 32)

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

        std::vector<Token> _tokens;
        const std::string  _source;
        uint               _start;      // Start of the current lexeme
        uint               _current;    // Current position in the source
        uint               _line;       // Current line number
        uint               _column;     // Total Lenght till the previous line
        uint               _lineStart;  // Start of the current line

        Logger& _logger;
    };
}  // namespace Krypton
