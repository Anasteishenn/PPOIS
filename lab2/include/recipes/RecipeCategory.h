#pragma once

#include <string>

/// @brief Категория рецепта: тип блюда, кухня, вегетарианское ли, сложность от 1 до 5.
class RecipeCategory {
public:
    static constexpr int MIN_DIFFICULTY = 1;
    static constexpr int MAX_DIFFICULTY = 5;
    /// До этой сложности включительно рецепт подходит новичку.
    static constexpr int EASY_LEVEL = 2;

    RecipeCategory(const std::string& name, const std::string& cuisine, bool isVegetarian, int difficulty);

    const std::string& getName() const { return _name; }
    const std::string& getCuisine() const { return _cuisine; }
    bool isVegetarian() const { return _isVegetarian; }
    int getDifficulty() const { return _difficulty; }

    /// Подпись для меню, например «Итальянская кухня / Паста (вегетарианское)».
    std::string label() const;
    /// Подходит ли категория под поисковый запрос (совпадает тип или кухня).
    bool matches(const std::string& query) const;
    /// Подходит ли рецепт новичку (сложность <= EASY_LEVEL).
    bool isBeginnerFriendly() const;

private:
    std::string _name;
    std::string _cuisine;
    bool _isVegetarian;
    int _difficulty;
};
