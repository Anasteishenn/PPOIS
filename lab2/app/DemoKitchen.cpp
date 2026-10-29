#include "DemoKitchen.h"

#include <memory>

#include "appliances/Blender.h"
#include "appliances/FoodProcessor.h"
#include "appliances/MeatGrinder.h"
#include "appliances/Oven.h"
#include "appliances/SmartScale.h"
#include "appliances/StoveTop.h"
#include "dishes/Appetizer.h"
#include "dishes/Beverage.h"
#include "dishes/Dessert.h"
#include "dishes/MainCourse.h"
#include "ingredients/LiquidIngredient.h"
#include "ingredients/PerishableIngredient.h"
#include "ingredients/SolidIngredient.h"
#include "recipes/ChemicalProcess.h"
#include "recipes/MechanicalProcess.h"
#include "recipes/ThermalProcess.h"

namespace demo {

namespace {

// Все числа ниже — это данные примера (цены, граммы, минуты), а не настройки программы.

struct Pantry {
    std::shared_ptr<SolidIngredient> potato, flour, sugar, lemon, egg;
    std::shared_ptr<PerishableIngredient> beef, cream;
    std::shared_ptr<LiquidIngredient> milk, water;
};

Pantry createPantry() {
    Pantry pantry;
    pantry.potato = std::make_shared<SolidIngredient>("Картофель", 0.002, NutritionalValue(77, 2, 0.1, 17), 150);
    pantry.flour = std::make_shared<SolidIngredient>("Мука", 0.003, NutritionalValue(364, 10, 1, 76), 1000);
    pantry.sugar = std::make_shared<SolidIngredient>("Сахар", 0.002, NutritionalValue(387, 0, 0, 100), 5);
    pantry.lemon = std::make_shared<SolidIngredient>("Лимон", 0.006, NutritionalValue(29, 1, 0.3, 9), 100);
    pantry.egg = std::make_shared<SolidIngredient>("Яйцо", 0.004, NutritionalValue(155, 13, 11, 1), 50);
    pantry.beef = std::make_shared<PerishableIngredient>("Говядина", 0.03, NutritionalValue(250, 26, 15, 0), 10, 4);
    pantry.cream = std::make_shared<PerishableIngredient>("Сливки", 0.012, NutritionalValue(200, 3, 20, 4), 3, 4);
    pantry.milk = std::make_shared<LiquidIngredient>("Молоко", 0.002, NutritionalValue(52, 3, 2.5, 4.7), 1.03, 100);
    pantry.water = std::make_shared<LiquidIngredient>("Вода", 0.0005, NutritionalValue(), 1.0, 100);
    pantry.flour->addAllergen(Allergen("Глютен", 3, "белок злаков"));
    pantry.egg->addAllergen(Allergen("Яйца", 4));
    pantry.cream->addAllergen(Allergen("Лактоза", 2, "молочный сахар"));
    pantry.milk->addAllergen(Allergen("Лактоза", 2, "молочный сахар"));
    return pantry;
}

std::shared_ptr<Dish> createPancakes(const Pantry& pantry) {
    Recipe recipe("Драники", RecipeCategory("Закуска", "Белорусская", true, 2), 2);
    recipe.addIngredient(IngredientProportion(pantry.potato, 300));
    recipe.addIngredient(IngredientProportion(pantry.flour, 30));
    recipe.addIngredient(IngredientProportion(pantry.egg, 50));
    recipe.addStep(RecipeStep(1, "Натереть картофель", std::make_shared<MechanicalProcess>("Натирание", 5, "тёрка", 120)));
    recipe.addStep(RecipeStep(2, "Обжарить с двух сторон", std::make_shared<ThermalProcess>("Жарка", 10, 180)));
    PlatingStyle plating("Деревенская", "глиняная тарелка", "сметана и укроп");
    return std::make_shared<Appetizer>("Драники", recipe, PriceTag(9.5), plating, false, 6);
}

std::shared_ptr<Dish> createSteak(const Pantry& pantry) {
    Recipe recipe("Бифштекс", RecipeCategory("Горячее", "Европейская", false, 3), 1);
    recipe.addIngredient(IngredientProportion(pantry.beef, 250));
    recipe.addIngredient(IngredientProportion(pantry.potato, 200));
    recipe.addIngredient(IngredientProportion(pantry.cream, 50));
    recipe.addStep(RecipeStep(1, "Замариновать мясо", std::make_shared<ChemicalProcess>("Маринование", 30, "уксус", 3.5)));
    recipe.addStep(RecipeStep(2, "Обжарить мясо", std::make_shared<ThermalProcess>("Жарка", 15, 220)));
    recipe.addStep(RecipeStep(3, "Отварить картофель", std::make_shared<ThermalProcess>("Варка", 20, 100)));
    PlatingStyle plating("Классика", "плоская белая тарелка", "розмарин");
    return std::make_shared<MainCourse>("Бифштекс", recipe, PriceTag(24), plating, "картофельное пюре", false);
}

std::shared_ptr<Dish> createIceCream(const Pantry& pantry) {
    Recipe recipe("Пломбир", RecipeCategory("Десерт", "Европейская", true, 2), 2);
    recipe.addIngredient(IngredientProportion(pantry.cream, 150));
    recipe.addIngredient(IngredientProportion(pantry.milk, 100));
    recipe.addIngredient(IngredientProportion(pantry.sugar, 20));
    recipe.addStep(RecipeStep(1, "Взбить сливки", std::make_shared<MechanicalProcess>("Взбивание", 5, "миксер", 1200)));
    recipe.addStep(RecipeStep(2, "Заморозить", std::make_shared<MechanicalProcess>("Заморозка", 30, "мороженица", 60)));
    PlatingStyle plating("Креманка", "стеклянная креманка", "мята");
    return std::make_shared<Dessert>("Пломбир", recipe, PriceTag(7), plating, 20, true);
}

std::shared_ptr<Dish> createLemonade(const Pantry& pantry) {
    Recipe recipe("Лимонад", RecipeCategory("Напиток", "Домашняя", true, 1), 1);
    recipe.addIngredient(IngredientProportion(pantry.water, 250));
    recipe.addIngredient(IngredientProportion(pantry.lemon, 50));
    recipe.addIngredient(IngredientProportion(pantry.sugar, 15));
    recipe.addStep(RecipeStep(1, "Выжать лимон", std::make_shared<MechanicalProcess>("Отжим", 3, "соковыжималка", 500)));
    recipe.addStep(RecipeStep(2, "Смешать с водой", std::make_shared<MechanicalProcess>("Смешивание", 2, "ложка", 60)));
    PlatingStyle plating("Летняя", "высокий стакан", "долька лимона");
    return std::make_shared<Beverage>("Лимонад", recipe, PriceTag(4.5), plating, 300, false, 8);
}

std::shared_ptr<Dish> createTea(const Pantry& pantry) {
    Recipe recipe("Чай с лимоном", RecipeCategory("Напиток", "Домашняя", true, 1), 1);
    recipe.addIngredient(IngredientProportion(pantry.water, 200));
    recipe.addIngredient(IngredientProportion(pantry.lemon, 20));
    recipe.addStep(RecipeStep(1, "Вскипятить воду", std::make_shared<ThermalProcess>("Кипячение", 5, 100)));
    PlatingStyle plating("Классика", "чашка с блюдцем");
    return std::make_shared<Beverage>("Чай с лимоном", recipe, PriceTag(3), plating, 250, false, 80);
}

Menu createMenu(const Pantry& pantry) {
    Menu menu("Основное меню", "Осень");
    std::shared_ptr<Dish> pancakes = createPancakes(pantry);
    std::shared_ptr<Dish> steak = createSteak(pantry);
    std::shared_ptr<Dish> lemonade = createLemonade(pantry);
    menu.addDish(pancakes);
    menu.addDish(steak);
    menu.addDish(createIceCream(pantry));
    menu.addDish(lemonade);
    menu.addDish(createTea(pantry));
    ComboMeal lunch("Бизнес-ланч", 15);
    lunch.addDish(pancakes);
    lunch.addDish(steak);
    lunch.addDish(lemonade);
    menu.addCombo(lunch);
    return menu;
}

KitchenInventory createInventory(const Pantry& pantry) {
    Supplier farm("ООО Фермер", "+375 29 111-22-33", "Минск", 2);
    KitchenInventory inventory("Кухня, 1 этаж");
    inventory.addStock(StockItem(pantry.potato, 5000, 2000, farm));
    inventory.addStock(StockItem(pantry.flour, 2000, 500, farm));
    inventory.addStock(StockItem(pantry.sugar, 1000, 300, farm));
    inventory.addStock(StockItem(pantry.lemon, 600, 300, farm));
    inventory.addStock(StockItem(pantry.egg, 500, 300, farm));
    inventory.addStock(StockItem(pantry.beef, 1000, 1000, farm));
    inventory.addStock(StockItem(pantry.cream, 400, 500, farm));
    inventory.addStock(StockItem(pantry.milk, 2000, 1000, farm));
    inventory.addStock(StockItem(pantry.water, 20000, 5000, farm));
    inventory.addAppliance(std::make_shared<Oven>("Bosch HBA", 3500, 3, true));
    inventory.addAppliance(std::make_shared<StoveTop>("Gefest", 6000, 4, true));
    inventory.addAppliance(std::make_shared<Blender>("Philips", 800, 5, 1500));
    inventory.addAppliance(std::make_shared<FoodProcessor>("Kenwood", 1000, 2000));
    inventory.addAppliance(std::make_shared<MeatGrinder>("Moulinex", 1600, 3000, 1500, 6));
    inventory.addAppliance(std::make_shared<SmartScale>("Xiaomi", 5, 5000));
    return inventory;
}

Chef createChef() {
    Chef chef("Анна Петрова", 12, 30, "Бифштекс", 1);
    chef.hireCook(std::make_shared<Cook>("Иван", 6, 18, "горячий цех", "Основное блюдо"));
    chef.hireCook(std::make_shared<Cook>("Мария", 3, 15, "холодный цех", "Закуска"));
    chef.hireCook(std::make_shared<Cook>("Олег", 1, 12, "бар и десерты", ""));
    return chef;
}

}  // namespace

Kitchen createKitchen() {
    Pantry pantry = createPantry();
    Kitchen kitchen("Бульбаш", createMenu(pantry), createInventory(pantry), createChef());
    kitchen.addTable(RestaurantTable(1, 2));
    kitchen.addTable(RestaurantTable(2, 4));
    kitchen.addTable(RestaurantTable(3, 6));
    return kitchen;
}

}  // namespace demo
