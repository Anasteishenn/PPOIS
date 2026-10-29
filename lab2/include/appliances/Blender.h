#pragma once

#include <string>

#include "appliances/SafeProcessor.h"
#include "ingredients/LiquidIngredient.h"

/// @brief Блендер: смешивает жидкости в чаше заданного объёма, на высоких скоростях колет лёд.
class Blender : public SafeProcessor {
public:
    /// Чтобы колоть лёд, нужно хотя бы столько скоростей.
    static constexpr int ICE_SPEED_LEVEL = 3;
    /// Сколько минут колется лёд.
    static constexpr int ICE_CRUSH_MINUTES = 1;

    Blender(const std::string& model, int powerWatts, int speedLevels, double jugVolumeMl);

    int getSpeedLevels() const { return _speedLevels; }
    double getJugVolumeMl() const { return _jugVolumeMl; }

    std::string type() const override;
    /// Взбить жидкость. Объём не должен превышать объём чаши.
    std::string blend(const LiquidIngredient& liquid, double milliliters, int minutes);
    /// Наколоть лёд (нужно не меньше ICE_SPEED_LEVEL скоростей).
    std::string crushIce(double grams);

private:
    int _speedLevels;
    double _jugVolumeMl;
};
