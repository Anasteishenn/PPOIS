#include "ingredients/LiquidIngredient.h"

#include "exceptions/CulinaryException.h"

LiquidIngredient::LiquidIngredient(const std::string& name, double pricePerMl, const NutritionalValue& nutrition,
                                   double density, int boilingPoint)
    : Ingredient(name, pricePerMl, nutrition), _density(density), _boilingPoint(boilingPoint) {
    if (density <= 0) {
        throw InvalidIngredientException("Плотность жидкости должна быть положительной");
    }
}

std::string LiquidIngredient::unit() const {
    return "мл";
}

double LiquidIngredient::toGrams(double milliliters) const {
    return milliliters * _density;
}

bool LiquidIngredient::boilsAt(int temperature) const {
    return temperature >= _boilingPoint;
}
