#pragma once

#include <string>

#include "operations/ServiceProvider.h"

/// @brief Тарифы банкомата: комиссия за снятие (своя/чужая карта), за перевод и за оплату услуг.
class FeePolicy {
public:
    static constexpr double PERCENT = 100.0;

    /// Бросает InvalidAmountException, если какой-то тариф отрицательный.
    FeePolicy(double ownBankFee, double foreignPercent, double minForeignFee, double transferPercent);

    double getOwnBankFee() const { return _ownBankFee; }
    double getForeignPercent() const { return _foreignPercent; }
    double getMinForeignFee() const { return _minForeignFee; }
    double getTransferPercent() const { return _transferPercent; }

    /// Комиссия за снятие: фиксированная для своих карт, процент (не меньше минимума) — для чужих.
    double withdrawalFee(double amount, bool isOwnBank) const;
    /// Комиссия за перевод.
    double transferFee(double amount) const;
    /// Комиссия за оплату услуги — берёт поставщик.
    double paymentFee(double amount, const ServiceProvider& provider) const;
    /// Тарифы текстом для экрана.
    std::string describe() const;

private:
    double _ownBankFee;
    double _foreignPercent;
    double _minForeignFee;
    double _transferPercent;
};
