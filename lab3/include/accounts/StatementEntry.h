#pragma once

#include <string>

/// @brief Одна строка выписки по счёту: номер операции, описание, сумма, остаток после неё.
class StatementEntry {
public:
    StatementEntry(int number, const std::string& description, double amount, double balanceAfter);

    int getNumber() const { return _number; }
    const std::string& getDescription() const { return _description; }
    double getAmount() const { return _amount; }
    double getBalanceAfter() const { return _balanceAfter; }

    /// Поступление ли это (сумма > 0).
    bool isIncome() const;
    /// Списание ли это (сумма < 0).
    bool isExpense() const;
    /// Строка для печати: «#3 Снятие наличных: -100.00 (остаток 400.00)».
    std::string format() const;

private:
    int _number;
    std::string _description;
    double _amount;
    double _balanceAfter;
};
