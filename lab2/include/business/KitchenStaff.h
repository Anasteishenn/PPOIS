#pragma once

#include <string>

/// @brief Абстрактный сотрудник кухни: имя, стаж, почасовая ставка, отработанные часы.
class KitchenStaff {
public:
    /// С такого стажа (лет) сотрудник считается опытным.
    static constexpr int SENIOR_YEARS = 5;
    /// Максимальная длина одной смены, часов.
    static constexpr double MAX_SHIFT_HOURS = 12.0;

    KitchenStaff(const std::string& name, int experienceYears, double hourlyRate);
    virtual ~KitchenStaff() = default;

    const std::string& getName() const { return _name; }
    int getExperienceYears() const { return _experienceYears; }
    double getHoursWorked() const { return _hoursWorked; }

    /// Должность («Повар», «Шеф-повар»). Каждый наследник задаёт свою.
    virtual std::string role() const = 0;

    /// Записать отработанную смену. Бросает StaffException, если часов <= 0 или больше MAX_SHIFT_HOURS.
    void logHours(double hours);
    /// Зарплата за отработанные часы.
    double salary() const;
    /// Опытный ли сотрудник.
    bool isSenior() const;
    /// Бейдж: «Повар Иван, стаж 3 г.».
    std::string badge() const;

private:
    std::string _name;
    int _experienceYears;
    double _hourlyRate;
    double _hoursWorked;
};
