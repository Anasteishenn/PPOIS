#include "appliances/SmartScale.h"

#include <cmath>

#include "exceptions/CulinaryException.h"

SmartScale::SmartScale(const std::string& model, int powerWatts, double maxWeight)
    : KitchenAppliance(model, powerWatts), _maxWeight(maxWeight), _tare(0.0), _lastWeight(0.0) {
    if (maxWeight <= 0) {
        throw ApplianceException("Предел весов должен быть положительным");
    }
}

std::string SmartScale::type() const {
    return "Весы";
}

void SmartScale::setTare(double grams) {
    if (grams < 0 || grams > _maxWeight) {
        throw ApplianceException("Некорректный вес тары");
    }
    _tare = grams;
}

double SmartScale::weigh(double rawGrams) {
    if (!isOn()) {
        throw ApplianceException("Весы выключены");
    }
    if (rawGrams > _maxWeight) {
        throw ApplianceException("Перегрузка весов");
    }
    _lastWeight = rawGrams - _tare;
    return _lastWeight;
}

bool SmartScale::checkPortion(const IngredientProportion& proportion, double rawGrams, double tolerancePercent) {
    double difference = std::fabs(weigh(rawGrams) - proportion.getAmount());
    return difference <= proportion.getAmount() * tolerancePercent / PERCENT;
}
