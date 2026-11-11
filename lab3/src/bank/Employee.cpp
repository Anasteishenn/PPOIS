#include "bank/Employee.h"

#include "exceptions/AtmException.h"

Employee::Employee(const std::string& name, const std::string& employeeId, const std::string& accessCode)
    : _name(name), _employeeId(employeeId), _accessCode(accessCode), _isOnDuty(false) {
    if (name.empty() || employeeId.empty() || accessCode.empty()) {
        throw AccessDeniedException("У сотрудника должны быть имя, табельный номер и код доступа");
    }
}

void Employee::verifyAccess(const std::string& accessCode) const {
    if (accessCode != _accessCode) {
        throw AccessDeniedException("Неверный код доступа сотрудника " + _name);
    }
}

void Employee::startShift(const std::string& accessCode) {
    verifyAccess(accessCode);
    _isOnDuty = true;
}

void Employee::endShift() {
    _isOnDuty = false;
}

void Employee::requireOnDuty() const {
    if (!_isOnDuty) {
        throw AccessDeniedException(_name + " не на смене");
    }
}

std::string Employee::badge() const {
    return role() + " " + _name + " (таб. № " + _employeeId + ")";
}
