#pragma once

#include "recipes/CookingProcess.h"

/// @brief Тепловая обработка: жарка, варка, запекание при заданной температуре.
class ThermalProcess : public CookingProcess {
public:
    static constexpr int MIN_TEMPERATURE = 40;
    static constexpr int MAX_TEMPERATURE = 300;
    /// С этой температуры нагрев считается сильным.
    static constexpr int HIGH_HEAT = 180;

    ThermalProcess(const std::string& method, int durationMinutes, int temperature);

    int getTemperature() const { return _temperature; }
    const std::string& getMethod() const { return _method; }

    /// Например: «Жарка при 200°C, 10 мин».
    std::string execute() const override;
    bool requiresHeat() const override;
    /// Сильный ли нагрев (температура >= HIGH_HEAT).
    bool isHighHeat() const;

private:
    int _temperature;
    std::string _method;
};
