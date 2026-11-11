#include "hardware/DepositModule.h"

#include "exceptions/AtmException.h"

DepositModule::DepositModule(const std::string& serialNumber, const BanknoteValidator& validator, int capacity)
    : AtmDevice(serialNumber), _validator(validator), _capacity(capacity), _lastRejectedCount(0) {}

std::string DepositModule::deviceName() const {
    return "Купюроприёмник";
}

int DepositModule::acceptNotes(const std::vector<Banknote>& notes) {
    ensureWorking();
    if (notes.empty()) {
        throw InvalidAmountException("Купюры не внесены");
    }
    if (static_cast<int>(notes.size()) > freeSpace()) {
        throw DeviceException("Лоток купюроприёмника переполнен");
    }
    int acceptedSum = 0;
    _lastRejectedCount = 0;
    for (const Banknote& note : notes) {
        if (_validator.inspect(note)) {
            _storedNotes.push_back(note);
            acceptedSum += note.getDenomination();
        } else {
            ++_lastRejectedCount;
        }
    }
    return acceptedSum;
}

int DepositModule::freeSpace() const {
    return _capacity - static_cast<int>(_storedNotes.size());
}

bool DepositModule::isFull() const {
    return freeSpace() <= 0;
}

int DepositModule::storedValue() const {
    int sum = 0;
    for (const Banknote& note : _storedNotes) {
        sum += note.getDenomination();
    }
    return sum;
}

std::vector<Banknote> DepositModule::collectAll() {
    std::vector<Banknote> notes = _storedNotes;
    _storedNotes.clear();
    return notes;
}
