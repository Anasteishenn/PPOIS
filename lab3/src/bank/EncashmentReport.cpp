#include "bank/EncashmentReport.h"

#include "Format.h"
#include "exceptions/AtmException.h"

EncashmentReport::EncashmentReport(const std::string& atmId, const std::string& collectorName, int day)
    : _atmId(atmId), _collectorName(collectorName), _day(day), _loadedAmount(0.0), _collectedAmount(0.0) {}

void EncashmentReport::addLoaded(double amount) {
    if (amount < 0) {
        throw InvalidAmountException("Загруженная сумма не может быть отрицательной");
    }
    _loadedAmount += amount;
}

void EncashmentReport::addCollected(double amount) {
    if (amount < 0) {
        throw InvalidAmountException("Забранная сумма не может быть отрицательной");
    }
    _collectedAmount += amount;
}

double EncashmentReport::netChange() const {
    return _loadedAmount - _collectedAmount;
}

std::string EncashmentReport::summary() const {
    return "Инкассация банкомата " + _atmId + ", день " + std::to_string(_day) + ", инкассатор " + _collectorName +
           ": загружено " + format::number(_loadedAmount) + ", забрано " + format::number(_collectedAmount) + " BYN";
}
