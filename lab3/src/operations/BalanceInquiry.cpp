#include "operations/BalanceInquiry.h"

#include "Format.h"

BalanceInquiry::BalanceInquiry(int id, const std::string& cardMask, double balance, const std::string& currencyCode)
    : Transaction(id, cardMask, 0.0, 0.0), _balance(balance), _currencyCode(currencyCode) {}

std::string BalanceInquiry::type() const {
    return "Запрос баланса";
}

std::string BalanceInquiry::describe() const {
    return type() + ": доступно " + format::number(_balance) + " " + _currencyCode;
}

bool BalanceInquiry::isEnoughFor(double amount) const {
    return amount <= _balance;
}
