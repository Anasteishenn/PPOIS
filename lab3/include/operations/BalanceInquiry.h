#pragma once

#include "operations/Transaction.h"

/// @brief Запрос баланса: сумма операции 0, запоминается показанный остаток.
class BalanceInquiry : public Transaction {
public:
    BalanceInquiry(int id, const std::string& cardMask, double balance, const std::string& currencyCode);

    double getBalance() const { return _balance; }
    const std::string& getCurrencyCode() const { return _currencyCode; }

    std::string type() const override;
    /// «Запрос баланса: доступно 500.00 BYN».
    std::string describe() const override;
    /// Хватает ли денег на сумму (подсказка клиенту перед снятием).
    bool isEnoughFor(double amount) const;

private:
    double _balance;
    std::string _currencyCode;
};
