#include "Lexer.hpp"
#include "Logger.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

const std::string FILE_EXTENSION = ".krp";

namespace {

    void runPrompt() {
        std::string line;

        while (true) {
            std::cout << ">> ";
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
            auto lexer = Krypton::Lexer(line + "\n");
        }
    }

    auto runFile(std::string file) -> int {
        auto ext = file.substr(file.rfind('.'));
        if (ext != FILE_EXTENSION) {
            Krypton::logger().error("Invalid file extension: `{}`", ext);
            Krypton::logger().info(
                "Only `{}` files are "
                "supported",
                FILE_EXTENSION);
            return 1;
        }
        if (not std::filesystem::exists(file)) {
            Krypton::logger().error("File doesn't exists: {}", file);
            return 1;
        }
        std::string source;
        {
            std::ifstream readFile(file);
            if (readFile.is_open()) {
                std::stringstream src_stream;
                src_stream << readFile.rdbuf();
                source = src_stream.str();
            } else {
                Krypton::logger().error("File found but failed to open: {}",
                                        file);
            }
        }
        auto lexer = Krypton::Lexer(source);
        return 0;
    }
}  // namespace

auto main(int argc, char const* argv[]) -> int {
    Krypton::logger().setLogFile("krypton.log");
    Krypton::logger().setLevel(Krypton::LogLevel::DEBUG);
    if (argc > 2) {
        Krypton::logger().error("Usage: krypton <filename>");
        return 1;
    }
    if (argc == 2) {
        runFile(argv[1]);
    } else {
        runPrompt();
    }

    return 0;
}
