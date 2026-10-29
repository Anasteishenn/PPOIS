#include "dishes/PortionScaler.h"

#include "exceptions/CulinaryException.h"

PortionScaler::PortionScaler(int baseServings, int targetServings)
    : _baseServings(baseServings), _targetServings(targetServings) {
    if (baseServings <= 0 || targetServings <= 0) {
        throw RecipeException("Число порций должно быть положительным");
    }
}

double PortionScaler::factor() const {
    return static_cast<double>(_targetServings) / _baseServings;
}

std::vector<IngredientProportion> PortionScaler::scale(const Recipe& recipe) const {
    std::vector<IngredientProportion> result;
    for (const IngredientProportion& proportion : recipe.getProportions()) {
        result.push_back(proportion.scaled(factor()));
    }
    return result;
}

double PortionScaler::scaleCost(const Recipe& recipe) const {
    return recipe.totalCost() * factor();
}

double PortionScaler::scalePrice(const PriceTag& price) const {
    return price.finalPrice() * factor();
}
