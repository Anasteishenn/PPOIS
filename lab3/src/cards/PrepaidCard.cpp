#include "cards/PrepaidCard.h"

#include "exceptions/AtmException.h"

PrepaidCard::PrepaidCard(const std::string& number, const std::string& holderName, int expiryMonth, int expiryYear,
                         const std::string& pin, std::shared_ptr<BankAccount> account, bool isRechargeable)
    : Card(number, holderName, expiryMonth, expiryYear, pin, std::move(account)),
      _isRechargeable(isRechargeable),
      _rechargeCount(0) {}

std::string PrepaidCard::type() const {
    return "Предоплаченная";
}

void PrepaidCard::recharge(double amount) {
    if (!_isRechargeable) {
        throw InvalidCardException("Эту предоплаченную карту нельзя пополнить");
    }
    topUp(amount, "Пополнение предоплаченной карты");
    ++_rechargeCount;
}

bool PrepaidCard::isEmpty() const {
    return availableFunds() <= 0;
}
