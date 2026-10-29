#include <memory>

#include "TestData.h"
#include "doctest.h"
#include "exceptions/CulinaryException.h"
#include "ingredients/GaseousIngredient.h"
#include "ingredients/IngredientPackage.h"
#include "ingredients/LiquidIngredient.h"
#include "ingredients/PerishableIngredient.h"
#include "ingredients/StockItem.h"

using doctest::Approx;

TEST_CASE("NutritionalValue: масштабирование, сложение, низкая калорийность") {
    NutritionalValue apple(50, 1, 0, 12);
    CHECK_THROWS_AS(NutritionalValue(-1, 0, 0, 0), InvalidIngredientException);
    CHECK(apple.scale(2).getCalories() == Approx(100));
    NutritionalValue sum = apple.add(NutritionalValue(10, 2, 3, 4));
    CHECK(sum.getCalories() == Approx(60));
    CHECK(sum.getProteins() == Approx(3));
    CHECK(sum.getFats() == Approx(3));
    CHECK(sum.getCarbs() == Approx(16));
    CHECK(apple.isLowCalorie());
    CHECK_FALSE(NutritionalValue(300).isLowCalorie());
}

TEST_CASE("Allergen: проверки, опасность, предупреждение") {
    CHECK_THROWS_AS(Allergen("", 3), InvalidIngredientException);
    CHECK_THROWS_AS(Allergen("Орехи", 0), InvalidIngredientException);
    CHECK_THROWS_AS(Allergen("Орехи", 6), InvalidIngredientException);
    Allergen nuts("Орехи", 5, "арахис");
    CHECK(nuts.isSevere());
    CHECK_FALSE(Allergen("Лактоза", 2).isSevere());
    CHECK(nuts.matches("Орехи"));
    CHECK_FALSE(nuts.matches("Глютен"));
    CHECK(nuts.warning().find("Орехи") != std::string::npos);
    CHECK(nuts.getDescription() == "арахис");
    CHECK(nuts.getSeverity() == 5);
}

TEST_CASE("Supplier: рейтинг, надёжность, доставка") {
    CHECK_THROWS_AS(Supplier("", "1", "Минск", 1), InvalidIngredientException);
    CHECK_THROWS_AS(Supplier("Ферма", "1", "Минск", -1), InvalidIngredientException);
    CHECK_THROWS_AS(Supplier("Ферма", "1", "Минск", 1, 6), InvalidRatingException);
    Supplier farm = testdata::makeSupplier();
    CHECK(farm.isReliable());
    farm.rate(1);
    CHECK(farm.getRating() == Approx(3));
    CHECK_FALSE(farm.isReliable());
    CHECK_THROWS_AS(farm.rate(10), InvalidRatingException);
    CHECK(farm.deliveryDay(5) == 7);
    CHECK(farm.getDeliveryDays() == 2);
    CHECK(farm.contactCard().find("Минск") != std::string::npos);
    CHECK(farm.getName() == "Ферма");
}

TEST_CASE("SolidIngredient и базовый Ingredient") {
    CHECK_THROWS_AS(SolidIngredient("", 1, NutritionalValue(), 1), InvalidIngredientException);
    CHECK_THROWS_AS(SolidIngredient("Лук", -1, NutritionalValue(), 1), InvalidIngredientException);
    CHECK_THROWS_AS(SolidIngredient("Лук", 1, NutritionalValue(), 0), InvalidIngredientException);
    std::shared_ptr<SolidIngredient> potato = testdata::makePotato();
    CHECK(potato->unit() == "г");
    CHECK(potato->describe() == "Картофель (г)");
    CHECK_FALSE(potato->isChopped());
    potato->chop();
    CHECK(potato->isChopped());
    CHECK(potato->describe() == "Картофель (г), нарезан");
    CHECK(potato->piecesFor(250) == 3);
    CHECK_THROWS_AS(potato->piecesFor(-1), InvalidIngredientException);
    CHECK(potato->getPieceWeight() == Approx(100));
    CHECK(potato->costFor(200) == Approx(2));
    CHECK_THROWS_AS(potato->costFor(-5), InvalidIngredientException);
    CHECK(potato->nutritionFor(200).getCalories() == Approx(160));
    CHECK(potato->getPricePerUnit() == Approx(0.01));
}

TEST_CASE("Ingredient: аллергены без повторов") {
    std::shared_ptr<SolidIngredient> flour = testdata::makeFlour();
    flour->addAllergen(Allergen("Глютен", 3));
    CHECK(flour->getAllergens().size() == 1);
    CHECK(flour->hasAllergen("Глютен"));
    CHECK_FALSE(flour->hasAllergen("Лактоза"));
}

