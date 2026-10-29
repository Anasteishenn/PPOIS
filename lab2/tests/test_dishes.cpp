#include <memory>

#include "TestData.h"
#include "dishes/ComboMeal.h"
#include "dishes/Menu.h"
#include "dishes/PortionScaler.h"
#include "doctest.h"
#include "exceptions/CulinaryException.h"

using doctest::Approx;

TEST_CASE("PriceTag: скидка и вывод") {
    CHECK_THROWS_AS(PriceTag(-1), InvalidPriceException);
    PriceTag price(20);
    CHECK(price.finalPrice() == Approx(20));
    CHECK(price.format() == "20.00 BYN");
    price.applyDiscount(25);
    CHECK(price.finalPrice() == Approx(15));
    CHECK(price.format() == "15.00 BYN (скидка 25.00%)");
    CHECK_THROWS_AS(price.applyDiscount(95), InvalidPriceException);
    CHECK_THROWS_AS(price.applyDiscount(-5), InvalidPriceException);
    CHECK(price.getBasePrice() == Approx(20));
    CHECK(price.getDiscountPercent() == Approx(25));
    CHECK(price.getCurrency() == "BYN");
}

TEST_CASE("PlatingStyle: описание подачи") {
    PlatingStyle withGarnish = testdata::makePlating();
    CHECK(withGarnish.hasGarnish());
    CHECK(withGarnish.describe() == "подача «Классика», тарелка, украшение: укроп");
    PlatingStyle plain("Простая", "миска");
    CHECK_FALSE(plain.hasGarnish());
    CHECK(plain.describe() == "подача «Простая», миска");
    CHECK(plain.getName() == "Простая");
    CHECK(plain.getPlateType() == "миска");
    CHECK(plain.getGarnish().empty());
}

TEST_CASE("PortionScaler: пересчёт порций") {
    CHECK_THROWS_AS(PortionScaler(0, 2), RecipeException);
    PortionScaler scaler(2, 6);
    CHECK(scaler.factor() == Approx(3));
    Recipe recipe = testdata::makeRecipe();
    std::vector<IngredientProportion> scaled = scaler.scale(recipe);
    REQUIRE(scaled.size() == 2);
    CHECK(scaled[0].getAmount() == Approx(600));
    CHECK(scaler.scaleCost(recipe) == Approx(6.6));
    CHECK(scaler.scalePrice(PriceTag(10)) == Approx(30));
    CHECK(scaler.getBaseServings() == 2);
    CHECK(scaler.getTargetServings() == 6);
}

TEST_CASE("Dish: приготовление, наличие, калории, аллергены") {
    CHECK_THROWS_AS(Appetizer("", testdata::makeRecipe(), PriceTag(1), testdata::makePlating(), true, 1),
                    DishNotFoundException);
    std::shared_ptr<Appetizer> pancakes = testdata::makeAppetizer();
    std::vector<std::string> log = pancakes->prepare();
    REQUIRE(log.size() == 3);
    CHECK(log.back() == pancakes->serve());
    CHECK(pancakes->caloriesPerServing() == Approx(260));
    CHECK(pancakes->hasAllergen("Глютен"));
    CHECK(pancakes->isVegetarian());
    pancakes->markSoldOut();
    CHECK_FALSE(pancakes->isAvailable());
    CHECK_THROWS_AS(pancakes->prepare(), DishNotAvailableException);
    pancakes->markAvailable();
    CHECK(pancakes->isAvailable());
    pancakes->getPrice().applyDiscount(10);
    CHECK(pancakes->getPrice().finalPrice() == Approx(9));
    CHECK(pancakes->getRecipe().getTitle() == "Драники");
    CHECK(pancakes->getPlating().getName() == "Классика");
}

TEST_CASE("Appetizer: подача и деление на компанию") {
    std::shared_ptr<Appetizer> hot = testdata::makeAppetizer();
    CHECK(hot->course() == "Закуска");
    CHECK(hot->serve().rfind("Горячая закуска", 0) == 0);
    Appetizer cold("Нарезка", testdata::makeRecipe(), PriceTag(8), testdata::makePlating(), true, 9);
    CHECK(cold.serve().rfind("Холодная закуска", 0) == 0);
    CHECK(cold.piecesPerPerson(3) == 3);
    CHECK_THROWS_AS(cold.piecesPerPerson(0), TableException);
    CHECK(cold.isCold());
    CHECK(cold.getPieces() == 9);
}

