#pragma once

#include "appliances/KitchenAppliance.h"
#include "recipes/IngredientProportion.h"

/// @brief Умные кухонные весы: учитывают тару и проверяют, правильно ли отмерена порция.
class SmartScale : public KitchenAppliance {
public:
    static constexpr double PERCENT = 100.0;

    SmartScale(const std::string& model, int powerWatts, double maxWeight);

    double getTare() const { return _tare; }
    double getLastWeight() const { return _lastWeight; }

    std::string type() const override;
    /// Запомнить вес тары (миски), он будет вычитаться при взвешивании.
    void setTare(double grams);
    /// Взвесить: возвращает вес без тары. Бросает ApplianceException при перегрузе.
    double weigh(double rawGrams);
    /// Взвесить продукт и проверить, что вес отличается от рецепта не больше чем на tolerancePercent.
    bool checkPortion(const IngredientProportion& proportion, double rawGrams, double tolerancePercent);

private:
    double _maxWeight;
    double _tare;
    double _lastWeight;
};
