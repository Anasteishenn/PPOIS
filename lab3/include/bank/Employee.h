#pragma once

#include <string>

/// @brief Абстрактный сотрудник банка, обслуживающий банкоматы: имя, табельный номер, код доступа, смена.
class Employee {
public:
    /// Бросает AccessDeniedException, если имя, табельный номер или код доступа пустые.
    Employee(const std::string& name, const std::string& employeeId, const std::string& accessCode);
    virtual ~Employee() = default;

    const std::string& getName() const { return _name; }
    const std::string& getEmployeeId() const { return _employeeId; }
    bool isOnDuty() const { return _isOnDuty; }

    /// Должность словами.
    virtual std::string role() const = 0;
    /// Проверить код доступа. Бросает AccessDeniedException.
    void verifyAccess(const std::string& accessCode) const;
    /// Начать смену (нужен код доступа).
    void startShift(const std::string& accessCode);
    /// Закончить смену.
    void endShift();
    /// Бросает AccessDeniedException, если сотрудник не на смене.
    void requireOnDuty() const;
    /// Бейдж: «Инкассатор Иван Петров (таб. № C-01)».
    std::string badge() const;

private:
    std::string _name;
    std::string _employeeId;
    std::string _accessCode;
    bool _isOnDuty;
};
