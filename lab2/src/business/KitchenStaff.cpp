#include "business/KitchenStaff.h"

#include "exceptions/CulinaryException.h"

KitchenStaff::KitchenStaff(const std::string& name, int experienceYears, double hourlyRate)
    : _name(name), _experienceYears(experienceYears), _hourlyRate(hourlyRate), _hoursWorked(0.0) {
    if (name.empty() || experienceYears < 0 || hourlyRate < 0) {
        throw StaffException("Некорректные данные сотрудника");
    }
}

void KitchenStaff::logHours(double hours) {
    if (hours <= 0 || hours > MAX_SHIFT_HOURS) {
        throw StaffException("Смена должна длиться от 0 до 12 часов");
    }
    _hoursWorked += hours;
}

double KitchenStaff::salary() const {
    return _hoursWorked * _hourlyRate;
}

bool KitchenStaff::isSenior() const {
    return _experienceYears >= SENIOR_YEARS;
}

std::string KitchenStaff::badge() const {
    return role() + " " + _name + ", стаж " + std::to_string(_experienceYears) + " г.";
}
