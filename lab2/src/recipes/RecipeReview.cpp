#include "recipes/RecipeReview.h"

#include "exceptions/CulinaryException.h"

RecipeReview::RecipeReview(const std::string& author, int rating, const std::string& comment)
    : _author(author), _rating(rating), _comment(comment) {
    if (rating < MIN_RATING || rating > MAX_RATING) {
        throw InvalidRatingException("Оценка рецепта должна быть от 1 до 5");
    }
}

bool RecipeReview::isPositive() const {
    return _rating >= POSITIVE_RATING;
}

bool RecipeReview::mentions(const std::string& word) const {
    return !word.empty() && _comment.find(word) != std::string::npos;
}
