/**
 * @file Format.h
 * @brief Вспомогательная функция для вывода дробных чисел.
 */
#pragma once

#include <string>

namespace format {

/// Число с двумя знаками после запятой: 12.5 -> "12.50".
std::string number(double value);

}  // namespace format
