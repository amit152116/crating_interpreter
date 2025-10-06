#include "Thor/Lexer.hpp"

#include <bit>
#include <utility>

namespace Thor {

    Lexer::Lexer()
        : start_(0),
          current_(0),
          line_(1),
          column_(0),
          lineStart_(0),
          logger_(Logger::getLogger()) {}

    Lexer::~Lexer() {
        tokens_.clear();
    }

    auto Lexer::tokenize(std::string& source) -> std::vector<Token::Token> {
        source_    = source;
        start_     = 0;
        current_   = 0;
        line_      = 1;
        column_    = 0;
        lineStart_ = 0;

        // Estimate: One token per ~4 characters is a common heuristic
        auto estCapacity = source_.size() / 4;
        estCapacity      = 1U << (std::__bit_width(estCapacity) - 1);

        if (tokens_.capacity() < estCapacity) {
            tokens_.reserve(estCapacity);
        }
        tokens_.clear();
        while (!isAtEnd()) {
            auto token = scanToken();
            addToken(token);
        }
        return tokens_;
    }

    auto Lexer::advance(int step) -> char {
        auto ch = peek();
        current_ += step;
        return ch;
    }

    auto Lexer::isAtEnd() const -> bool {
        return source_.length() <= current_;
    }

    void Lexer::nextLine() {
        line_++;
        column_    = 0;
        lineStart_ = current_ - 1;
    }

    auto Lexer::matchNext(char expected) -> bool {
        if (isAtEnd() || source_.at(current_) != expected) {
            return false;
        }
        advance();
        return true;
    }

    void Lexer::addToken(Token::Token token) {
        tokens_.emplace_back(token);
    }

    auto Lexer::makeToken(Token::Type                type,
                          Token::Literal::LiteralVal literal) const
        -> Token::Token {
        auto length = current_ - start_;
        auto text   = source_.substr(start_, length);

        auto token = Token::Token{
            type, text, literal, start_ - lineStart_, current_ - lineStart_,
            line_};
        logger_.debug("Created: {}", token);
        return token;
    }

    template <typename... Args>
    auto Lexer::errorToken(fmt::format_string<Args...> fmt,
                           Args&&... args) const -> Token::Token {
        auto message = fmt::format(fmt, std::forward<Args>(args)...);
        logger_.error(message);
        return makeToken(Token::Type::ERROR, message);
    }

    auto Lexer::peek() const -> char {
        if (isAtEnd()) {
            return '\0';
        }
        return source_.at(current_);
    }

    auto Lexer::peekNext() const -> char {
        if (source_.length() <= 1 + current_) {
            return '\0';
        }
        return source_.at(current_ + 1);
    }

