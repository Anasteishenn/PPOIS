#pragma once

#include <string>
#include <vector>

#include "dishes/PlatingStyle.h"
#include "dishes/PriceTag.h"
#include "recipes/Recipe.h"

/// @brief Абстрактное блюдо меню: название, рецепт, цена, подача и есть ли оно в наличии.
class Dish {
public:
    Dish(const std::string& name, const Recipe& recipe, const PriceTag& price, const PlatingStyle& plating);
    virtual ~Dish() = default;

    const std::string& getName() const { return _name; }
    const Recipe& getRecipe() const { return _recipe; }
    const PriceTag& getPrice() const { return _price; }
    PriceTag& getPrice() { return _price; }
    const PlatingStyle& getPlating() const { return _plating; }
    bool isAvailable() const { return _isAvailable; }

    /// Раздел меню («Закуска», «Десерт»...). Каждый наследник задаёт свой.
    virtual std::string course() const = 0;
    /// Как блюдо подаётся гостю. Наследники дополняют текст.
    virtual std::string serve() const;

    /// Приготовить: выполнить рецепт и подать. Бросает DishNotAvailableException, если блюдо закончилось.
    std::vector<std::string> prepare() const;
    /// Калорийность одной порции.
    double caloriesPerServing() const;
    /// Есть ли в блюде аллерген.
    bool hasAllergen(const std::string& allergenName) const;
    /// Вегетарианское ли блюдо (по категории рецепта).
    bool isVegetarian() const;
    /// Отметить, что блюдо закончилось.
    void markSoldOut();
    /// Снова сделать блюдо доступным.
    void markAvailable();

private:
    std::string _name;
    Recipe _recipe;
    PriceTag _price;
    PlatingStyle _plating;
    bool _isAvailable;
};
