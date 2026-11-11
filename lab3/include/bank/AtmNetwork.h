#pragma once

#include <memory>
#include <string>
#include <vector>

#include "bank/Atm.h"

/// @brief Сеть банкоматов банка: поиск по городу, контроль наличных, сколько банкоматов работает.
class AtmNetwork {
public:
    /// Бросает InvalidAmountException, если минимальный остаток наличных отрицательный.
    AtmNetwork(const std::string& bankName, int minCashLevel);

    const std::string& getBankName() const { return _bankName; }
    const std::vector<std::shared_ptr<Atm>>& getAtms() const { return _atms; }

    /// Добавить банкомат. Бросает DeviceException, если номер уже занят.
    void addAtm(std::shared_ptr<Atm> atm);
    /// Найти банкомат по номеру. Бросает DeviceException.
    std::shared_ptr<Atm> findById(const std::string& id) const;
    /// Банкоматы в городе.
    std::vector<std::shared_ptr<Atm>> atmsInCity(const std::string& city) const;
    /// Банкоматы, где наличных меньше минимума (нужна инкассация).
    std::vector<std::shared_ptr<Atm>> needingEncashment() const;
    /// Сколько наличных во всей сети.
    int totalCash() const;
    /// Сколько банкоматов исправны и могут выдавать деньги.
    int operationalCount() const;

private:
    std::string _bankName;
    std::vector<std::shared_ptr<Atm>> _atms;
    int _minCashLevel;
};
