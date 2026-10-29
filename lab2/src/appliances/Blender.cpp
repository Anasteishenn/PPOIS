#include "appliances/Blender.h"

#include "Format.h"
#include "exceptions/CulinaryException.h"

Blender::Blender(const std::string& model, int powerWatts, int speedLevels, double jugVolumeMl)
    : SafeProcessor(model, powerWatts, jugVolumeMl), _speedLevels(speedLevels), _jugVolumeMl(jugVolumeMl) {
    if (speedLevels <= 0) {
        throw ApplianceException("У блендера должна быть хотя бы одна скорость");
    }
}

std::string Blender::type() const {
    return "Блендер";
}

std::string Blender::blend(const LiquidIngredient& liquid, double milliliters, int minutes) {
    if (milliliters > _jugVolumeMl) {
        throw ApplianceException("Не помещается в чашу блендера");
    }
    safeRun(liquid.toGrams(milliliters), minutes);
    return "Взбито " + format::number(milliliters) + " мл: " + liquid.getName();
}

std::string Blender::crushIce(double grams) {
    if (_speedLevels < ICE_SPEED_LEVEL) {
        throw ApplianceException("Этот блендер не умеет колоть лёд");
    }
    safeRun(grams, ICE_CRUSH_MINUTES);
    return "Наколото льда: " + format::number(grams) + " г";
}
