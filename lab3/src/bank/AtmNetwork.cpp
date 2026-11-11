#include "bank/AtmNetwork.h"

#include "exceptions/AtmException.h"

AtmNetwork::AtmNetwork(const std::string& bankName, int minCashLevel)
    : _bankName(bankName), _minCashLevel(minCashLevel) {
    if (minCashLevel < 0) {
        throw InvalidAmountException("Минимальный остаток наличных не может быть отрицательным");
    }
}

void AtmNetwork::addAtm(std::shared_ptr<Atm> atm) {
    if (!atm) {
        throw DeviceException("Пустой банкомат");
    }
    for (const std::shared_ptr<Atm>& existing : _atms) {
        if (existing->getId() == atm->getId()) {
            throw DeviceException("Банкомат " + atm->getId() + " уже есть в сети");
        }
    }
    _atms.push_back(std::move(atm));
}

std::shared_ptr<Atm> AtmNetwork::findById(const std::string& id) const {
    for (const std::shared_ptr<Atm>& atm : _atms) {
        if (atm->getId() == id) {
            return atm;
        }
    }
    throw DeviceException("Банкомат " + id + " не найден");
}

std::vector<std::shared_ptr<Atm>> AtmNetwork::atmsInCity(const std::string& city) const {
    std::vector<std::shared_ptr<Atm>> result;
    for (const std::shared_ptr<Atm>& atm : _atms) {
        if (atm->getAddress().getCity() == city) {
            result.push_back(atm);
        }
    }
    return result;
}

std::vector<std::shared_ptr<Atm>> AtmNetwork::needingEncashment() const {
    std::vector<std::shared_ptr<Atm>> result;
    for (const std::shared_ptr<Atm>& atm : _atms) {
        if (atm->getDispenser().totalCash() < _minCashLevel) {
            result.push_back(atm);
        }
    }
    return result;
}

int AtmNetwork::totalCash() const {
    int total = 0;
    for (const std::shared_ptr<Atm>& atm : _atms) {
        total += atm->getDispenser().totalCash();
    }
    return total;
}

int AtmNetwork::operationalCount() const {
    int count = 0;
    for (const std::shared_ptr<Atm>& atm : _atms) {
        if (atm->isOperational()) {
            ++count;
        }
    }
    return count;
}
