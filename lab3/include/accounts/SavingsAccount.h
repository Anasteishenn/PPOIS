#pragma once

#include "accounts/BankAccount.h"

/// @brief Сберегательный счёт (вклад): проценты и неснижаемый остаток, который нельзя снять.
class SavingsAccount : public BankAccount {
public:
    static constexpr double PERCENT = 100.0;
    static constexpr int MONTHS_IN_YEAR = 12;

    /// Бросает InvalidAmountException, если ставка или неснижаемый остаток отрицательные.
    SavingsAccount(const std::string& number, const std::string& ownerName, const Currency& currency,
                   double initialBalance, double yearRatePercent, double minBalance);

    double getYearRatePercent() const { return _yearRatePercent; }
    double getMinBalance() const { return _minBalance; }

    std::string type() const override;
    /// Снять можно всё, кроме неснижаемого остатка.
    double availableFunds() const override;
    /// Проценты, которые набегут за заданное число месяцев (без капитализации).
    double interestForMonths(int months) const;
    /// Начислить проценты за месяц. Возвращает начисленную сумму.
    double accrueMonthlyInterest();

private:
    double _yearRatePercent;
    double _minBalance;
};
