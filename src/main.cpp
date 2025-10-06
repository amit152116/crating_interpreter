#include "Thor/Interpreter.hpp"
#include "Thor/Lexer.hpp"
#include "Thor/Parser.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

constexpr std::string_view FILE_EXTENSION = ".krp";

namespace {

    void runPrompt() {
        std::string line;

        auto lexer       = Thor::Lexer();
        auto parser      = Parser::Parser();
        auto interpreter = Interpreter::Interpreter();
        while (true) {
            std::cout << ">>> ";
            if (!std::getline(std::cin, line)) {
                std::cout << "\n[EOF received. Exiting...]\n";
                std::cout.flush();
                break;  // Exit on Ctrl+D or EOF
            }
            if (line.empty()) {
                continue;  // Skip empty lines
            }
            if (line == "exit()" || line == "quit()") {
                std::cout << "[Exiting prompt...]\n";
                break;  // Exit the prompt
            }
            line.append("\n");
            auto tokens = lexer.tokenize(line);
            interpreter.interpret(parser.parse(tokens));
        }
    }

    auto runFile(std::string file) -> int {
        auto ext = file.substr(file.rfind('.'));
        if (ext != FILE_EXTENSION) {
            Logger::getLogger().error("Invalid file extension: `{}`", ext);
            Logger::getLogger().warn(
                "Only `{}` files are "
                "supported",
                FILE_EXTENSION);
            return 1;
        }
        if (not std::filesystem::exists(file)) {
            Logger::getLogger().error("File doesn't exists: {}", file);
            return 1;
        }
        std::string source;
        {
            std::ifstream readFile(file);
            if (readFile.is_open()) {
                std::stringstream ss;
                ss << readFile.rdbuf();
                source = ss.str();
            } else {
                Logger::getLogger().error("File found but failed to open: {}",
                                          file);
            }
        }
        auto lexer  = Thor::Lexer();
        auto tokens = lexer.tokenize(source);
        auto parser = Parser::Parser();

        auto interpreter = Interpreter::Interpreter();
        interpreter.interpret(parser.parse(tokens));
        return 0;
    }
}  // namespace

auto main(int argc, char const* argv[]) -> int {
    Logger::getLogger().setLogFile("krypton.log");
    Logger::getLogger().setLevel(Logger::LogLevel::DEBUG);
    if (argc > 2) {
        Logger::getLogger().error("Usage: krypton <filename>");
        return 1;
    }
    if (argc == 2) {
        runFile(argv[1]);
    } else {
        runPrompt();
    }

    Logger::getLogger().warn("Exiting Thor interpreter...");

    return 0;
}
