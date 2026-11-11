#include "cards/CardLimit.h"

#include "Format.h"
#include "exceptions/AtmException.h"

CardLimit::CardLimit(double dailyLimit, int maxOperationsPerDay)
    : _dailyLimit(dailyLimit), _withdrawnToday(0.0), _maxOperationsPerDay(maxOperationsPerDay), _operationsToday(0) {
    if (dailyLimit <= 0 || maxOperationsPerDay <= 0) {
        throw InvalidAmountException("Лимит и число операций должны быть больше нуля");
    }
}

double CardLimit::remaining() const {
    return _dailyLimit - _withdrawnToday;
}

bool CardLimit::canWithdraw(double amount) const {
    return amount <= remaining() && _operationsToday < _maxOperationsPerDay;
}

void CardLimit::registerWithdrawal(double amount) {
    if (_operationsToday >= _maxOperationsPerDay) {
        throw LimitExceededException("Превышено число операций за день");
    }
    if (amount > remaining()) {
        throw LimitExceededException("Превышен дневной лимит. Можно снять ещё " + format::number(remaining()));
    }
    _withdrawnToday += amount;
    ++_operationsToday;
}

void CardLimit::resetDay() {
    _withdrawnToday = 0.0;
    _operationsToday = 0;
}

void CardLimit::changeLimit(double newLimit) {
    if (newLimit <= 0) {
        throw InvalidAmountException("Лимит должен быть больше нуля");
    }
    _dailyLimit = newLimit;
}
