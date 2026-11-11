#include "bank/MaintenanceLog.h"

#include "exceptions/AtmException.h"

MaintenanceLog::MaintenanceLog(const std::string& atmId, int serviceIntervalDays)
    : _atmId(atmId), _serviceIntervalDays(serviceIntervalDays), _lastServiceDay(0) {
    if (serviceIntervalDays <= 0) {
        throw InvalidAmountException("Интервал обслуживания должен быть больше нуля");
    }
}

void MaintenanceLog::addRecord(const MaintenanceRecord& record) {
    _records.push_back(record);
    if (record.getDay() > _lastServiceDay) {
        _lastServiceDay = record.getDay();
    }
}

bool MaintenanceLog::isServiceDue(int today) const {
    return today - _lastServiceDay >= _serviceIntervalDays;
}

double MaintenanceLog::totalCost() const {
    double total = 0.0;
    for (const MaintenanceRecord& record : _records) {
        total += record.getCost();
    }
    return total;
}

std::vector<MaintenanceRecord> MaintenanceLog::recordsFor(const std::string& deviceName) const {
    std::vector<MaintenanceRecord> result;
    for (const MaintenanceRecord& record : _records) {
        if (record.getDeviceName() == deviceName) {
            result.push_back(record);
        }
    }
    return result;
}
