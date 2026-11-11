#pragma once

#include "bank/Atm.h"
#include "bank/EncashmentReport.h"
#include "bank/Employee.h"

/// @brief Инкассатор: загружает наличные в банкомат и забирает деньги из купюроприёмника.
class CashCollector : public Employee {
public:
    CashCollector(const std::string& name, const std::string& employeeId, const std::string& accessCode);

    double getLoadedTotal() const { return _loadedTotal; }
    double getCollectedTotal() const { return _collectedTotal; }
    int getVisits() const { return _visits; }

    std::string role() const override;
    /// Загрузить купюры в кассету диспенсера. Возвращает загруженную сумму.
    int loadCassette(CashDispenser& dispenser, int denomination, int count);
    /// Забрать все купюры из купюроприёмника. Возвращает забранную сумму.
    int collectDeposits(DepositModule& depositModule);
    /// Полная инкассация банкомата: кассеты доверху, купюроприёмник пустой. Возвращает акт.
    EncashmentReport encash(Atm& atm, const std::string& serviceCode, int day);

private:
    double _loadedTotal;
    double _collectedTotal;
    int _visits;
};
