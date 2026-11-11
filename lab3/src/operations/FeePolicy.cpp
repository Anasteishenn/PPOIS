#include "operations/FeePolicy.h"

#include <algorithm>

#include "Format.h"
#include "exceptions/AtmException.h"

FeePolicy::FeePolicy(double ownBankFee, double foreignPercent, double minForeignFee, double transferPercent)
    : _ownBankFee(ownBankFee),
      _foreignPercent(foreignPercent),
      _minForeignFee(minForeignFee),
      _transferPercent(transferPercent) {
    if (ownBankFee < 0 || foreignPercent < 0 || minForeignFee < 0 || transferPercent < 0) {
        throw InvalidAmountException("Тарифы не могут быть отрицательными");
    }
}

double FeePolicy::withdrawalFee(double amount, bool isOwnBank) const {
    if (isOwnBank) {
        return _ownBankFee;
    }
    return std::max(amount * _foreignPercent / PERCENT, _minForeignFee);
}

double FeePolicy::transferFee(double amount) const {
    return amount * _transferPercent / PERCENT;
}

double FeePolicy::paymentFee(double amount, const ServiceProvider& provider) const {
    return provider.commissionFor(amount);
}

std::string FeePolicy::describe() const {
    return "Снятие картой нашего банка: " + format::number(_ownBankFee) + " BYN; чужой картой: " +
           format::number(_foreignPercent) + "% (не меньше " + format::number(_minForeignFee) + " BYN); перевод: " +
           format::number(_transferPercent) + "%";
}
