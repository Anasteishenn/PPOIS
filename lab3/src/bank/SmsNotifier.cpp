#include "bank/SmsNotifier.h"

#include "exceptions/AtmException.h"

SmsNotifier::SmsNotifier(double smsPrice) : _smsPrice(smsPrice), _isEnabled(true) {
    if (smsPrice < 0) {
        throw InvalidAmountException("Цена SMS не может быть отрицательной");
    }
}

bool SmsNotifier::notify(const CardHolder& holder, const Transaction& transaction) {
    if (!_isEnabled || !holder.hasValidPhone()) {
        return false;
    }
    _sentMessages.push_back(holder.getPhone() + ": " + transaction.describe() + ", карта " +
                            transaction.getCardMask());
    return true;
}

void SmsNotifier::enable() {
    _isEnabled = true;
}

void SmsNotifier::disable() {
    _isEnabled = false;
}

double SmsNotifier::totalCost() const {
    return _sentMessages.size() * _smsPrice;
}

int SmsNotifier::messagesTo(const std::string& phone) const {
    int count = 0;
    for (const std::string& message : _sentMessages) {
        if (message.rfind(phone, 0) == 0) {
            ++count;
        }
    }
    return count;
}
