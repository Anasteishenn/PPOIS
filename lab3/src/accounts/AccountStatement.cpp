#include "accounts/AccountStatement.h"

#include "exceptions/AtmException.h"

AccountStatement::AccountStatement(const std::string& accountNumber, int maxEntries)
    : _accountNumber(accountNumber), _maxEntries(maxEntries), _totalAdded(0) {
    if (maxEntries <= 0) {
        throw InvalidAmountException("Выписка должна хранить хотя бы одну строку");
    }
}

void AccountStatement::addEntry(const std::string& description, double amount, double balanceAfter) {
    ++_totalAdded;
    _entries.emplace_back(_totalAdded, description, amount, balanceAfter);
    if (static_cast<int>(_entries.size()) > _maxEntries) {
        _entries.erase(_entries.begin());
    }
}

std::vector<StatementEntry> AccountStatement::lastEntries(int count) const {
    if (count <= 0) {
        return {};
    }
    std::size_t start = _entries.size() > static_cast<std::size_t>(count) ? _entries.size() - count : 0;
    return std::vector<StatementEntry>(_entries.begin() + start, _entries.end());
}

double AccountStatement::totalIncome() const {
    double sum = 0.0;
    for (const StatementEntry& entry : _entries) {
        if (entry.isIncome()) {
            sum += entry.getAmount();
        }
    }
    return sum;
}

double AccountStatement::totalExpense() const {
    double sum = 0.0;
    for (const StatementEntry& entry : _entries) {
        if (entry.isExpense()) {
            sum -= entry.getAmount();
        }
    }
    return sum;
}
