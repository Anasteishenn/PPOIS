#pragma once

#include <string>
#include <vector>

#include "ingredients/Allergen.h"
#include "ingredients/NutritionalValue.h"

/// @brief Абстрактный ингредиент: название, цена за единицу, пищевая ценность на 100 единиц, аллергены.
class Ingredient {
public:
    /// Пищевая ценность задаётся на это количество единиц (100 г или 100 мл).
    static constexpr double NUTRITION_BASE_AMOUNT = 100.0;

    Ingredient(const std::string& name, double pricePerUnit, const NutritionalValue& nutrition);
    virtual ~Ingredient() = default;

    const std::string& getName() const { return _name; }
    double getPricePerUnit() const { return _pricePerUnit; }
    const std::vector<Allergen>& getAllergens() const { return _allergens; }

    /// Единица измерения: «г», «мл» или «л». Каждый наследник задаёт свою.
    virtual std::string unit() const = 0;
    /// Краткое описание для вывода, например «Мука (г)».
    virtual std::string describe() const;

    /// Добавляет аллерген (повторно тот же не добавляется).
    void addAllergen(const Allergen& allergen);
    /// Есть ли в ингредиенте аллерген с таким названием.
    bool hasAllergen(const std::string& allergenName) const;
    /// Стоимость заданного количества.
    double costFor(double amount) const;
    /// Пищевая ценность заданного количества.
    NutritionalValue nutritionFor(double amount) const;

private:
    std::string _name;
    double _pricePerUnit;
    NutritionalValue _nutrition;
    std::vector<Allergen> _allergens;
};
