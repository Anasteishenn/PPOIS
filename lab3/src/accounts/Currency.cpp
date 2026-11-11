#include "accounts/Currency.h"

#include "Format.h"
#include "exceptions/AtmException.h"

Currency::Currency(const std::string& code, const std::string& symbol, double rateToByn)
    : _code(code), _symbol(symbol), _rateToByn(rateToByn) {
    if (code.size() != CODE_LENGTH) {
        throw CurrencyException("Код валюты должен состоять из 3 букв");
    }
    if (rateToByn <= 0) {
        throw CurrencyException("Курс валюты должен быть больше нуля");
    }
}

double Currency::toByn(double amount) const {
    return amount * _rateToByn;
}

double Currency::fromByn(double amountByn) const {
    return amountByn / _rateToByn;
}

std::string Currency::format(double amount) const {
    return format::number(amount) + " " + _symbol;
}

void Currency::updateRate(double newRate) {
    if (newRate <= 0) {
        throw CurrencyException("Курс валюты должен быть больше нуля");
    }
    _rateToByn = newRate;
}

bool Currency::isSame(const Currency& other) const {
    return _code == other._code;
}
