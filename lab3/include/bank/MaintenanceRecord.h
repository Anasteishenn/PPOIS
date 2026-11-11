#pragma once

#include <string>

/// @brief Запись о ремонте или осмотре: день, устройство, что сделано, кто сделал, стоимость.
class MaintenanceRecord {
public:
    /// Ремонт считается дорогим, если стоит не меньше 100 BYN.
    static constexpr double EXPENSIVE_COST = 100.0;

    /// Бросает InvalidAmountException, если день или стоимость отрицательные.
    MaintenanceRecord(int day, const std::string& deviceName, const std::string& description,
                      const std::string& technicianName, double cost);

    int getDay() const { return _day; }
    const std::string& getDeviceName() const { return _deviceName; }
    const std::string& getDescription() const { return _description; }
    const std::string& getTechnicianName() const { return _technicianName; }
    double getCost() const { return _cost; }

    /// Запись одной строкой.
    std::string summary() const;
    /// Дорогой ли ремонт.
    bool isExpensive() const;
    /// Сколько дней прошло с ремонта.
    int daysSince(int today) const;

private:
    int _day;
    std::string _deviceName;
    std::string _description;
    std::string _technicianName;
    double _cost;
};
