#include "hardware/CashCassette.h"

#include "exceptions/AtmException.h"
#include "hardware/Banknote.h"

CashCassette::CashCassette(int denomination, int count, int capacity, const std::string& currencyCode)
    : _denomination(denomination), _count(count), _capacity(capacity), _currencyCode(currencyCode) {
    if (!Banknote::isValidDenomination(denomination)) {
        throw InvalidBanknoteException("Нет купюр номиналом " + std::to_string(denomination));
    }
    if (capacity <= 0 || count < 0 || count > capacity) {
        throw DeviceException("Неверная ёмкость или заполнение кассеты");
    }
}

void CashCassette::take(int count) {
    if (count <= 0) {
        throw InvalidAmountException("Число купюр должно быть больше нуля");
    }
    if (count > _count) {
        throw CashShortageException("В кассете " + std::to_string(_denomination) + " не хватает купюр");
    }
    _count -= count;
}

void CashCassette::load(int count) {
    if (count <= 0) {
        throw InvalidAmountException("Число купюр должно быть больше нуля");
    }
    if (_count + count > _capacity) {
        throw DeviceException("Купюры не помещаются в кассету");
    }
    _count += count;
}

int CashCassette::fillToCapacity() {
    int added = _capacity - _count;
    _count = _capacity;
    return added;
}

int CashCassette::totalValue() const {
    return _denomination * _count;
}

bool CashCassette::isEmpty() const {
    return _count == 0;
}

bool CashCassette::isLow() const {
    return fillPercent() < LOW_LEVEL_PERCENT;
}

double CashCassette::fillPercent() const {
    return _count * PERCENT / _capacity;
}
