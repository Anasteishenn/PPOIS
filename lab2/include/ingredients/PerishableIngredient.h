#pragma once

#include "ingredients/Ingredient.h"

/// @brief Скоропортящийся ингредиент (рыба, сливки). Срок годности хранится как номер дня.
class PerishableIngredient : public Ingredient {
public:
    /// На столько дней заморозка продлевает срок годности.
    static constexpr int FREEZE_EXTRA_DAYS = 30;

    PerishableIngredient(const std::string& name, double pricePerGram, const NutritionalValue& nutrition,
                         int expiryDay, int storageTemperature);

    int getExpiryDay() const { return _expiryDay; }
    int getStorageTemperature() const { return _storageTemperature; }
    bool isFrozen() const { return _isFrozen; }

    std::string unit() const override;
    /// Просрочен ли продукт в заданный день.
    bool isExpired(int today) const;
    /// Сколько дней осталось до конца срока (0, если уже просрочен).
    int daysLeft(int today) const;
    /// Бросает ExpiredIngredientException, если продукт просрочен.
    void checkFresh(int today) const;
    /// Заморозить продукт: срок годности увеличивается на FREEZE_EXTRA_DAYS (только один раз).
    void freeze();

private:
    int _expiryDay;
    int _storageTemperature;
    bool _isFrozen;
};
