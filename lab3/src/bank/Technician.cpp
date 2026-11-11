#include "bank/Technician.h"

#include "exceptions/AtmException.h"

Technician::Technician(const std::string& name, const std::string& employeeId, const std::string& accessCode,
                       const std::string& specialization, double callOutFee)
    : Employee(name, employeeId, accessCode),
      _specialization(specialization),
      _repairsDone(0),
      _callOutFee(callOutFee) {
    if (callOutFee < 0) {
        throw InvalidAmountException("Стоимость выезда не может быть отрицательной");
    }
}

std::string Technician::role() const {
    return "Техник";
}

std::vector<std::string> Technician::diagnose(const Atm& atm) const {
    std::vector<std::string> broken;
    for (const AtmDevice* device : atm.devices()) {
        if (!device->selfTest()) {
            broken.push_back(device->statusLine());
        }
    }
    return broken;
}

void Technician::repair(AtmDevice& device, MaintenanceLog& log, int day) {
    requireOnDuty();
    if (device.isWorking()) {
        throw DeviceException(device.deviceName() + " исправно, ремонт не нужен");
    }
    std::string problem = device.getLastError();
    device.service();
    ++_repairsDone;
    log.addRecord(MaintenanceRecord(day, device.deviceName(), "Ремонт: " + problem, getName(), _callOutFee));
}

int Technician::repairAll(Atm& atm, const std::string& serviceCode, MaintenanceLog& log, int day) {
    requireOnDuty();
    atm.enterServiceMode(serviceCode);
    int repaired = 0;
    for (AtmDevice* device : atm.devices()) {
        if (!device->isWorking()) {
            repair(*device, log, day);
            ++repaired;
        }
    }
    log.addRecord(MaintenanceRecord(day, "Банкомат " + atm.getId(), "Плановый осмотр", getName(), 0.0));
    return repaired;
}
