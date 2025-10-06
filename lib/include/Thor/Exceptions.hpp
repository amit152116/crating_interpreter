#pragma once

#include "Tokens.hpp"

#include <utility>

namespace Error {
    class RuntimeException : public std::runtime_error {
      public:

        RuntimeException(Token::Token token, std::string message)
            : std::runtime_error(
                  fmt::format("[line {}, column {}] Error at '{}': {}",
                              token.line, token.start, token.lexeme, message)),
              token_(std::move(token)) {}

      private:

        Token::Token token_;
    };

    class ParseException : public std::runtime_error {
      public:

        explicit ParseException(std::string& error)
            : std::runtime_error(error) {}
    };
}  // namespace Error
