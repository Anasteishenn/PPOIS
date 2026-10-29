#include "recipes/RecipeCategory.h"

#include "exceptions/CulinaryException.h"

RecipeCategory::RecipeCategory(const std::string& name, const std::string& cuisine, bool isVegetarian,
                               int difficulty)
    : _name(name), _cuisine(cuisine), _isVegetarian(isVegetarian), _difficulty(difficulty) {
    if (difficulty < MIN_DIFFICULTY || difficulty > MAX_DIFFICULTY) {
        throw RecipeException("Сложность рецепта должна быть от 1 до 5");
    }
}

std::string RecipeCategory::label() const {
    std::string text = _cuisine + " кухня / " + _name;
    return _isVegetarian ? text + " (вегетарианское)" : text;
}

bool RecipeCategory::matches(const std::string& query) const {
    return _name == query || _cuisine == query;
}

bool RecipeCategory::isBeginnerFriendly() const {
    return _difficulty <= EASY_LEVEL;
}
