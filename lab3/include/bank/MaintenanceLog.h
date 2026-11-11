#pragma once

#include <string>
#include <vector>

#include "bank/MaintenanceRecord.h"

/// @brief Журнал обслуживания банкомата: все ремонты и когда пора на плановый осмотр.
class MaintenanceLog {
public:
    /// Бросает InvalidAmountException, если интервал обслуживания <= 0.
    MaintenanceLog(const std::string& atmId, int serviceIntervalDays);

    const std::string& getAtmId() const { return _atmId; }
    const std::vector<MaintenanceRecord>& getRecords() const { return _records; }
    int getLastServiceDay() const { return _lastServiceDay; }

    /// Добавить запись (день последнего обслуживания обновляется).
    void addRecord(const MaintenanceRecord& record);
    /// Пора ли на плановое обслуживание.
    bool isServiceDue(int today) const;
    /// Сколько всего потрачено на ремонты.
    double totalCost() const;
    /// Все записи по одному устройству.
    std::vector<MaintenanceRecord> recordsFor(const std::string& deviceName) const;

private:
    std::string _atmId;
    std::vector<MaintenanceRecord> _records;
    int _serviceIntervalDays;
    int _lastServiceDay;
};
