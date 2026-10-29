#include "appliances/ThermalAppliance.h"

#include "exceptions/CulinaryException.h"

ThermalAppliance::ThermalAppliance(const std::string& model, int powerWatts, int maxTemperature)
    : KitchenAppliance(model, powerWatts), _currentTemperature(ROOM_TEMPERATURE), _maxTemperature(maxTemperature) {
    if (maxTemperature <= ROOM_TEMPERATURE) {
        throw InvalidTemperatureException("Максимальная температура прибора должна быть выше комнатной");
    }
}

void ThermalAppliance::heatTo(int temperature) {
    if (!isOn()) {
        throw ApplianceException("Нельзя нагреть выключенный прибор: " + getModel());
    }
    if (temperature > _maxTemperature || temperature < ROOM_TEMPERATURE) {
        throw InvalidTemperatureException("Прибор не может работать при " + std::to_string(temperature) + "°C");
    }
    _currentTemperature = temperature;
}

void ThermalAppliance::coolDown() {
    _currentTemperature = ROOM_TEMPERATURE;
}

bool ThermalAppliance::canPerform(const ThermalProcess& process) const {
    return process.getTemperature() <= _maxTemperature;
}
