/**
 * @file TestData.h
 * @brief Готовые объекты для тестов: ингредиенты, рецепт, блюда, склад, шеф.
 */
#pragma once

#include <memory>
#include <string>

#include "appliances/KitchenInventory.h"
#include "business/Chef.h"
#include "business/Cook.h"
#include "dishes/Appetizer.h"
#include "dishes/Beverage.h"
#include "dishes/Dessert.h"
#include "dishes/MainCourse.h"
#include "ingredients/SolidIngredient.h"
#include "ingredients/Supplier.h"
#include "recipes/MechanicalProcess.h"
#include "recipes/Recipe.h"
#include "recipes/ThermalProcess.h"

namespace testdata {

/// Картофель: 0.01 BYN за грамм, 80 ккал на 100 г, клубень 100 г.
inline std::shared_ptr<SolidIngredient> makePotato() {
    return std::make_shared<SolidIngredient>("Картофель", 0.01, NutritionalValue(80, 2, 0, 17), 100);
}

/// Мука: 0.002 BYN за грамм, 360 ккал на 100 г, содержит глютен.
inline std::shared_ptr<SolidIngredient> makeFlour() {
    auto flour = std::make_shared<SolidIngredient>("Мука", 0.002, NutritionalValue(360, 10, 1, 76), 1000);
    flour->addAllergen(Allergen("Глютен", 3));
    return flour;
}

inline Supplier makeSupplier() {
    return Supplier("Ферма", "+375 29 000-00-00", "Минск", 2);
}

/// Рецепт на 2 порции: картофель 200 г + мука 100 г (себестоимость 2.2, 520 ккал), 2 шага на 25 минут.
inline Recipe makeRecipe(const std::string& title = "Пюре", bool isVegetarian = true) {
    Recipe recipe(title, RecipeCategory("Гарнир", "Белорусская", isVegetarian, 2), 2);
    recipe.addIngredient(IngredientProportion(makePotato(), 200));
    recipe.addIngredient(IngredientProportion(makeFlour(), 100));
    recipe.addStep(RecipeStep(1, "Отварить", std::make_shared<ThermalProcess>("Варка", 20, 100)));
    recipe.addStep(RecipeStep(2, "Размять", std::make_shared<MechanicalProcess>("Толчение", 5, "толкушка", 100)));
    return recipe;
}

inline PlatingStyle makePlating() {
    return PlatingStyle("Классика", "тарелка", "укроп");
}

inline std::shared_ptr<Appetizer> makeAppetizer(const std::string& name = "Драники", double price = 10) {
    return std::make_shared<Appetizer>(name, makeRecipe(name), PriceTag(price), makePlating(), false, 6);
}

inline std::shared_ptr<MainCourse> makeMainCourse(bool isSpicy = false) {
    return std::make_shared<MainCourse>("Бифштекс", makeRecipe("Бифштекс", false), PriceTag(20), makePlating(), "пюре",
                                        isSpicy);
}

inline std::shared_ptr<Beverage> makeBeverage(bool isAlcoholic = false, int temperature = 8) {
    return std::make_shared<Beverage>("Морс", makeRecipe("Морс"), PriceTag(5), makePlating(), 300, isAlcoholic,
                                      temperature);
}

inline std::shared_ptr<Dessert> makeDessert(double sugarGrams = 20, bool isFrozen = true) {
    return std::make_shared<Dessert>("Пломбир", makeRecipe("Пломбир"), PriceTag(7), makePlating(), sugarGrams,
                                     isFrozen);
}

/// Склад с картофелем и мукой (минимальные запасы 300 г и 100 г).
inline KitchenInventory makeInventory(double potatoGrams = 1000, double flourGrams = 500) {
    KitchenInventory inventory("Кухня");
    inventory.addStock(StockItem(makePotato(), potatoGrams, 300, makeSupplier()));
    inventory.addStock(StockItem(makeFlour(), flourGrams, 100, makeSupplier()));
    return inventory;
}

/// Шеф с одним поваром-универсалом.
inline Chef makeChef() {
    Chef chef("Анна", 10, 30, "Бифштекс", 1);
    chef.hireCook(std::make_shared<Cook>("Иван", 6, 20, "горячий цех", ""));
    return chef;
}

}  // namespace testdata
