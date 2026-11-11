#include "bank/CashCollector.h"

CashCollector::CashCollector(const std::string& name, const std::string& employeeId, const std::string& accessCode)
    : Employee(name, employeeId, accessCode), _loadedTotal(0.0), _collectedTotal(0.0), _visits(0) {}

std::string CashCollector::role() const {
    return "Инкассатор";
}

int CashCollector::loadCassette(CashDispenser& dispenser, int denomination, int count) {
    requireOnDuty();
    dispenser.loadNotes(denomination, count);
    int loaded = denomination * count;
    _loadedTotal += loaded;
    return loaded;
}

int CashCollector::collectDeposits(DepositModule& depositModule) {
    requireOnDuty();
    int collected = 0;
    for (const Banknote& note : depositModule.collectAll()) {
        collected += note.getDenomination();
    }
    _collectedTotal += collected;
    return collected;
}

EncashmentReport CashCollector::encash(Atm& atm, const std::string& serviceCode, int day) {
    requireOnDuty();
    atm.enterServiceMode(serviceCode);
    EncashmentReport report(atm.getId(), getName(), day);
    int loaded = atm.getDispenser().refillAll();
    _loadedTotal += loaded;
    report.addLoaded(loaded);
    report.addCollected(collectDeposits(atm.getDepositModule()));
    ++_visits;
    return report;
}
