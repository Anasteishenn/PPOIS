#pragma once

#include <string>
#include <vector>

#include "bank/Atm.h"
#include "bank/Employee.h"
#include "bank/MaintenanceLog.h"

/// @brief Техник: находит неисправные устройства банкомата и чинит их, записывая ремонт в журнал.
class Technician : public Employee {
public:
    /// Бросает InvalidAmountException, если стоимость выезда отрицательная.
    Technician(const std::string& name, const std::string& employeeId, const std::string& accessCode,
               const std::string& specialization, double callOutFee);

    const std::string& getSpecialization() const { return _specialization; }
    int getRepairsDone() const { return _repairsDone; }
    double getCallOutFee() const { return _callOutFee; }

    std::string role() const override;
    /// Список неисправных устройств банкомата.
    std::vector<std::string> diagnose(const Atm& atm) const;
    /// Починить устройство и записать ремонт в журнал. Бросает DeviceException, если устройство исправно.
    void repair(AtmDevice& device, MaintenanceLog& log, int day);
    /// Починить всё неисправное в банкомате (нужен служебный код). Возвращает число ремонтов.
    int repairAll(Atm& atm, const std::string& serviceCode, MaintenanceLog& log, int day);

private:
    std::string _specialization;
    int _repairsDone;
    double _callOutFee;
};
