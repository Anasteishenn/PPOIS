#pragma once

#include "cards/Card.h"

/// @brief Предоплаченная (подарочная) карта: тратится только внесённая сумма, пополнять можно не всегда.
class PrepaidCard : public Card {
public:
    PrepaidCard(const std::string& number, const std::string& holderName, int expiryMonth, int expiryYear,
                const std::string& pin, std::shared_ptr<BankAccount> account, bool isRechargeable);

    bool isRechargeable() const { return _isRechargeable; }
    int getRechargeCount() const { return _rechargeCount; }

    std::string type() const override;
    /// Пополнить карту. Бросает InvalidCardException, если карта не пополняемая.
    void recharge(double amount);
    /// Закончились ли деньги на карте.
    bool isEmpty() const;

private:
    bool _isRechargeable;
    int _rechargeCount;
};
