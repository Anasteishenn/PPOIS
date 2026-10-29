#pragma once

#include "appliances/ThermalAppliance.h"

/// @brief Варочная плита с несколькими конфорками (газовая или электрическая).
class StoveTop : public ThermalAppliance {
public:
    static constexpr int MAX_TEMPERATURE = 300;

    StoveTop(const std::string& model, int powerWatts, int burners, bool isGas);

    int getBurners() const { return _burners; }
    bool isGas() const { return _isGas; }

    std::string type() const override;
    /// Занять свободную конфорку. Бросает ApplianceException, если плита выключена или все заняты.
    void occupyBurner();
    /// Освободить конфорку.
    void releaseBurner();
    /// Сколько конфорок свободно.
    int freeBurners() const;

private:
    int _burners;
    int _busyBurners;
    bool _isGas;
};
