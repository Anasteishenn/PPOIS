#pragma once

#include <string>
#include <vector>

#include "ingredients/NutritionalValue.h"
#include "recipes/IngredientProportion.h"
#include "recipes/RecipeCategory.h"
#include "recipes/RecipeReview.h"
#include "recipes/RecipeStep.h"

/// @brief Рецепт: название, категория, ингредиенты, шаги, отзывы и число порций.
class Recipe {
public:
    Recipe(const std::string& title, const RecipeCategory& category, int servings);

    const std::string& getTitle() const { return _title; }
    const RecipeCategory& getCategory() const { return _category; }
    const std::vector<IngredientProportion>& getProportions() const { return _proportions; }
    const std::vector<RecipeStep>& getSteps() const { return _steps; }
    int getServings() const { return _servings; }

    /// Добавить ингредиент в рецепт.
    void addIngredient(const IngredientProportion& proportion);
    /// Добавить шаг приготовления.
    void addStep(const RecipeStep& step);
    /// Добавить отзыв.
    void addReview(const RecipeReview& review);
    /// Общее время приготовления в минутах (сумма шагов).
    int totalTime() const;
    /// Себестоимость всех ингредиентов.
    double totalCost() const;
    /// Себестоимость одной порции.
    double costPerServing() const;
    /// Средняя оценка по отзывам (0, если отзывов нет).
    double averageRating() const;
    /// Приготовить: выполнить все шаги по порядку. Бросает RecipeException, если шагов нет.
    std::vector<std::string> cook() const;
    /// Есть ли в рецепте ингредиент с таким аллергеном.
    bool containsAllergen(const std::string& allergenName) const;
    /// Пищевая ценность всего рецепта.
    NutritionalValue totalNutrition() const;

private:
    std::string _title;
    RecipeCategory _category;
    std::vector<IngredientProportion> _proportions;
    std::vector<RecipeStep> _steps;
    std::vector<RecipeReview> _reviews;
    int _servings;
};
