#include "ingredients/Ingredient.h"

#include "exceptions/CulinaryException.h"

Ingredient::Ingredient(const std::string& name, double pricePerUnit, const NutritionalValue& nutrition)
    : _name(name), _pricePerUnit(pricePerUnit), _nutrition(nutrition) {
    if (name.empty()) {
        throw InvalidIngredientException("Название ингредиента не может быть пустым");
    }
    if (pricePerUnit < 0) {
        throw InvalidIngredientException("Цена ингредиента не может быть отрицательной");
    }
}

std::string Ingredient::describe() const {
    return _name + " (" + unit() + ")";
}

void Ingredient::addAllergen(const Allergen& allergen) {
    if (!hasAllergen(allergen.getName())) {
        _allergens.push_back(allergen);
    }
}

bool Ingredient::hasAllergen(const std::string& allergenName) const {
    for (const Allergen& allergen : _allergens) {
        if (allergen.matches(allergenName)) {
            return true;
        }
    }
    return false;
}

double Ingredient::costFor(double amount) const {
    if (amount < 0) {
        throw InvalidIngredientException("Количество ингредиента не может быть отрицательным");
    }
    return amount * _pricePerUnit;
}

NutritionalValue Ingredient::nutritionFor(double amount) const {
    return _nutrition.scale(amount / NUTRITION_BASE_AMOUNT);
}
