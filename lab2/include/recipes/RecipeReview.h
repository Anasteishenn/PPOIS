#pragma once

#include <string>

/// @brief Отзыв о рецепте: автор, оценка от 1 до 5 и комментарий.
class RecipeReview {
public:
    static constexpr int MIN_RATING = 1;
    static constexpr int MAX_RATING = 5;
    /// С этой оценки отзыв считается положительным.
    static constexpr int POSITIVE_RATING = 4;

    RecipeReview(const std::string& author, int rating, const std::string& comment);

    const std::string& getAuthor() const { return _author; }
    int getRating() const { return _rating; }
    const std::string& getComment() const { return _comment; }

    /// Положительный ли отзыв (оценка >= POSITIVE_RATING).
    bool isPositive() const;
    /// Упоминается ли слово в комментарии.
    bool mentions(const std::string& word) const;

private:
    std::string _author;
    int _rating;
    std::string _comment;
};
