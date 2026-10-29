#pragma once

#include <vector>

#include "dishes/PriceTag.h"
#include "recipes/IngredientProportion.h"
#include "recipes/Recipe.h"

/// @brief Пересчёт порций: например, рецепт на 2 порции пересчитать на 6.
class PortionScaler {
public:
    PortionScaler(int baseServings, int targetServings);

    int getBaseServings() const { return _baseServings; }
    int getTargetServings() const { return _targetServings; }

    /// Коэффициент пересчёта: target / base.
    double factor() const;
    /// Ингредиенты рецепта, пересчитанные на новое число порций.
    std::vector<IngredientProportion> scale(const Recipe& recipe) const;
    /// Себестоимость рецепта на новое число порций.
    double scaleCost(const Recipe& recipe) const;
    /// Цена блюда на новое число порций.
    double scalePrice(const PriceTag& price) const;

private:
    int _baseServings;
    int _targetServings;
};
