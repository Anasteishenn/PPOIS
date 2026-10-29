#pragma once

#include <memory>
#include <string>

#include "ingredients/Ingredient.h"
#include "ingredients/NutritionalValue.h"

/// @brief Строка рецепта «сколько какого ингредиента»: например, «Мука: 200 г».
class IngredientProportion {
public:
    IngredientProportion(std::shared_ptr<Ingredient> ingredient, double amount);

    const Ingredient& getIngredient() const { return *_ingredient; }
    double getAmount() const { return _amount; }

    /// Та же строка, но количество умножено на коэффициент (для пересчёта порций).
    IngredientProportion scaled(double factor) const;
    /// Стоимость этого количества ингредиента.
    double cost() const;
    /// Пищевая ценность этого количества ингредиента.
    NutritionalValue nutrition() const;
    /// Текст, например «Мука: 200.00 г».
    std::string describe() const;

private:
    std::shared_ptr<Ingredient> _ingredient;
    double _amount;
};
