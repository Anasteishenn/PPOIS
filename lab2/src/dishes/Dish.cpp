#include "dishes/Dish.h"

#include "exceptions/CulinaryException.h"

Dish::Dish(const std::string& name, const Recipe& recipe, const PriceTag& price, const PlatingStyle& plating)
    : _name(name), _recipe(recipe), _price(price), _plating(plating), _isAvailable(true) {
    if (name.empty()) {
        throw DishNotFoundException("У блюда должно быть название");
    }
}

std::string Dish::serve() const {
    return "Подано: " + _name + ", " + _plating.describe();
}

std::vector<std::string> Dish::prepare() const {
    if (!_isAvailable) {
        throw DishNotAvailableException("Блюдо закончилось: " + _name);
    }
    std::vector<std::string> log = _recipe.cook();
    log.push_back(serve());
    return log;
}

double Dish::caloriesPerServing() const {
    return _recipe.totalNutrition().getCalories() / _recipe.getServings();
}

bool Dish::hasAllergen(const std::string& allergenName) const {
    return _recipe.containsAllergen(allergenName);
}

bool Dish::isVegetarian() const {
    return _recipe.getCategory().isVegetarian();
}

void Dish::markSoldOut() {
    _isAvailable = false;
}

void Dish::markAvailable() {
    _isAvailable = true;
}