TEST_CASE("MainCourse: гарнир, острота, сочетание с напитком") {
    std::shared_ptr<MainCourse> steak = testdata::makeMainCourse(true);
    CHECK(steak->course() == "Основное блюдо");
    CHECK(steak->serve().find("гарнир: пюре") != std::string::npos);
    CHECK(steak->spiceWarning() == "Острое блюдо!");
    CHECK(steak->pairWith(*testdata::makeBeverage(false, 80)).find("не рекомендуем") != std::string::npos);
    CHECK(steak->pairWith(*testdata::makeBeverage()).find("хорошее сочетание") != std::string::npos);
    MainCourse plain("Котлета", testdata::makeRecipe(), PriceTag(10), testdata::makePlating(), "", false);
    CHECK(plain.serve().find("гарнир") == std::string::npos);
    CHECK(plain.spiceWarning().empty());
    CHECK(steak->isSpicy());
    CHECK(steak->getSideDish() == "пюре");
    CHECK_FALSE(steak->isVegetarian());
}

TEST_CASE("Dessert: сахар и охлаждение") {
    std::shared_ptr<Dessert> iceCream = testdata::makeDessert();
    CHECK(iceCream->course() == "Десерт");
    CHECK(iceCream->serve().find("охлаждённым") != std::string::npos);
    CHECK_FALSE(iceCream->isSugarFree());
    std::shared_ptr<Dessert> fruit = testdata::makeDessert(0, false);
    CHECK(fruit->isSugarFree());
    CHECK(fruit->serve().find("охлаждённым") == std::string::npos);
    CHECK(iceCream->isFrozen());
    CHECK(iceCream->getSugarGrams() == Approx(20));
}

TEST_CASE("Beverage: возраст, температура, подача") {
    CHECK_THROWS_AS(Beverage("Сок", testdata::makeRecipe(), PriceTag(3), testdata::makePlating(), 0, false, 5),
                    DishNotFoundException);
    std::shared_ptr<Beverage> wine = testdata::makeBeverage(true, 80);
    CHECK(wine->course() == "Напиток");
    CHECK_THROWS_AS(wine->checkAge(16), AgeRestrictionException);
    CHECK_NOTHROW(wine->checkAge(20));
    CHECK_NOTHROW(testdata::makeBeverage()->checkAge(10));
    CHECK(wine->isHot());
    CHECK(wine->serve().find("горячий") != std::string::npos);
    CHECK(testdata::makeBeverage()->serve().find("горячий") == std::string::npos);
    CHECK(wine->getVolumeMl() == 300);
    CHECK(wine->isAlcoholic());
    CHECK(wine->getServingTemperature() == 80);
}

TEST_CASE("ComboMeal: цена со скидкой, калории, напиток") {
    CHECK_THROWS_AS(ComboMeal("Ланч", 95), InvalidPriceException);
    ComboMeal lunch("Ланч", 10);
    CHECK_THROWS_AS(lunch.addDish(nullptr), DishNotFoundException);
    lunch.addDish(testdata::makeAppetizer());
    CHECK_FALSE(lunch.hasBeverage());
    lunch.addDish(testdata::makeBeverage());
    CHECK(lunch.hasBeverage());
    CHECK(lunch.fullPrice() == Approx(15));
    CHECK(lunch.totalPrice() == Approx(13.5));
    CHECK(lunch.savings() == Approx(1.5));
    CHECK(lunch.totalCalories() == Approx(520));
    CHECK(lunch.getTitle() == "Ланч");
    CHECK(lunch.getDishes().size() == 2);
}

TEST_CASE("Menu: поиск, удаление, фильтры") {
    Menu menu("Основное", "Осень");
    CHECK_THROWS_AS(menu.cheapestDish(), DishNotFoundException);
    CHECK_THROWS_AS(menu.addDish(nullptr), DishNotFoundException);
    std::shared_ptr<Appetizer> pancakes = testdata::makeAppetizer();
    menu.addDish(pancakes);
    menu.addDish(testdata::makeMainCourse());
    menu.addDish(testdata::makeBeverage());
    CHECK(menu.findDish("Бифштекс")->course() == "Основное блюдо");
    CHECK_THROWS_AS(menu.findDish("Суп"), DishNotFoundException);
    CHECK(menu.cheapestDish()->getName() == "Морс");
    pancakes->markSoldOut();
    CHECK(menu.availableDishes().size() == 2);
    CHECK(menu.vegetarianDishes().size() == 2);
    CHECK(menu.withoutAllergen("Глютен").empty());
    CHECK(menu.withoutAllergen("Лактоза").size() == 3);
    menu.addCombo(ComboMeal("Ланч", 10));
    CHECK(menu.getCombos().size() == 1);
    menu.removeDish("Морс");
    CHECK(menu.getDishes().size() == 2);
    CHECK_THROWS_AS(menu.removeDish("Морс"), DishNotFoundException);
    CHECK(menu.getTitle() == "Основное");
    CHECK(menu.getSeason() == "Осень");
}
