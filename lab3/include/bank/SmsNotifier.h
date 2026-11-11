#pragma once

#include <string>
#include <vector>

#include "cards/CardHolder.h"
#include "operations/Transaction.h"

/// @brief SMS-оповещения клиенту об операциях по карте.
class SmsNotifier {
public:
    /// Бросает InvalidAmountException, если цена SMS отрицательная.
    explicit SmsNotifier(double smsPrice);

    const std::vector<std::string>& getSentMessages() const { return _sentMessages; }
    bool isEnabled() const { return _isEnabled; }

    /// Отправить SMS об операции. Возвращает false, если оповещения выключены или телефон неверный.
    bool notify(const CardHolder& holder, const Transaction& transaction);
    /// Включить оповещения.
    void enable();
    /// Выключить оповещения.
    void disable();
    /// Сколько стоили все отправленные SMS.
    double totalCost() const;
    /// Сколько SMS отправлено на номер.
    int messagesTo(const std::string& phone) const;

private:
    std::vector<std::string> _sentMessages;
    double _smsPrice;
    bool _isEnabled;
};
