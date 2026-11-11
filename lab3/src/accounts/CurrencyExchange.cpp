#include "accounts/CurrencyExchange.h"

#include "exceptions/AtmException.h"

CurrencyExchange::CurrencyExchange(double commissionPercent)
    : _commissionPercent(commissionPercent), _operationsCount(0) {
    if (commissionPercent < 0 || commissionPercent > MAX_COMMISSION_PERCENT) {
        throw CurrencyException("Комиссия за обмен должна быть от 0 до 10%");
    }
}

void CurrencyExchange::addCurrency(const Currency& currency) {
    _currencies.insert_or_assign(currency.getCode(), currency);
}

bool CurrencyExchange::supports(const std::string& code) const {
    return _currencies.count(code) > 0;
}

const Currency& CurrencyExchange::findCurrency(const std::string& code) const {
    auto found = _currencies.find(code);
    if (found == _currencies.end()) {
        throw CurrencyException("Неизвестная валюта: " + code);
    }
    return found->second;
}

double CurrencyExchange::commissionFor(double amount) const {
    return amount * _commissionPercent / PERCENT;
}

double CurrencyExchange::convert(double amount, const std::string& fromCode, const std::string& toCode) {
    if (amount <= 0) {
        throw InvalidAmountException("Сумма обмена должна быть больше нуля");
    }
    const Currency& from = findCurrency(fromCode);
    const Currency& to = findCurrency(toCode);
    if (from.isSame(to)) {
        return amount;
    }
    ++_operationsCount;
    double afterCommission = amount - commissionFor(amount);
    return to.fromByn(from.toByn(afterCommission));
}
