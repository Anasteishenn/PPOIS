#include "ingredients/GaseousIngredient.h"

#include "exceptions/CulinaryException.h"

GaseousIngredient::GaseousIngredient(const std::string& name, double pricePerLiter, double pressure,
                                     bool isFlammable)
    : Ingredient(name, pricePerLiter, NutritionalValue()), _pressure(pressure), _isFlammable(isFlammable) {
    if (pressure <= 0) {
        throw InvalidIngredientException("Давление газа должно быть положительным");
    }
}

std::string GaseousIngredient::unit() const {
    return "л";
}

bool GaseousIngredient::isDangerous() const {
    return _isFlammable || _pressure > DANGER_PRESSURE;
}

bool GaseousIngredient::canBeStoredUnder(double maxPressure) const {
    return _pressure <= maxPressure;
}
