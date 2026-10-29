#pragma once

#include "ingredients/Ingredient.h"

/// @brief Жидкий ингредиент (молоко, масло, бульон). Измеряется в миллилитрах.
class LiquidIngredient : public Ingredient {
public:
    LiquidIngredient(const std::string& name, double pricePerMl, const NutritionalValue& nutrition, double density,
                     int boilingPoint);

    double getDensity() const { return _density; }
    int getBoilingPoint() const { return _boilingPoint; }

    std::string unit() const override;
    /// Переводит миллилитры в граммы с учётом плотности.
    double toGrams(double milliliters) const;
    /// Закипает ли жидкость при заданной температуре.
    bool boilsAt(int temperature) const;

private:
    double _density;
    int _boilingPoint;
};
