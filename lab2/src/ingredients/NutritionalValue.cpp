#include "ingredients/NutritionalValue.h"

#include "exceptions/CulinaryException.h"

NutritionalValue::NutritionalValue(double calories, double proteins, double fats, double carbs)
    : _calories(calories), _proteins(proteins), _fats(fats), _carbs(carbs) {
    if (calories < 0 || proteins < 0 || fats < 0 || carbs < 0) {
        throw InvalidIngredientException("Пищевая ценность не может быть отрицательной");
    }
}

NutritionalValue NutritionalValue::scale(double factor) const {
    return NutritionalValue(_calories * factor, _proteins * factor, _fats * factor, _carbs * factor);
}

NutritionalValue NutritionalValue::add(const NutritionalValue& other) const {
    return NutritionalValue(_calories + other._calories, _proteins + other._proteins, _fats + other._fats,
                            _carbs + other._carbs);
}

bool NutritionalValue::isLowCalorie() const {
    return _calories < LOW_CALORIE_LIMIT;
}
