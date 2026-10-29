#pragma once

#include <string>

#include "recipes/RecipeTimer.h"

/// @brief Абстрактный процесс приготовления (жарка, нарезка, маринование). У каждого процесса свой таймер.
class CookingProcess {
public:
    CookingProcess(const std::string& name, int durationMinutes);
    virtual ~CookingProcess() = default;

    const std::string& getName() const { return _name; }
    int getDuration() const { return _durationMinutes; }
    const RecipeTimer& getTimer() const { return _timer; }

    /// Описание того, что делает процесс. Каждый наследник описывает себя сам.
    virtual std::string execute() const = 0;
    /// Нужен ли нагрев (плита/духовка). По умолчанию — нет.
    virtual bool requiresHeat() const;
    /// Выполнить процесс: запустить таймер, дождаться окончания и вернуть описание.
    std::string run();

private:
    std::string _name;
    int _durationMinutes;
    RecipeTimer _timer;
};
