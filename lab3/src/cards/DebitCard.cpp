#include "cards/DebitCard.h"

DebitCard::DebitCard(const std::string& number, const std::string& holderName, int expiryMonth, int expiryYear,
                     const std::string& pin, std::shared_ptr<BankAccount> account, const CashbackProgram& cashback)
    : Card(number, holderName, expiryMonth, expiryYear, pin, std::move(account)),
      _cashbackProgram(cashback),
      _cashbackEarned(0.0) {}

std::string DebitCard::type() const {
    return "Дебетовая";
}

double DebitCard::earnCashback(double purchaseAmount, const std::string& category) {
    double cashback = _cashbackProgram.calculate(purchaseAmount, category, _cashbackEarned);
    _cashbackEarned += cashback;
    return cashback;
}

double DebitCard::transferCashbackToAccount() {
    double amount = _cashbackEarned;
    if (amount > 0) {
        topUp(amount, "Кэшбэк");
        _cashbackEarned = 0.0;
    }
    return amount;
}
