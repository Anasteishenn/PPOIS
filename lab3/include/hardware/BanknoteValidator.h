#pragma once

#include <string>

#include "hardware/Banknote.h"

/// @brief Детектор купюр: проверяет подлинность, номинал, валюту и состояние купюры.
class BanknoteValidator {
public:
    static constexpr double PERCENT = 100.0;

    explicit BanknoteValidator(const std::string& currencyCode);

    const std::string& getCurrencyCode() const { return _currencyCode; }
    int getAcceptedCount() const { return _acceptedCount; }
    int getRejectedCount() const { return _rejectedCount; }

    /// Почему купюра не принимается (пустая строка — купюра в порядке).
    std::string rejectionReason(const Banknote& note) const;
    /// Проверить купюру и учесть результат. Возвращает true, если купюра принята.
    bool inspect(const Banknote& note);
    /// Проверить купюру. Бросает InvalidBanknoteException с причиной отказа.
    void check(const Banknote& note);
    /// Какой процент купюр отклонён.
    double rejectionRate() const;

private:
    std::string _currencyCode;
    int _acceptedCount;
    int _rejectedCount;
};
