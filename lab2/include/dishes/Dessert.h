#pragma once

#include "dishes/Dish.h"

/// @brief Десерт: количество сахара, замороженный или нет (мороженое).
class Dessert : public Dish {
public:
    /// Если сахара меньше этого (г), десерт считается «без сахара».
    static constexpr double SUGAR_FREE_LIMIT = 0.5;

    Dessert(const std::string& name, const Recipe& recipe, const PriceTag& price, const PlatingStyle& plating,
            double sugarGrams, bool isFrozen);

    double getSugarGrams() const { return _sugarGrams; }
    bool isFrozen() const { return _isFrozen; }

    std::string course() const override;
    std::string serve() const override;
    /// Можно ли назвать десерт «без сахара».
    bool isSugarFree() const;

private:
    double _sugarGrams;
    bool _isFrozen;
};
