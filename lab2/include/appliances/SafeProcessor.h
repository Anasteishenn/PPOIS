#pragma once

#include "appliances/KitchenAppliance.h"

/// @brief Абстрактный прибор с ножами и защитной крышкой (блендер, комбайн, мясорубка).
/// Работает, только если крышка закрыта и загрузка не превышает максимум.
class SafeProcessor : public KitchenAppliance {
public:
    SafeProcessor(const std::string& model, int powerWatts, double maxLoadGrams);

    bool isLidClosed() const { return _lidClosed; }
    double getMaxLoadGrams() const { return _maxLoadGrams; }

    /// Закрыть крышку.
    void closeLid();
    /// Открыть крышку.
    void openLid();
    /// Проверить, что можно работать: прибор включён, крышка закрыта, загрузка в норме.
    void checkReady(double loadGrams) const;
    /// Проверить готовность и поработать заданное число минут.
    void safeRun(double loadGrams, int minutes);

private:
    bool _lidClosed;
    double _maxLoadGrams;
};
