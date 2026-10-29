#include "recipes/IngredientProportion.h"

#include <utility>

#include "Format.h"
#include "exceptions/CulinaryException.h"

IngredientProportion::IngredientProportion(std::shared_ptr<Ingredient> ingredient, double amount)
    : _ingredient(std::move(ingredient)), _amount(amount) {
    if (!_ingredient || amount <= 0) {
        throw InvalidIngredientException("В рецепте нужен ингредиент и положительное количество");
    }
}

IngredientProportion IngredientProportion::scaled(double factor) const {
    return IngredientProportion(_ingredient, _amount * factor);
}

double IngredientProportion::cost() const {
    return _ingredient->costFor(_amount);
}

NutritionalValue IngredientProportion::nutrition() const {
    return _ingredient->nutritionFor(_amount);
}

std::string IngredientProportion::describe() const {
    return _ingredient->getName() + ": " + format::number(_amount) + " " + _ingredient->unit();
}
