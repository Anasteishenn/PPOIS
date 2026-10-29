#include "appliances/StoveTop.h"

#include "exceptions/CulinaryException.h"

StoveTop::StoveTop(const std::string& model, int powerWatts, int burners, bool isGas)
    : ThermalAppliance(model, powerWatts, MAX_TEMPERATURE), _burners(burners), _busyBurners(0), _isGas(isGas) {
    if (burners <= 0) {
        throw ApplianceException("У плиты должна быть хотя бы одна конфорка");
    }
}

std::string StoveTop::type() const {
    return "Плита";
}

void StoveTop::occupyBurner() {
    if (!isOn()) {
        throw ApplianceException("Плита выключена");
    }
    if (freeBurners() == 0) {
        throw ApplianceException("Все конфорки заняты");
    }
    ++_busyBurners;
}

void StoveTop::releaseBurner() {
    if (_busyBurners == 0) {
        throw ApplianceException("Нет занятых конфорок");
    }
    --_busyBurners;
}

int StoveTop::freeBurners() const {
    return _burners - _busyBurners;
}
