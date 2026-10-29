#pragma once

#include "recipes/CookingProcess.h"

/// @brief Механическая обработка: нарезка, взбивание, перемешивание инструментом.
class MechanicalProcess : public CookingProcess {
public:
    /// Начиная с этой скорости (об/мин) обработка считается быстрой.
    static constexpr int FAST_SPEED = 1000;

    MechanicalProcess(const std::string& name, int durationMinutes, const std::string& tool, int speed);

    const std::string& getTool() const { return _tool; }
    int getSpeed() const { return _speed; }

    /// Например: «Взбивание: венчик, 1200 об/мин, 5 мин».
    std::string execute() const override;
    /// Быстрая ли обработка (скорость >= FAST_SPEED).
    bool isFast() const;

private:
    std::string _tool;
    int _speed;
};
