#include "cards/CreditCard.h"

#include <algorithm>

#include "exceptions/AtmException.h"

CreditCard::CreditCard(const std::string& number, const std::string& holderName, int expiryMonth, int expiryYear,
                       const std::string& pin, std::shared_ptr<BankAccount> account, double creditLimit,
                       double monthlyRatePercent)
    : Card(number, holderName, expiryMonth, expiryYear, pin, std::move(account)),
      _creditLimit(creditLimit),
      _debt(0.0),
      _monthlyRatePercent(monthlyRatePercent) {
    if (creditLimit < 0 || monthlyRatePercent < 0) {
        throw InvalidAmountException("Кредитный лимит и ставка не могут быть отрицательными");
    }
}

std::string CreditCard::type() const {
    return "Кредитная";
}

double CreditCard::availableFunds() const {
    return account().availableFunds() + (_creditLimit - _debt);
}

void CreditCard::charge(double amount, const std::string& description) {
    if (amount <= 0) {
        throw InvalidAmountException("Сумма операции должна быть больше нуля");
    }
    if (amount > availableFunds()) {
        throw InsufficientFundsException("Превышен кредитный лимит карты " + maskedNumber());
    }
    double ownMoney = std::min(amount, account().availableFunds());
    if (ownMoney > 0) {
        account().withdraw(ownMoney, description);
    }
    _debt += amount - ownMoney;
}

void CreditCard::repayDebt(double amount) {
    if (amount <= 0 || amount > _debt) {
        throw InvalidAmountException("Сумма погашения должна быть больше нуля и не больше долга");
    }
    account().withdraw(amount, "Погашение кредита");
    _debt -= amount;
}

double CreditCard::monthlyInterest() const {
    return _debt * _monthlyRatePercent / PERCENT;
}

void CreditCard::accrueInterest() {
    _debt += monthlyInterest();
}

double CreditCard::minimumPayment() const {
    return _debt * MIN_PAYMENT_PERCENT / PERCENT;
}
