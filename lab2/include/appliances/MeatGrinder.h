#pragma once

#include <string>

#include "appliances/SafeProcessor.h"
#include "ingredients/SolidIngredient.h"

/// @brief Мясорубка: перемалывает продукт со скоростью грамм в минуту через решётку с отверстиями.
class MeatGrinder : public SafeProcessor {
public:
    MeatGrinder(const std::string& model, int powerWatts, double maxLoadGrams, double gramsPerMinute,
                double plateHoleMm);

    double getPlateHoleMm() const { return _plateHoleMm; }
    double getGramsPerMinute() const { return _gramsPerMinute; }

    std::string type() const override;
    /// Сколько минут нужно, чтобы перемолоть заданную массу (с округлением вверх).
    int grindingMinutes(double grams) const;
    /// Перемолоть продукт и вернуть описание результата.
    std::string grind(const SolidIngredient& ingredient, double grams);
    /// Поменять решётку (диаметр отверстий в мм).
    void changePlate(double holeMm);

private:
    double _gramsPerMinute;
    double _plateHoleMm;
};
