#include "bank/FraudDetector.h"

#include "Format.h"
#include "exceptions/AtmException.h"

FraudDetector::FraudDetector(double largeAmountThreshold, int windowSize)
    : _largeAmountThreshold(largeAmountThreshold), _windowSize(windowSize) {
    if (largeAmountThreshold <= 0 || windowSize <= 0) {
        throw InvalidAmountException("Порог и размер окна антифрода должны быть больше нуля");
    }
}

bool FraudDetector::isSuspicious(double amount) const {
    bool isLarge = amount > _largeAmountThreshold;
    bool isSeries = recentTotal() + amount > _largeAmountThreshold * SERIES_MULTIPLIER;
    return isLarge || isSeries;
}

void FraudDetector::check(const std::string& cardMask, double amount) {
    if (isSuspicious(amount)) {
        _alerts.push_back("Карта " + cardMask + ": подозрительная операция на " + format::number(amount));
        throw FraudDetectedException("Операция заблокирована службой безопасности банка");
    }
    record(amount);
}

void FraudDetector::record(double amount) {
    _recentAmounts.push_back(amount);
    if (static_cast<int>(_recentAmounts.size()) > _windowSize) {
        _recentAmounts.pop_front();
    }
}

double FraudDetector::recentTotal() const {
    double total = 0.0;
    for (double amount : _recentAmounts) {
        total += amount;
    }
    return total;
}

void FraudDetector::reset() {
    _recentAmounts.clear();
}
