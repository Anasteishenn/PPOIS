#include "ConsoleUtils.h"

#include <iostream>
#include <stdexcept>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace console {

const char* InputClosed::what() const noexcept {
    return "Ввод закрыт";
}

void setupEncoding() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) {
        throw InputClosed();
    }
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    return line;
}

std::string readNonEmptyLine(const std::string& prompt) {
    std::string line = readLine(prompt);
    while (line.empty()) {
        std::cout << "Строка не должна быть пустой.\n";
        line = readLine(prompt);
    }
    return line;
}

int readInt(const std::string& prompt) {
    while (true) {
        std::string line = readLine(prompt);
        try {
            std::size_t parsedLength = 0;
            int value = std::stoi(line, &parsedLength);
            if (parsedLength == line.size()) {
                return value;
            }
        } catch (const std::logic_error&) {
            // Строка не является числом — запросим ввод ещё раз.
        }
        std::cout << "Введите целое число.\n";
    }
}

void printError(const std::exception& error) {
    std::cout << "Ошибка: " << error.what() << '\n';
}

}  // namespace console
