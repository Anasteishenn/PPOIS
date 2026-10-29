#include "recipes/Recipe.h"

#include "exceptions/CulinaryException.h"

Recipe::Recipe(const std::string& title, const RecipeCategory& category, int servings)
    : _title(title), _category(category), _servings(servings) {
    if (title.empty() || servings <= 0) {
        throw RecipeException("У рецепта нужно название и положительное число порций");
    }
}

void Recipe::addIngredient(const IngredientProportion& proportion) {
    _proportions.push_back(proportion);
}

void Recipe::addStep(const RecipeStep& step) {
    _steps.push_back(step);
}

void Recipe::addReview(const RecipeReview& review) {
    _reviews.push_back(review);
}

int Recipe::totalTime() const {
    int minutes = 0;
    for (const RecipeStep& step : _steps) {
        minutes += step.duration();
    }
    return minutes;
}

double Recipe::totalCost() const {
    double cost = 0.0;
    for (const IngredientProportion& proportion : _proportions) {
        cost += proportion.cost();
    }
    return cost;
}

double Recipe::costPerServing() const {
    return totalCost() / _servings;
}

double Recipe::averageRating() const {
    if (_reviews.empty()) {
        return 0.0;
    }
    double sum = 0.0;
    for (const RecipeReview& review : _reviews) {
        sum += review.getRating();
    }
    return sum / static_cast<double>(_reviews.size());
}

std::vector<std::string> Recipe::cook() const {
    if (_steps.empty()) {
        throw RecipeException("В рецепте нет шагов: " + _title);
    }
    std::vector<std::string> log;
    for (const RecipeStep& step : _steps) {
        log.push_back(step.perform());
    }
    return log;
}

bool Recipe::containsAllergen(const std::string& allergenName) const {
    for (const IngredientProportion& proportion : _proportions) {
        if (proportion.getIngredient().hasAllergen(allergenName)) {
            return true;
        }
    }
    return false;
}

NutritionalValue Recipe::totalNutrition() const {
    NutritionalValue total;
    for (const IngredientProportion& proportion : _proportions) {
        total = total.add(proportion.nutrition());
    }
    return total;
}
