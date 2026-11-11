#include "hardware/CashDispenser.h"

#include <algorithm>

#include "exceptions/AtmException.h"

namespace {
/// Сколько всего купюр в раскладке.
int countNotes(const NotesPlan& plan) {
    int count = 0;
    for (const auto& [denomination, number] : plan) {
        count += number;
    }
    return count;
}
}  // namespace

CashDispenser::CashDispenser(const std::string& serialNumber, int maxNotesPerWithdrawal)
    : AtmDevice(serialNumber), _maxNotesPerWithdrawal(maxNotesPerWithdrawal), _dispensedTotal(0) {}

std::string CashDispenser::deviceName() const {
    return "Диспенсер наличных";
}

void CashDispenser::addCassette(const CashCassette& cassette) {
    for (const CashCassette& installed : _cassettes) {
        if (installed.getDenomination() == cassette.getDenomination()) {
            throw DeviceException("Кассета номиналом " + std::to_string(cassette.getDenomination()) + " уже есть");
        }
    }
    _cassettes.push_back(cassette);
    // Держим кассеты от крупных купюр к мелким — так проще выдавать деньги.
    std::sort(_cassettes.begin(), _cassettes.end(), [](const CashCassette& left, const CashCassette& right) {
        return left.getDenomination() > right.getDenomination();
    });
}

int CashDispenser::totalCash() const {
    int total = 0;
    for (const CashCassette& cassette : _cassettes) {
        total += cassette.totalValue();
    }
    return total;
}

NotesPlan CashDispenser::planNotes(int amount) const {
    if (amount <= 0) {
        throw InvalidAmountException("Сумма должна быть больше нуля");
    }
    NotesPlan plan;
    int rest = amount;
    // Жадный алгоритм: берём как можно больше крупных купюр, остаток добираем мелкими.
    for (const CashCassette& cassette : _cassettes) {
        int count = std::min(rest / cassette.getDenomination(), cassette.getCount());
        if (count > 0) {
            plan[cassette.getDenomination()] = count;
        }
        rest -= count * cassette.getDenomination();
    }
    if (rest != 0) {
        throw CashShortageException("Банкомат не может выдать " + std::to_string(amount) + " имеющимися купюрами");
    }
    if (countNotes(plan) > _maxNotesPerWithdrawal) {
        throw LimitExceededException("Слишком много купюр за одну выдачу, уменьшите сумму");
    }
    return plan;
}

bool CashDispenser::canDispense(int amount) const {
    try {
        planNotes(amount);
        return true;
    } catch (const AtmException&) {
        return false;
    }
}

NotesPlan CashDispenser::dispense(int amount) {
    ensureWorking();
    NotesPlan plan = planNotes(amount);
    for (CashCassette& cassette : _cassettes) {
        auto planned = plan.find(cassette.getDenomination());
        if (planned != plan.end()) {
            cassette.take(planned->second);
        }
    }
    _dispensedTotal += amount;
    return plan;
}

void CashDispenser::loadNotes(int denomination, int count) {
    for (CashCassette& cassette : _cassettes) {
        if (cassette.getDenomination() == denomination) {
            cassette.load(count);
            return;
        }
    }
    throw DeviceException("Нет кассеты номиналом " + std::to_string(denomination));
}

int CashDispenser::refillAll() {
    int loaded = 0;
    for (CashCassette& cassette : _cassettes) {
        loaded += cassette.fillToCapacity() * cassette.getDenomination();
    }
    return loaded;
}

int CashDispenser::lowCassettesCount() const {
    return static_cast<int>(std::count_if(_cassettes.begin(), _cassettes.end(),
                                          [](const CashCassette& cassette) { return cassette.isLow(); }));
}

int CashDispenser::minDenomination() const {
    for (auto cassette = _cassettes.rbegin(); cassette != _cassettes.rend(); ++cassette) {
        if (!cassette->isEmpty()) {
            return cassette->getDenomination();
        }
    }
    return 0;
}
