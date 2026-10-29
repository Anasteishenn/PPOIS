#include "appliances/Oven.h"

#include "exceptions/CulinaryException.h"

Oven::Oven(const std::string& model, int powerWatts, int trays, bool hasConvection)
    : ThermalAppliance(model, powerWatts, MAX_TEMPERATURE), _trays(trays), _hasConvection(hasConvection) {
    if (trays <= 0) {
        throw ApplianceException("В духовке должен быть хотя бы один противень");
    }
}

std::string Oven::type() const {
    return "Духовка";
}

int Oven::preheatMinutes(int targetTemperature) const {
    int degrees = targetTemperature - getCurrentTemperature();
    if (degrees <= 0) {
        return 0;
    }
    int speed = _hasConvection ? DEGREES_PER_MINUTE * CONVECTION_FACTOR : DEGREES_PER_MINUTE;
    return (degrees + speed - 1) / speed;
}

std::string Oven::bake(const Recipe& recipe, int traysUsed) {
    if (traysUsed <= 0 || traysUsed > _trays) {
        throw ApplianceException("В духовке только " + std::to_string(_trays) + " противня(ей)");
    }
    if (getCurrentTemperature() == ROOM_TEMPERATURE) {
        throw ApplianceException("Духовка не разогрета");
    }
    use(recipe.totalTime());
    return "Запекается " + recipe.getTitle() + " при " + std::to_string(getCurrentTemperature()) + "°C, " +
           std::to_string(recipe.totalTime()) + " мин";
}
