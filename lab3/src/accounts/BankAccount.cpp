#include "accounts/BankAccount.h"

#include "exceptions/AtmException.h"

BankAccount::BankAccount(const std::string& number, const std::string& ownerName, const Currency& currency,
                         double initialBalance)
    : _number(number),
      _ownerName(ownerName),
      _currency(currency),
      _balance(initialBalance),
      _isFrozen(false),
      _statement(number) {
    if (number.empty() || ownerName.empty()) {
        throw AccountNotFoundException("У счёта должны быть номер и владелец");
    }
    if (initialBalance < 0) {
        throw InvalidAmountException("Начальный баланс не может быть отрицательным");
    }
}

std::string BankAccount::type() const {
    return "Текущий счёт";
}

double BankAccount::availableFunds() const {
    return _balance;
}

bool BankAccount::canWithdraw(double amount) const {
    return !_isFrozen && amount > 0 && amount <= availableFunds();
}

void BankAccount::deposit(double amount, const std::string& description) {
    checkOperation(amount);
    addToBalance(amount, description);
}

void BankAccount::withdraw(double amount, const std::string& description) {
    checkOperation(amount);
    if (amount > availableFunds()) {
        throw InsufficientFundsException("Недостаточно средств на счёте " + _number);
    }
    _balance -= amount;
    _statement.addEntry(description, -amount, _balance);
}

void BankAccount::transferTo(BankAccount& target, double amount) {
    if (!_currency.isSame(target._currency)) {
        throw CurrencyException("Перевод между счетами в разных валютах делает банк с конвертацией");
    }
    target.checkOperation(amount);
    withdraw(amount, "Перевод на счёт " + target._number);
    target.deposit(amount, "Перевод со счёта " + _number);
}

void BankAccount::freeze() {
    _isFrozen = true;
}

void BankAccount::unfreeze() {
    _isFrozen = false;
}

void BankAccount::addToBalance(double amount, const std::string& description) {
    _balance += amount;
    _statement.addEntry(description, amount, _balance);
}

void BankAccount::checkOperation(double amount) const {
    if (amount <= 0) {
        throw InvalidAmountException("Сумма операции должна быть больше нуля");
    }
    if (_isFrozen) {
        throw AccountFrozenException("Счёт " + _number + " заморожен");
    }
}
