#include "Lexer.hpp"

#include <utility>

namespace Krypton {

    Lexer::Lexer(std::string source)
        : _source(std::move(source)),
          _start(0),
          _current(0),
          _line(1),
          _column(0),
          _lineStart(0),
          _logger(logger()) {
        tokenize();
    }

    Lexer::~Lexer() {
        _tokens.clear();
    }

    void Lexer::tokenize() {
        while (!isAtEnd()) {
            auto token = scanToken();
            addToken(token);
        }
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
        _logger.debug("Moved to next line: {} at column {}", _line, _column);
    }

    auto Lexer::matchNext(char expected) -> bool {
        if (isAtEnd() || _source.at(_current) != expected) {
            return false;
        }
        advance();
        return true;
    }

    void Lexer::addToken(Token token) {
        _tokens.emplace_back(token);
    }

    auto Lexer::makeToken(TokenType type, Literal literal) const -> Token {
        auto length = _current - _start;
        auto text   = _source.substr(_start, length);

        auto token = Token{
            type, text, literal, _start - _lineStart, _current - _lineStart,
            _line};
        _logger.info("Created: {}", token);
        return token;
    }

    template <typename... Args>
    auto Lexer::errorToken(fmt::format_string<Args...> fmt,
                           Args&&... args) const -> Token {
        auto message = fmt::format(fmt, std::forward<Args>(args)...);
        _logger.error(message);
        return makeToken(TokenType::ERROR, message);
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

    auto Lexer::getStringLiteral() -> Token {
        std::vector<uint> newLines;
        while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n') {
                nextLine();
                newLines.push_back(_current - _start);
            }
            advance();
        }
        if (isAtEnd()) {
            return errorToken("Unterminated string at {}:{}", _line, _column);
        }
        // The closing ".
        advance();

        // Trim the surrounding quotes.
        auto value = _source.substr(_start, _current - _start);

        // Remove the newline characters from the string.
        for (auto it = newLines.rbegin(); it != newLines.rend(); ++it) {
            value.erase(*it, 1);
        }
        return makeToken(TokenType::STRING, value);
    }

    auto Lexer::getNumberLiteral() -> Token {
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
        return makeToken(TokenType::NUMBER, number);
    }

    auto Lexer::checkKeyword(const std::string& identifier) const -> TokenType {
        auto keywords = getKeywordMap();
        auto it       = keywords.find(identifier);

        if (it != keywords.end()) {
            _logger.debug("Identifier '{}' matched with keyword '{}'",
                          identifier, it->second);
            return it->second;
        }
        _logger.debug("Identifier '{}' is not a keyword", identifier);

        return TokenType::IDENTIFIER;
    }

    auto Lexer::getIdentifier() -> Token {
        while (isAlphaNumeric(peek())) {
            advance();
        }

        // Check if the identifier is a keyword
        auto identifier = _source.substr(_start, _current - _start);
        auto type       = checkKeyword(identifier);

        return makeToken(type);
    }

    auto Lexer::scanToken() -> Token {
        // Skip any whitespace or comments before starting a token
        while (true) {
            _start = _current;
            if (isAtEnd()) {
                return makeToken(TokenType::EOF_);
            }

            char ch = advance();

            // Skip whitespace
            if (ch == ' ' || ch == '\r' || ch == '\t') {
                continue;
            }

            if (ch == '\n') {
                nextLine();
                continue;
            }

            // Handle comments
            if (ch == '/') {
                if (matchNext('/')) {
                    _logger.debug("Single-line comment detected at line {}",
                                  _line);
                    while (!matchNext('\n')) {
                        advance();
                    }
                    nextLine();
                    continue;
                }
                if (matchNext('*')) {
                    _logger.debug("Multi-line comment detected at line {}",
                                  _line);
                    while (true) {
                        if (isAtEnd()) {
                            return errorToken(
                                "Unterminated multi-line comment "
                                "at {}:{}",
                                _line, _column);
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
                    continue;
                }
            }

            // We have found a meaningful character, process it
            return scanChar(ch);
        }
    }

    auto Lexer::scanChar(char ch) -> Token {
        switch (ch) {
            case '(':
                return makeToken(TokenType::LEFT_PAREN);
            case ')':
                return makeToken(TokenType::RIGHT_PAREN);
            case '{':
                return makeToken(TokenType::LEFT_BRACE);
            case '}':
                return makeToken(TokenType::RIGHT_BRACE);
            case '[':
                return makeToken(TokenType::LEFT_BRACKET);
            case ']':
                return makeToken(TokenType::RIGHT_BRACKET);
            case ',':
                return makeToken(TokenType::COMMA);
            case '.':
                return makeToken(TokenType::DOT);
            case ';':
                return makeToken(TokenType::SEMICOLON);
            case '?':
                return makeToken(TokenType::QUESTION);
            case ':':
                return makeToken(TokenType::COLON);
            case '%':
                return makeToken(TokenType::PERCENT);
            case '$':
                return makeToken(TokenType::DOLLAR);
            case '^':
                return makeToken(TokenType::CARET);
            case '~':
                return makeToken(TokenType::TILDE);

            case '!':
                if (matchNext('=')) {
                    return makeToken(TokenType::BANG_EQUAL);
                }
                return makeToken(TokenType::BANG);
            case '=':
                if (matchNext('=')) {
                    return makeToken(TokenType::EQUAL_EQUAL);
                }
                return makeToken(TokenType::EQUAL);
            case '>':
                if (matchNext('=')) {
                    return makeToken(TokenType::GREATER_EQUAL);
                }
                if (matchNext('>')) {
                    return makeToken(TokenType::GREATER_GREATER);
                }
                return makeToken(TokenType::GREATER);
            case '<':
                if (matchNext('=')) {
                    return makeToken(TokenType::LESS_EQUAL);
                }
                if (matchNext('<')) {
                    return makeToken(TokenType::LESS_LESS);
                }
                return makeToken(TokenType::LESS);
            case '&':
                if (matchNext('&')) {
                    return makeToken(TokenType::AMPERSAND_AMPERSAND);
                }
                return makeToken(TokenType::AMPERSAND);
            case '|':
                if (matchNext('|')) {
                    return makeToken(TokenType::PIPE_PIPE);
                }
                return makeToken(TokenType::PIPE);
            case '+':
                if (matchNext('+')) {
                    return makeToken(TokenType::PLUS_PLUS);
                }
                if (matchNext('=')) {
                    return makeToken(TokenType::PLUS_EQUAL);
                }
                return makeToken(TokenType::PLUS);
            case '-':
                if (matchNext('-')) {
                    return makeToken(TokenType::MINUS_MINUS);
                }
                if (matchNext('=')) {
                    return makeToken(TokenType::MINUS_EQUAL);
                }
                return makeToken(TokenType::MINUS);
            case '*':
                if (matchNext('*')) {
                    return makeToken(TokenType::STAR_STAR);
                }
                if (matchNext('=')) {
                    return makeToken(TokenType::STAR_EQUAL);
                }
                return makeToken(TokenType::STAR);
            case '/':
                if (matchNext('=')) {
                    return makeToken(TokenType::SLASH_EQUAL);
                }
                return makeToken(TokenType::SLASH);
            case '"':
                return getStringLiteral();
            case '\0':
                return makeToken(TokenType::EOF_);
            default:
                if (isDigit(ch)) {
                    return getNumberLiteral();
                } else if (isAlpha(ch)) {
                    return getIdentifier();
                }
                return errorToken("Unexpected character: '{}' at {}:{}", ch,
                                  _line, _column);
        }
    }

}  // namespace Krypton
