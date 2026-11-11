#include "accounts/SavingsAccount.h"

#include "exceptions/AtmException.h"

SavingsAccount::SavingsAccount(const std::string& number, const std::string& ownerName, const Currency& currency,
                               double initialBalance, double yearRatePercent, double minBalance)
    : BankAccount(number, ownerName, currency, initialBalance),
      _yearRatePercent(yearRatePercent),
      _minBalance(minBalance) {
    if (yearRatePercent < 0 || minBalance < 0) {
        throw InvalidAmountException("Ставка и неснижаемый остаток не могут быть отрицательными");
    }
}

std::string SavingsAccount::type() const {
    return "Сберегательный счёт";
}

double SavingsAccount::availableFunds() const {
    double available = getBalance() - _minBalance;
    return available > 0 ? available : 0.0;
}

double SavingsAccount::interestForMonths(int months) const {
    if (months <= 0) {
        throw InvalidAmountException("Число месяцев должно быть больше нуля");
    }
    return getBalance() * _yearRatePercent / PERCENT / MONTHS_IN_YEAR * months;
}

double SavingsAccount::accrueMonthlyInterest() {
    double interest = interestForMonths(1);
    if (interest > 0) {
        addToBalance(interest, "Проценты по вкладу");
    }
    return interest;
}
