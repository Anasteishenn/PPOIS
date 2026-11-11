#include "operations/Transfer.h"

Transfer::Transfer(int id, const std::string& cardMask, double amount, double fee, const std::string& targetAccount,
                   const std::string& targetOwner)
    : Transaction(id, cardMask, amount, fee), _targetAccount(targetAccount), _targetOwner(targetOwner) {}

std::string Transfer::type() const {
    return "Перевод";
}

std::string Transfer::describe() const {
    return Transaction::describe() + " -> " + _targetOwner + " (счёт " + _targetAccount + ")";
}

bool Transfer::isToOwner(const std::string& ownerName) const {
    return _targetOwner == ownerName;
}
