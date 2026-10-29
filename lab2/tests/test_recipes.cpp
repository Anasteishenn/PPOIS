#include <memory>

#include "TestData.h"
#include "doctest.h"
#include "exceptions/CulinaryException.h"
#include "recipes/ChemicalProcess.h"

using doctest::Approx;

TEST_CASE("RecipeTimer: запуск, отсчёт, сброс") {
    CHECK_THROWS_AS(RecipeTimer(0), RecipeException);
    RecipeTimer timer(10);
    CHECK_THROWS_AS(timer.tick(1), RecipeException);
    timer.start();
    CHECK(timer.isRunning());
    timer.tick(4);
    CHECK(timer.remaining() == 6);
    CHECK_FALSE(timer.isFinished());
    timer.tick(100);
    CHECK(timer.isFinished());
    CHECK(timer.remaining() == 0);
    CHECK_FALSE(timer.isRunning());
    timer.reset();
    CHECK(timer.remaining() == timer.getDuration());
}

TEST_CASE("ThermalProcess: температура и выполнение") {
    CHECK_THROWS_AS(ThermalProcess("Жарка", 10, 10), InvalidTemperatureException);
    CHECK_THROWS_AS(ThermalProcess("Жарка", 10, 400), InvalidTemperatureException);
    ThermalProcess frying("Жарка", 10, 200);
    CHECK(frying.execute() == "Жарка при 200°C, 10 мин");
    CHECK(frying.requiresHeat());
    CHECK(frying.isHighHeat());
    CHECK_FALSE(ThermalProcess("Варка", 20, 100).isHighHeat());
    CHECK(frying.run() == frying.execute());
    CHECK(frying.getTimer().isFinished());
    CHECK(frying.getMethod() == "Жарка");
    CHECK(frying.getTemperature() == 200);
}

TEST_CASE("MechanicalProcess: скорость и описание") {
    CHECK_THROWS_AS(MechanicalProcess("Взбивание", 5, "венчик", -1), RecipeException);
    MechanicalProcess whisking("Взбивание", 5, "венчик", 1200);
    CHECK(whisking.execute() == "Взбивание: венчик, 1200 об/мин, 5 мин");
    CHECK(whisking.isFast());
    CHECK_FALSE(whisking.requiresHeat());
    CHECK_FALSE(MechanicalProcess("Нарезка", 5, "нож", 100).isFast());
    CHECK(whisking.getTool() == "венчик");
    CHECK(whisking.getSpeed() == 1200);
}

TEST_CASE("ChemicalProcess: кислотность") {
    CHECK_THROWS_AS(ChemicalProcess("Маринование", 60, "уксус", 15), RecipeException);
    CHECK_THROWS_AS(ChemicalProcess("Маринование", 60, "уксус", -1), RecipeException);
    ChemicalProcess marinating("Маринование", 60, "уксус", 3.5);
    CHECK(marinating.execute() == "Маринование (уксус, pH 3.50), 60 мин");
    CHECK(marinating.isAcidic());
    CHECK_FALSE(ChemicalProcess("Засолка", 60, "соль", 7).isAcidic());
    CHECK(marinating.getAgent() == "уксус");
    CHECK(marinating.getPhLevel() == Approx(3.5));
}

TEST_CASE("IngredientProportion: пересчёт, стоимость, описание") {
    CHECK_THROWS_AS(IngredientProportion(nullptr, 10), InvalidIngredientException);
    CHECK_THROWS_AS(IngredientProportion(testdata::makePotato(), 0), InvalidIngredientException);
    IngredientProportion potatoes(testdata::makePotato(), 200);
    CHECK(potatoes.scaled(1.5).getAmount() == Approx(300));
    CHECK(potatoes.cost() == Approx(2));
    CHECK(potatoes.nutrition().getCalories() == Approx(160));
    CHECK(potatoes.describe() == "Картофель: 200.00 г");
    CHECK(potatoes.getIngredient().getName() == "Картофель");
}

