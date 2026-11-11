#pragma once

#include <map>
#include <string>

#include "accounts/Currency.h"

/// @brief Обмен валют: таблица курсов и комиссия банка за конвертацию.
class CurrencyExchange {
public:
    static constexpr double PERCENT = 100.0;
    /// Максимальная комиссия за обмен, %.
    static constexpr double MAX_COMMISSION_PERCENT = 10.0;

    /// Бросает CurrencyException, если комиссия вне 0..10%.
    explicit CurrencyExchange(double commissionPercent);

    double getCommissionPercent() const { return _commissionPercent; }
    int getOperationsCount() const { return _operationsCount; }

    /// Добавить валюту в таблицу (или обновить её курс).
    void addCurrency(const Currency& currency);
    /// Есть ли такая валюта в таблице.
    bool supports(const std::string& code) const;
    /// Найти валюту по коду. Бросает CurrencyException, если её нет.
    const Currency& findCurrency(const std::string& code) const;
    /// Комиссия за обмен суммы (в исходной валюте).
    double commissionFor(double amount) const;
    /// Обменять сумму из одной валюты в другую с учётом комиссии.
    double convert(double amount, const std::string& fromCode, const std::string& toCode);

private:
    std::map<std::string, Currency> _currencies;
    double _commissionPercent;
    int _operationsCount;
};
