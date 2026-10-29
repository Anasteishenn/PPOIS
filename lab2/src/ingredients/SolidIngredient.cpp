#include "ingredients/SolidIngredient.h"

#include <cmath>

#include "exceptions/CulinaryException.h"

SolidIngredient::SolidIngredient(const std::string& name, double pricePerGram, const NutritionalValue& nutrition,
                                 double pieceWeight)
    : Ingredient(name, pricePerGram, nutrition), _pieceWeight(pieceWeight), _isChopped(false) {
    if (pieceWeight <= 0) {
        throw InvalidIngredientException("Масса одной штуки должна быть положительной");
    }
}

std::string SolidIngredient::unit() const {
    return "г";
}

std::string SolidIngredient::describe() const {
    std::string text = Ingredient::describe();
    return _isChopped ? text + ", нарезан" : text;
}

void SolidIngredient::chop() {
    _isChopped = true;
}

int SolidIngredient::piecesFor(double grams) const {
    if (grams < 0) {
        throw InvalidIngredientException("Масса не может быть отрицательной");
    }
    return static_cast<int>(std::ceil(grams / _pieceWeight));
}
