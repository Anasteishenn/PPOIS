#include "ingredients/PerishableIngredient.h"

#include "exceptions/CulinaryException.h"

PerishableIngredient::PerishableIngredient(const std::string& name, double pricePerGram,
                                           const NutritionalValue& nutrition, int expiryDay, int storageTemperature)
    : Ingredient(name, pricePerGram, nutrition),
      _expiryDay(expiryDay),
      _storageTemperature(storageTemperature),
      _isFrozen(false) {}

std::string PerishableIngredient::unit() const {
    return "г";
}

bool PerishableIngredient::isExpired(int today) const {
    return today > _expiryDay;
}

int PerishableIngredient::daysLeft(int today) const {
    return isExpired(today) ? 0 : _expiryDay - today;
}

void PerishableIngredient::checkFresh(int today) const {
    if (isExpired(today)) {
        throw ExpiredIngredientException("Продукт просрочен: " + getName());
    }
}

void PerishableIngredient::freeze() {
    if (!_isFrozen) {
        _isFrozen = true;
        _expiryDay += FREEZE_EXTRA_DAYS;
    }
}
