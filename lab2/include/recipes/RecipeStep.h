#pragma once

#include <memory>
#include <string>

#include "recipes/CookingProcess.h"

/// @brief Шаг рецепта: номер, текст инструкции и процесс, который выполняется на этом шаге.
class RecipeStep {
public:
    RecipeStep(int number, const std::string& instruction, std::shared_ptr<CookingProcess> process);

    int getNumber() const { return _number; }
    const std::string& getInstruction() const { return _instruction; }
    const CookingProcess& getProcess() const { return *_process; }

    /// Выполнить шаг и вернуть строку для журнала: «Шаг 1: ... — ...».
    std::string perform() const;
    /// Сколько минут занимает шаг.
    int duration() const;

private:
    int _number;
    std::string _instruction;
    std::shared_ptr<CookingProcess> _process;
};
