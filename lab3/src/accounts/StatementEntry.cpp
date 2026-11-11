#include "accounts/StatementEntry.h"

#include "Format.h"

StatementEntry::StatementEntry(int number, const std::string& description, double amount, double balanceAfter)
    : _number(number), _description(description), _amount(amount), _balanceAfter(balanceAfter) {}

bool StatementEntry::isIncome() const {
    return _amount > 0;
}

bool StatementEntry::isExpense() const {
    return _amount < 0;
}

std::string StatementEntry::format() const {
    std::string sign = isIncome() ? "+" : "";
    return "#" + std::to_string(_number) + " " + _description + ": " + sign + format::number(_amount) +
           " (остаток " + format::number(_balanceAfter) + ")";
}
