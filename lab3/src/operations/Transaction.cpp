#include "operations/Transaction.h"

#include "Format.h"
#include "exceptions/AtmException.h"

Transaction::Transaction(int id, const std::string& cardMask, double amount, double fee)
    : _id(id), _cardMask(cardMask), _amount(amount), _fee(fee), _status(TransactionStatus::Pending) {
    if (amount < 0 || fee < 0) {
        throw InvalidAmountException("Сумма и комиссия операции не могут быть отрицательными");
    }
}

std::string Transaction::describe() const {
    std::string text = type() + ": " + format::number(_amount) + " BYN";
    if (_fee > 0) {
        text += " (комиссия " + format::number(_fee) + ")";
    }
    return text;
}

double Transaction::total() const {
    return _amount + _fee;
}

void Transaction::complete() {
    ensurePending();
    _status = TransactionStatus::Completed;
}

void Transaction::fail(const std::string& reason) {
    ensurePending();
    _status = TransactionStatus::Failed;
    _failReason = reason;
}

bool Transaction::isSuccessful() const {
    return _status == TransactionStatus::Completed;
}

std::string Transaction::statusName() const {
    switch (_status) {
        case TransactionStatus::Completed:
            return "Успешно";
        case TransactionStatus::Failed:
            return "Отклонено";
        default:
            return "В обработке";
    }
}

void Transaction::ensurePending() const {
    if (_status != TransactionStatus::Pending) {
        throw TransactionException("Операция №" + std::to_string(_id) + " уже завершена");
    }
}
