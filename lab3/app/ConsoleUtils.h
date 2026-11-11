/**
 * @file ConsoleUtils.h
 * @brief Вспомогательные функции консольного ввода-вывода для меню.
 */
#pragma once

#include <exception>
#include <string>

namespace console {

/// Исключение: поток ввода закрыт (например, нажато Ctrl+Z / Ctrl+D).
class InputClosed : public std::exception {
public:
    const char* what() const noexcept override;
};

/// Включает вывод и ввод в кодировке UTF-8 (нужно для русских букв в консоли Windows).
void setupEncoding();

/// Выводит приглашение и читает строку. Бросает InputClosed, если ввод закончился.
std::string readLine(const std::string& prompt);

/// Читает строку, пока пользователь не введёт непустую.
std::string readNonEmptyLine(const std::string& prompt);

/// Читает целое число, пока пользователь не введёт корректное значение.
int readInt(const std::string& prompt);

/// Выводит сообщение об ошибке.
void printError(const std::exception& error);

}  // namespace console
