#include <iostream>
#include <cpp-terminal/input.hpp>
#include <cpp-terminal/prompt.hpp>
#include "common/Lexer.h"
#include "common/Environment.h"
#include "common/Parser.h"
#include "interpreter/Evaluator.h"

using Term::Key;
using Term::prompt_multiline;
using Term::Terminal;

bool determine_completeness([[maybe_unused]] std::string command) {
    // Determine if the statement is complete
    bool complete;
    if (command.size() > 1 && command.substr(command.size() - 2, 1) == "\\") {
        complete = false;
    } else {
        complete = true;
    }
    return complete;
}

int main() {
    try {
        if (!Term::is_stdin_a_tty()) {
            std::cout << "The terminal is not attached to a TTY and therefore "
                         "can't catch user input. Exiting...\n";
            return 1;
        }
        Common::TokenTypeMapping tokenTypeMap;

        Terminal term(false, true, false, false);
        std::cout << "Interactive prompt." << std::endl;
        std::cout << "  * Use Ctrl-D to exit." << std::endl;
        std::cout << "  * Use Enter to submit." << std::endl;
        std::cout << "  * Features:" << std::endl;
        std::cout << "    - Editing (Keys: Left, Right, Home, End, Backspace)"
                  << std::endl;
        std::cout << "    - History (Keys: Up, Down)" << std::endl;
        std::cout
                << "    - Multi-line editing (use Alt-Enter to add a new line)"
                << std::endl;
        std::vector<std::string> history;
        std::function<bool(std::string)> iscomplete = determine_completeness;
        auto env = std::make_shared<Common::Environment>();
        while (true) {
            std::string answer =
                    Term::prompt_multiline(term, "> ", history, iscomplete);
            if (answer.size() == 1 && answer[0] == Key::CTRL + 'd')
                break;
            Common::Lexer lexer{answer};
            Common::Parser parser{&lexer};
            auto program = parser.parseProgram();
            // std::cout << "Program: " << program->toString() << std::endl;
            auto result = GI::eval(program.get(), env);
            if (result != nullptr) {
                std::cout << result->inspect() << std::endl;
            }
        }
    } catch (const std::runtime_error &re) {
        std::cerr << "Runtime error: " << re.what() << std::endl;
        return 2;
    } catch (...) {
        std::cerr << "Unknown error." << std::endl;
        return 1;
    }
    return 0;
}
