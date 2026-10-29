#pragma once

#include <string>

/// @brief Абстрактный кухонный прибор: модель, мощность, включён ли, сколько минут отработал.
class KitchenAppliance {
public:
    /// После стольких минут работы прибору нужно техобслуживание (100 часов).
    static constexpr int MAINTENANCE_MINUTES = 6000;
    static constexpr double MINUTES_PER_HOUR = 60.0;
    static constexpr double WATTS_PER_KILOWATT = 1000.0;

    KitchenAppliance(const std::string& model, int powerWatts);
    virtual ~KitchenAppliance() = default;

    const std::string& getModel() const { return _model; }
    int getPowerWatts() const { return _powerWatts; }
    bool isOn() const { return _isOn; }
    int getUsageMinutes() const { return _usageMinutes; }

    /// Тип прибора («Духовка», «Блендер»...). Каждый наследник задаёт свой.
    virtual std::string type() const = 0;

    /// Включить прибор.
    void turnOn();
    /// Выключить прибор.
    void turnOff();
    /// Поработать заданное число минут. Бросает ApplianceException, если прибор выключен.
    void use(int minutes);
    /// Сколько электроэнергии (кВт·ч) прибор израсходовал за всё время.
    double energyUsedKwh() const;
    /// Пора ли на техобслуживание.
    bool needsMaintenance() const;
    /// Провести техобслуживание: счётчик минут обнуляется.
    void service();

private:
    std::string _model;
    int _powerWatts;
    bool _isOn;
    int _usageMinutes;
};
