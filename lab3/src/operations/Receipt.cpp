#include "operations/Receipt.h"

#include "Format.h"

Receipt::Receipt(const std::string& atmId, int transactionId, const std::string& cardMask)
    : _atmId(atmId), _transactionId(transactionId), _cardMask(cardMask) {}

void Receipt::addLine(const std::string& line) {
    _lines.push_back(line);
}

void Receipt::addTransaction(const Transaction& transaction) {
    addLine(transaction.describe());
    addLine("Итого списано: " + format::number(transaction.total()) + " BYN");
    std::string status = "Статус: " + transaction.statusName();
    if (!transaction.getFailReason().empty()) {
        status += " (" + transaction.getFailReason() + ")";
    }
    addLine(status);
}

int Receipt::lineCount() const {
    return static_cast<int>(_lines.size());
}

std::string Receipt::text() const {
    std::string text = "===== ЧЕК =====\nБанкомат: " + _atmId + "\nОперация №" + std::to_string(_transactionId) +
                       "\nКарта: " + _cardMask + "\n";
    for (const std::string& line : _lines) {
        text += line + "\n";
    }
    return text + "Спасибо, что пользуетесь нашим банком!\n===============";
}
