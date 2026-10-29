#include "recipes/ThermalProcess.h"

#include "exceptions/CulinaryException.h"

ThermalProcess::ThermalProcess(const std::string& method, int durationMinutes, int temperature)
    : CookingProcess(method, durationMinutes), _temperature(temperature), _method(method) {
    if (temperature < MIN_TEMPERATURE || temperature > MAX_TEMPERATURE) {
        throw InvalidTemperatureException("Температура процесса должна быть от 40 до 300 градусов");
    }
}

std::string ThermalProcess::execute() const {
    return _method + " при " + std::to_string(_temperature) + "°C, " + std::to_string(getDuration()) + " мин";
}

bool ThermalProcess::requiresHeat() const {
    return true;
}

bool ThermalProcess::isHighHeat() const {
    return _temperature >= HIGH_HEAT;
}
