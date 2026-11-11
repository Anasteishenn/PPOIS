#pragma once

#include "cards/Card.h"

/// @brief Кредитная карта: сначала тратятся свои деньги, потом кредитные (появляется долг).
class CreditCard : public Card {
public:
    static constexpr double PERCENT = 100.0;
    /// Минимальный ежемесячный платёж, % от долга.
    static constexpr double MIN_PAYMENT_PERCENT = 5.0;

    /// Бросает InvalidAmountException, если кредитный лимит или ставка отрицательные.
    CreditCard(const std::string& number, const std::string& holderName, int expiryMonth, int expiryYear,
               const std::string& pin, std::shared_ptr<BankAccount> account, double creditLimit,
               double monthlyRatePercent);

    double getCreditLimit() const { return _creditLimit; }
    double getDebt() const { return _debt; }
    double getMonthlyRatePercent() const { return _monthlyRatePercent; }

    std::string type() const override;
    /// Свои деньги плюс неиспользованный кредит.
    double availableFunds() const override;
    /// Списать сначала свои деньги, остаток — в долг. Бросает InsufficientFundsException.
    void charge(double amount, const std::string& description) override;
    /// Погасить часть долга деньгами со счёта.
    void repayDebt(double amount);
    /// Проценты за месяц на текущий долг.
    double monthlyInterest() const;
    /// Начислить проценты за месяц (долг растёт).
    void accrueInterest();
    /// Минимальный обязательный платёж за месяц.
    double minimumPayment() const;

private:
    double _creditLimit;
    double _debt;
    double _monthlyRatePercent;
};