TEST_CASE("LiquidIngredient: граммы и кипение") {
    CHECK_THROWS_AS(LiquidIngredient("Молоко", 1, NutritionalValue(), 0, 100), InvalidIngredientException);
    LiquidIngredient milk("Молоко", 0.002, NutritionalValue(52), 1.03, 100);
    CHECK(milk.unit() == "мл");
    CHECK(milk.toGrams(100) == Approx(103));
    CHECK(milk.boilsAt(100));
    CHECK_FALSE(milk.boilsAt(80));
    CHECK(milk.getDensity() == Approx(1.03));
    CHECK(milk.getBoilingPoint() == 100);
}

TEST_CASE("GaseousIngredient: опасность и хранение") {
    CHECK_THROWS_AS(GaseousIngredient("CO2", 1, 0, false), InvalidIngredientException);
    GaseousIngredient co2("CO2", 0.5, 5, false);
    CHECK(co2.unit() == "л");
    CHECK_FALSE(co2.isDangerous());
    CHECK(GaseousIngredient("Пропан", 1, 2, true).isDangerous());
    CHECK(GaseousIngredient("N2O", 1, 20, false).isDangerous());
    CHECK(co2.canBeStoredUnder(6));
    CHECK_FALSE(co2.canBeStoredUnder(4));
    CHECK(co2.getPressure() == Approx(5));
    CHECK_FALSE(co2.isFlammable());
}

TEST_CASE("PerishableIngredient: срок годности и заморозка") {
    PerishableIngredient fish("Рыба", 0.02, NutritionalValue(120), 10, 2);
    CHECK(fish.unit() == "г");
    CHECK_FALSE(fish.isExpired(10));
    CHECK(fish.isExpired(11));
    CHECK(fish.daysLeft(7) == 3);
    CHECK(fish.daysLeft(20) == 0);
    CHECK_NOTHROW(fish.checkFresh(9));
    CHECK_THROWS_AS(fish.checkFresh(11), ExpiredIngredientException);
    fish.freeze();
    fish.freeze();
    CHECK(fish.isFrozen());
    CHECK(fish.getExpiryDay() == 10 + PerishableIngredient::FREEZE_EXTRA_DAYS);
    CHECK(fish.getStorageTemperature() == 2);
}

TEST_CASE("IngredientPackage: количество, цена, выдача упаковок") {
    CHECK_THROWS_AS(IngredientPackage(nullptr, 1000, 1, "пачка"), InvalidIngredientException);
    IngredientPackage flour(testdata::makeFlour(), 1000, 3, "пачка");
    CHECK(flour.totalAmount() == Approx(3000));
    CHECK(flour.totalPrice() == Approx(6));
    CHECK(flour.label().find("3 x пачка") != std::string::npos);
    flour.takePackages(2);
    CHECK(flour.getPackageCount() == 1);
    CHECK_THROWS_AS(flour.takePackages(5), InsufficientStockException);
    CHECK_THROWS_AS(flour.takePackages(0), InsufficientStockException);
}

TEST_CASE("StockItem: списание, пополнение, дозаказ") {
    CHECK_THROWS_AS(StockItem(nullptr, 1, 1, testdata::makeSupplier()), InvalidIngredientException);
    StockItem potatoes(testdata::makePotato(), 500, 300, testdata::makeSupplier());
    CHECK_FALSE(potatoes.needsRestock());
    CHECK(potatoes.amountToOrder() == Approx(0));
    potatoes.consume(300);
    CHECK(potatoes.getQuantity() == Approx(200));
    CHECK(potatoes.needsRestock());
    CHECK(potatoes.amountToOrder() == Approx(400));
    CHECK_THROWS_AS(potatoes.consume(1000), InsufficientStockException);
    CHECK_THROWS_AS(potatoes.consume(0), InvalidIngredientException);
    potatoes.restock(100);
    CHECK(potatoes.getQuantity() == Approx(300));
    CHECK_THROWS_AS(potatoes.restock(-1), InvalidIngredientException);
    potatoes.receivePackage(IngredientPackage(testdata::makePotato(), 1000, 2, "мешок"));
    CHECK(potatoes.getQuantity() == Approx(2300));
    IngredientPackage flour(testdata::makeFlour(), 1000, 1, "пачка");
    CHECK_THROWS_AS(potatoes.receivePackage(flour), InvalidIngredientException);
    CHECK(potatoes.value() == Approx(23));
    CHECK(potatoes.getSupplier().getName() == "Ферма");
}