    auto Lexer::getStringLiteral() -> Token::Token {
        std::vector<uint> newLines;
        while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n') {
                nextLine();
                newLines.push_back(current_ - start_);
            }
            advance();
        }
        if (isAtEnd()) {
            return errorToken("{} Error: Unterminated string", lineInfo());
        }
        // The closing ".
        advance();

        // Trim the surrounding quotes.
        auto value = source_.substr(start_ + 1, (current_ - start_) - 2);

        // Remove the newline characters from the string.
        for (auto it = newLines.rbegin(); it != newLines.rend(); ++it) {
            value.erase(*it - 1, 1);
        }
        return makeToken(Token::Type::STRING, value);
    }

    auto Lexer::getNumberLiteral() -> Token::Token {
        while (isDigit(peek())) {
            advance();
        }

        // Look for a fractional part.
        if (peek() == '.' && isDigit(peekNext())) {
            advance();

            while (isDigit(peek())) {
                advance();
            }
        }
        double number =
            std::stod(std::string(source_.substr(start_, current_)));
        return makeToken(Token::Type::NUMBER, number);
    }

    auto Lexer::checkKeyword(const std::string& identifier) -> Token::Type {
        auto keywords = Token::getKeywordMap();
        auto it       = keywords.find(identifier);

        if (it != keywords.end()) {
            return it->second;
        }

        return Token::Type::IDENTIFIER;
    }

    auto Lexer::getIdentifier() -> Token::Token {
        while (isAlphaNumeric(peek())) {
            advance();
        }

        // Check if the identifier is a keyword
        auto identifier = source_.substr(start_, current_ - start_);
        auto type       = checkKeyword(identifier);

        return makeToken(type);
    }

    auto Lexer::scanToken() -> Token::Token {
        // Skip any whitespace or comments before starting a token
        while (true) {
            start_ = current_;
            if (isAtEnd()) {
                return makeToken(Token::Type::EOF_);
            }

            char ch = advance();

            // Skip whitespace
            if (ch == '\n') {
                nextLine();
                continue;
            }
            if ((isspace(ch) != 0)) {
                continue;
            }

            // Handle comments
            if (ch == '/') {
                if (matchNext('/')) {
                    while (!matchNext('\n')) {
                        advance();
                    }
                    auto comment = makeToken(Token::Type::SINGLE_COMMENT);
                    nextLine();
                    continue;
                }
                if (matchNext('*')) {
                    while (true) {
                        if (isAtEnd()) {
                            return errorToken(
                                "{} Error: Unterminated multi-line comment ",
                                lineInfo());
                        }
                        if (peek() == '*' && peekNext() == '/') {
                            advance(2);  // Skip the closing */
                            break;
                        }
                        if (peek() == '\n') {
                            nextLine();
                        }
                        advance();
                    }
                    auto comment = makeToken(Token::Type::MULTI_COMMENT);
                    continue;
                }
            }

            // We have found a meaningful character, process it
            return scanChar(ch);
        }
    }

    auto Lexer::scanChar(char ch) -> Token::Token {
        switch (ch) {
            case '(':
                return makeToken(Token::Type::LEFT_PAREN);
            case ')':
                return makeToken(Token::Type::RIGHT_PAREN);
            case '{':
                return makeToken(Token::Type::LEFT_BRACE);
            case '}':
                return makeToken(Token::Type::RIGHT_BRACE);
            case '[':
                return makeToken(Token::Type::LEFT_BRACKET);
            case ']':
                return makeToken(Token::Type::RIGHT_BRACKET);
            case ',':
                return makeToken(Token::Type::COMMA);
            case '.':
                return makeToken(Token::Type::DOT);
            case ';':
                return makeToken(Token::Type::SEMICOLON);
            case '?':
                return makeToken(Token::Type::QUESTION);
            case ':':
                return makeToken(Token::Type::COLON);
            case '%':
                return makeToken(Token::Type::PERCENT);
            case '$':
                return makeToken(Token::Type::DOLLAR);
            case '^':
                return makeToken(Token::Type::BIT_XOR);
            case '~':
                return makeToken(Token::Type::TILDE);

            case '!':
                if (matchNext('=')) {
                    return makeToken(Token::Type::BANG_EQUAL);
                }
                return makeToken(Token::Type::BANG);
            case '=':
                if (matchNext('=')) {
                    return makeToken(Token::Type::EQUAL_EQUAL);
                }
                return makeToken(Token::Type::EQUAL);
            case '>':
                if (matchNext('=')) {
                    return makeToken(Token::Type::GREATER_EQUAL);
                }
                if (matchNext('>')) {
                    return makeToken(Token::Type::RIGHT_SHIFT);
                }
                return makeToken(Token::Type::GREATER);
            case '<':
                if (matchNext('=')) {
                    return makeToken(Token::Type::LESS_EQUAL);
                }
                if (matchNext('<')) {
                    return makeToken(Token::Type::LEFT_SHIFT);
                }
                return makeToken(Token::Type::LESS);
            case '&':
                if (matchNext('&')) {
                    return makeToken(Token::Type::LOGICAL_AND);
                }
                return makeToken(Token::Type::BIT_AND);
            case '|':
                if (matchNext('|')) {
                    return makeToken(Token::Type::LOGICAL_OR);
                }
                return makeToken(Token::Type::BIT_OR);
            case '+':
                if (matchNext('+')) {
                    return makeToken(Token::Type::PLUS_PLUS);
                }
                if (matchNext('=')) {
                    return makeToken(Token::Type::PLUS_EQUAL);
                }
                return makeToken(Token::Type::PLUS);
            case '-':
                if (matchNext('-')) {
                    return makeToken(Token::Type::MINUS_MINUS);
                }
                if (matchNext('=')) {
                    return makeToken(Token::Type::MINUS_EQUAL);
                }
                return makeToken(Token::Type::MINUS);
            case '*':
                if (matchNext('*')) {
                    return makeToken(Token::Type::STAR_STAR);
                }
                if (matchNext('=')) {
                    return makeToken(Token::Type::STAR_EQUAL);
                }
                return makeToken(Token::Type::STAR);
            case '/':
                if (matchNext('=')) {
                    return makeToken(Token::Type::SLASH_EQUAL);
                }
                return makeToken(Token::Type::SLASH);
            case '"':
                return getStringLiteral();
            case '\0':
                return makeToken(Token::Type::EOF_);
            default:
                if (isDigit(ch)) {
                    return getNumberLiteral();
                } else if (isAlpha(ch)) {
                    return getIdentifier();
                }
                return errorToken("{} Error: Unexpected character: '{}'",
                                  lineInfo(), ch);
        }
    }

}  // namespace Thor
