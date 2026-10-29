#include "appliances/KitchenAppliance.h"

#include "exceptions/CulinaryException.h"

KitchenAppliance::KitchenAppliance(const std::string& model, int powerWatts)
    : _model(model), _powerWatts(powerWatts), _isOn(false), _usageMinutes(0) {
    if (model.empty() || powerWatts <= 0) {
        throw ApplianceException("У прибора нужна модель и положительная мощность");
    }
}

void KitchenAppliance::turnOn() {
    _isOn = true;
}

void KitchenAppliance::turnOff() {
    _isOn = false;
}

void KitchenAppliance::use(int minutes) {
    if (!_isOn) {
        throw ApplianceException("Прибор выключен: " + _model);
    }
    if (minutes <= 0) {
        throw ApplianceException("Время работы должно быть положительным");
    }
    _usageMinutes += minutes;
}

double KitchenAppliance::energyUsedKwh() const {
    return _powerWatts / WATTS_PER_KILOWATT * (_usageMinutes / MINUTES_PER_HOUR);
}

bool KitchenAppliance::needsMaintenance() const {
    return _usageMinutes >= MAINTENANCE_MINUTES;
}

void KitchenAppliance::service() {
    _usageMinutes = 0;
}
