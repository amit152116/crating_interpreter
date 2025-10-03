#include "Lexer.hpp"

#include <bit>
#include <utility>

namespace Krypton {

    Lexer::Lexer()
        : _start(0),
          _current(0),
          _line(1),
          _column(0),
          _lineStart(0),
          _logger(Logger::getLogger()) {}

    Lexer::~Lexer() {
        _tokens.clear();
    }

    auto Lexer::tokenize(std::string& source) -> std::vector<Token::Token> {
        // _logger.warn("Source Size : {}", source.size());
        _source    = source;
        _start     = 0;
        _current   = 0;
        _line      = 1;
        _column    = 0;
        _lineStart = 0;

        // Estimate: One token per ~4 characters is a common heuristic
        auto estCapacity = _source.size() / 4;
        estCapacity      = 1U << (std::__bit_width(estCapacity) - 1);

        if (_tokens.capacity() < estCapacity) {
            _tokens.reserve(estCapacity);
        }
        _tokens.clear();
        while (!isAtEnd()) {
            auto token = scanToken();
            addToken(token);
        }
        return _tokens;
    }

    auto Lexer::advance(int step) -> char {
        auto ch = peek();
        _current += step;
        return ch;
    }

    auto Lexer::isAtEnd() const -> bool {
        return _source.length() <= _current;
    }

    void Lexer::nextLine() {
        _line++;
        _column    = 0;
        _lineStart = _current - 1;
    }

    auto Lexer::matchNext(char expected) -> bool {
        if (isAtEnd() || _source.at(_current) != expected) {
            return false;
        }
        advance();
        return true;
    }

    void Lexer::addToken(Token::Token token) {
        _tokens.emplace_back(token);
        _logger.debug("Size: {}, Capacity: {}", _tokens.size(),
                      _tokens.capacity());
    }

    auto Lexer::makeToken(Token::Type                type,
                          Token::Literal::LiteralVal literal) const
        -> Token::Token {
        auto length = _current - _start;
        auto text   = _source.substr(_start, length);

        auto token = Token::Token{
            type, text, literal, _start - _lineStart, _current - _lineStart,
            _line};
        _logger.debug("Created: {}", token);
        return token;
    }

    template <typename... Args>
    auto Lexer::errorToken(fmt::format_string<Args...> fmt,
                           Args&&... args) const -> Token::Token {
        auto message = fmt::format(fmt, std::forward<Args>(args)...);
        _logger.error(message);
        return makeToken(Token::Type::ERROR, message);
    }

    auto Lexer::peek() const -> char {
        if (isAtEnd()) {
            return '\0';
        }
        return _source.at(_current);
    }

    auto Lexer::peekNext() const -> char {
        if (_source.length() <= 1 + _current) {
            return '\0';
        }
        return _source.at(_current + 1);
    }

    auto Lexer::getStringLiteral() -> Token::Token {
        std::vector<uint> newLines;
        while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n') {
                nextLine();
                newLines.push_back(_current - _start);
            }
            advance();
        }
        if (isAtEnd()) {
            return errorToken("{} Error: Unterminated string", lineInfo());
        }
        // The closing ".
        advance();

        // Trim the surrounding quotes.
        auto value = _source.substr(_start, _current - _start);

        // Remove the newline characters from the string.
        for (auto it = newLines.rbegin(); it != newLines.rend(); ++it) {
            value.erase(*it, 1);
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
            std::stod(std::string(_source.substr(_start, _current)));
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
        auto identifier = _source.substr(_start, _current - _start);
        auto type       = checkKeyword(identifier);

        return makeToken(type);
    }

    auto Lexer::scanToken() -> Token::Token {
        // Skip any whitespace or comments before starting a token
        while (true) {
            _start = _current;
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
                if (matchNext('/')) {
                    return makeToken(Token::Type::SLASH_SLASH);
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

}  // namespace Krypton
