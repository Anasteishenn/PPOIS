#pragma once

#include <memory>
#include <string>

#include "ingredients/Ingredient.h"

/// @brief Упаковки ингредиента от поставщика, например «3 пачки муки по 1000 г».
class IngredientPackage {
public:
    IngredientPackage(std::shared_ptr<Ingredient> ingredient, double amountPerPackage, int packageCount,
                      const std::string& packageType);

    const Ingredient& getIngredient() const { return *_ingredient; }
    int getPackageCount() const { return _packageCount; }

    /// Общее количество продукта во всех упаковках.
    double totalAmount() const;
    /// Общая стоимость всех упаковок.
    double totalPrice() const;
    /// Забрать несколько упаковок (их становится меньше).
    void takePackages(int count);
    /// Подпись, например «3 x пачка по 1000.00 г: Мука».
    std::string label() const;

private:
    std::shared_ptr<Ingredient> _ingredient;
    double _amountPerPackage;
    int _packageCount;
    std::string _packageType;
};
