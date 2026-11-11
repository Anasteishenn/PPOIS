#pragma once

#include <string>

#include "operations/ServiceProvider.h"

/// @brief Счёт за коммунальные услуги: сколько платить, до какого дня, пеня за просрочку.
class UtilityBill {
public:
    static constexpr double PERCENT = 100.0;
    /// Пеня за каждый день просрочки, % от суммы.
    static constexpr double PENALTY_PERCENT_PER_DAY = 0.1;

    /// Бросает AccountNotFoundException для неверного лицевого счёта, InvalidAmountException — для суммы <= 0.
    UtilityBill(const ServiceProvider& provider, const std::string& accountNumber, double amountDue, int dueDay);

    const ServiceProvider& getProvider() const { return _provider; }
    const std::string& getAccountNumber() const { return _accountNumber; }
    double getAmountDue() const { return _amountDue; }
    int getDueDay() const { return _dueDay; }
    bool isPaid() const { return _isPaid; }

    /// Просрочен ли счёт на заданный день.
    bool isOverdue(int day) const;
    /// Пеня за просрочку на заданный день.
    double penalty(int day) const;
    /// Сколько всего платить на заданный день (сумма + пеня).
    double totalToPay(int day) const;
    /// Отметить оплату. Бросает TransactionException (уже оплачен) или InvalidAmountException (мало денег).
    void markPaid(double amountPaid, int day);

private:
    ServiceProvider _provider;
    std::string _accountNumber;
    double _amountDue;
    int _dueDay;
    bool _isPaid;
};
