#include "bank/MaintenanceRecord.h"

#include "Format.h"
#include "exceptions/AtmException.h"

MaintenanceRecord::MaintenanceRecord(int day, const std::string& deviceName, const std::string& description,
                                     const std::string& technicianName, double cost)
    : _day(day), _deviceName(deviceName), _description(description), _technicianName(technicianName), _cost(cost) {
    if (day < 0 || cost < 0) {
        throw InvalidAmountException("День и стоимость ремонта не могут быть отрицательными");
    }
}

std::string MaintenanceRecord::summary() const {
    return "День " + std::to_string(_day) + ": " + _deviceName + " — " + _description + " (" + _technicianName +
           ", " + format::number(_cost) + " BYN)";
}

bool MaintenanceRecord::isExpensive() const {
    return _cost >= EXPENSIVE_COST;
}

int MaintenanceRecord::daysSince(int today) const {
    return today - _day;
}
