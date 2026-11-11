#pragma once

#include "cards/CashbackProgram.h"
#include "cards/Card.h"

/// @brief Дебетовая карта: тратятся только свои деньги, за покупки начисляется кэшбэк.
class DebitCard : public Card {
public:
    DebitCard(const std::string& number, const std::string& holderName, int expiryMonth, int expiryYear,
              const std::string& pin, std::shared_ptr<BankAccount> account, const CashbackProgram& cashback);

    const CashbackProgram& getCashbackProgram() const { return _cashbackProgram; }
    double getCashbackEarned() const { return _cashbackEarned; }

    std::string type() const override;
    /// Начислить кэшбэк за покупку. Возвращает начисленную сумму.
    double earnCashback(double purchaseAmount, const std::string& category);
    /// Перевести накопленный кэшбэк на счёт карты. Возвращает переведённую сумму.
    double transferCashbackToAccount();

private:
    CashbackProgram _cashbackProgram;
    double _cashbackEarned;
};
