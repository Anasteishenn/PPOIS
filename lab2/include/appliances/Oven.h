#pragma once

#include <string>

#include "appliances/ThermalAppliance.h"
#include "recipes/Recipe.h"

/// @brief Духовка: несколько противней, может быть с конвекцией (тогда греется быстрее).
class Oven : public ThermalAppliance {
public:
    static constexpr int MAX_TEMPERATURE = 280;
    /// Обычная скорость нагрева, градусов в минуту.
    static constexpr int DEGREES_PER_MINUTE = 10;
    /// Во сколько раз конвекция ускоряет нагрев.
    static constexpr int CONVECTION_FACTOR = 2;

    Oven(const std::string& model, int powerWatts, int trays, bool hasConvection);

    int getTrays() const { return _trays; }
    bool hasConvection() const { return _hasConvection; }

    std::string type() const override;
    /// Сколько минут нужно, чтобы разогреться до заданной температуры.
    int preheatMinutes(int targetTemperature) const;
    /// Запечь блюдо по рецепту. Духовка должна быть разогрета, противней — не больше, чем есть.
    std::string bake(const Recipe& recipe, int traysUsed);

private:
    int _trays;
    bool _hasConvection;
};
