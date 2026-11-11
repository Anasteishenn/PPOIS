#pragma once

#include <string>
#include <vector>

#include "accounts/StatementEntry.h"

/// @brief Выписка по счёту: хранит последние операции (старые удаляются при переполнении).
class AccountStatement {
public:
    /// Сколько строк хранит выписка по умолчанию.
    static constexpr int DEFAULT_MAX_ENTRIES = 20;

    explicit AccountStatement(const std::string& accountNumber, int maxEntries = DEFAULT_MAX_ENTRIES);

    const std::string& getAccountNumber() const { return _accountNumber; }
    const std::vector<StatementEntry>& getEntries() const { return _entries; }
    int getTotalAdded() const { return _totalAdded; }

    /// Добавить строку. Если места нет — самая старая строка удаляется.
    void addEntry(const std::string& description, double amount, double balanceAfter);
    /// Последние count строк (мини-выписка в банкомате).
    std::vector<StatementEntry> lastEntries(int count) const;
    /// Сумма всех поступлений в выписке.
    double totalIncome() const;
    /// Сумма всех списаний в выписке (положительное число).
    double totalExpense() const;

private:
    std::string _accountNumber;
    std::vector<StatementEntry> _entries;
    int _maxEntries;
    int _totalAdded;
};
