#pragma once

#include "appliances/KitchenAppliance.h"
#include "recipes/ThermalProcess.h"

/// @brief Абстрактный нагревательный прибор: умеет нагреваться до температуры не выше максимальной.
class ThermalAppliance : public KitchenAppliance {
public:
    /// Температура выключенного (остывшего) прибора.
    static constexpr int ROOM_TEMPERATURE = 20;

    ThermalAppliance(const std::string& model, int powerWatts, int maxTemperature);

    int getCurrentTemperature() const { return _currentTemperature; }
    int getMaxTemperature() const { return _maxTemperature; }

    /// Нагреть до температуры. Прибор должен быть включён, температура — не выше максимальной.
    void heatTo(int temperature);
    /// Остудить до комнатной температуры.
    void coolDown();
    /// Можно ли выполнить тепловой процесс на этом приборе (хватит ли температуры).
    bool canPerform(const ThermalProcess& process) const;

private:
    int _currentTemperature;
    int _maxTemperature;
};
