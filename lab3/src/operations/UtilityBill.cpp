#include "operations/UtilityBill.h"

#include "exceptions/AtmException.h"

UtilityBill::UtilityBill(const ServiceProvider& provider, const std::string& accountNumber, double amountDue,
                         int dueDay)
    : _provider(provider), _accountNumber(accountNumber), _amountDue(amountDue), _dueDay(dueDay), _isPaid(false) {
    if (!provider.isValidAccount(accountNumber)) {
        throw AccountNotFoundException("Неверный лицевой счёт: " + accountNumber);
    }
    if (amountDue <= 0) {
        throw InvalidAmountException("Сумма счёта должна быть больше нуля");
    }
}

bool UtilityBill::isOverdue(int day) const {
    return !_isPaid && day > _dueDay;
}

double UtilityBill::penalty(int day) const {
    if (!isOverdue(day)) {
        return 0.0;
    }
    return _amountDue * PENALTY_PERCENT_PER_DAY / PERCENT * (day - _dueDay);
}

double UtilityBill::totalToPay(int day) const {
    return _amountDue + penalty(day);
}

void UtilityBill::markPaid(double amountPaid, int day) {
    if (_isPaid) {
        throw TransactionException("Счёт уже оплачен");
    }
    if (amountPaid < totalToPay(day)) {
        throw InvalidAmountException("Недостаточная сумма для оплаты счёта");
    }
    _isPaid = true;
}