TEST_CASE("RecipeStep: выполнение шага") {
    auto boiling = std::make_shared<ThermalProcess>("Варка", 20, 100);
    CHECK_THROWS_AS(RecipeStep(0, "Сварить", boiling), RecipeException);
    CHECK_THROWS_AS(RecipeStep(1, "", boiling), RecipeException);
    CHECK_THROWS_AS(RecipeStep(1, "Сварить", nullptr), RecipeException);
    RecipeStep step(1, "Сварить", boiling);
    CHECK(step.perform() == "Шаг 1: Сварить — Варка при 100°C, 20 мин");
    CHECK(step.duration() == 20);
    CHECK(step.getNumber() == 1);
    CHECK(step.getInstruction() == "Сварить");
    CHECK(step.getProcess().getName() == "Варка");
}

TEST_CASE("RecipeCategory: подпись, поиск, сложность") {
    CHECK_THROWS_AS(RecipeCategory("Суп", "Русская", false, 0), RecipeException);
    CHECK_THROWS_AS(RecipeCategory("Суп", "Русская", false, 6), RecipeException);
    RecipeCategory pasta("Паста", "Итальянская", true, 2);
    CHECK(pasta.label() == "Итальянская кухня / Паста (вегетарианское)");
    CHECK(RecipeCategory("Суп", "Русская", false, 3).label() == "Русская кухня / Суп");
    CHECK(pasta.matches("Паста"));
    CHECK(pasta.matches("Итальянская"));
    CHECK_FALSE(pasta.matches("Суп"));
    CHECK(pasta.isBeginnerFriendly());
    CHECK_FALSE(RecipeCategory("Суп", "Русская", false, 4).isBeginnerFriendly());
    CHECK(pasta.getDifficulty() == 2);
}

TEST_CASE("RecipeReview: оценка и поиск слова") {
    CHECK_THROWS_AS(RecipeReview("Гость", 0, "плохо"), InvalidRatingException);
    CHECK_THROWS_AS(RecipeReview("Гость", 6, "супер"), InvalidRatingException);
    RecipeReview review("Гость", 5, "очень вкусно");
    CHECK(review.isPositive());
    CHECK_FALSE(RecipeReview("Гость", 2, "так себе").isPositive());
    CHECK(review.mentions("вкусно"));
    CHECK_FALSE(review.mentions("солёно"));
    CHECK_FALSE(review.mentions(""));
    CHECK(review.getAuthor() == "Гость");
    CHECK(review.getComment() == "очень вкусно");
}

TEST_CASE("Recipe: время, стоимость, рейтинг, готовка, пищевая ценность") {
    CHECK_THROWS_AS(Recipe("", RecipeCategory("Суп", "Русская", false, 1), 1), RecipeException);
    CHECK_THROWS_AS(Recipe("Суп", RecipeCategory("Суп", "Русская", false, 1), 0), RecipeException);
    Recipe recipe = testdata::makeRecipe();
    CHECK(recipe.totalTime() == 25);
    CHECK(recipe.totalCost() == Approx(2.2));
    CHECK(recipe.costPerServing() == Approx(1.1));
    CHECK(recipe.averageRating() == Approx(0));
    recipe.addReview(RecipeReview("Анна", 5, "отлично"));
    recipe.addReview(RecipeReview("Олег", 4, "хорошо"));
    CHECK(recipe.averageRating() == Approx(4.5));
    std::vector<std::string> log = recipe.cook();
    REQUIRE(log.size() == 2);
    CHECK(log[0].find("Шаг 1") != std::string::npos);
    CHECK(recipe.containsAllergen("Глютен"));
    CHECK_FALSE(recipe.containsAllergen("Лактоза"));
    CHECK(recipe.totalNutrition().getCalories() == Approx(520));
    CHECK(recipe.getSteps().size() == 2);
    CHECK(recipe.getCategory().getName() == "Гарнир");
    Recipe empty("Пусто", RecipeCategory("Суп", "Русская", false, 1), 1);
    CHECK_THROWS_AS(empty.cook(), RecipeException);
}
