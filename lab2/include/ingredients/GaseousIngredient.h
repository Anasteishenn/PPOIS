#pragma once

#include "ingredients/Ingredient.h"

/// @brief Газообразный ингредиент (углекислый газ для газировки, закись азота для сливок).
class GaseousIngredient : public Ingredient {
public:
    /// При давлении выше этого (в атмосферах) газ считается опасным.
    static constexpr double DANGER_PRESSURE = 10.0;

    GaseousIngredient(const std::string& name, double pricePerLiter, double pressure, bool isFlammable);

    double getPressure() const { return _pressure; }
    bool isFlammable() const { return _isFlammable; }

    std::string unit() const override;
    /// Опасен ли газ: горючий или под большим давлением.
    bool isDangerous() const;
    /// Можно ли хранить газ в баллоне, рассчитанном на заданное давление.
    bool canBeStoredUnder(double maxPressure) const;

private:
    double _pressure;
    bool _isFlammable;
};
