#include "hardware/BanknoteValidator.h"

#include "exceptions/AtmException.h"

BanknoteValidator::BanknoteValidator(const std::string& currencyCode)
    : _currencyCode(currencyCode), _acceptedCount(0), _rejectedCount(0) {}

std::string BanknoteValidator::rejectionReason(const Banknote& note) const {
    if (note.getCurrencyCode() != _currencyCode) {
        return "чужая валюта " + note.getCurrencyCode();
    }
    if (!Banknote::isValidDenomination(note.getDenomination())) {
        return "неизвестный номинал " + std::to_string(note.getDenomination());
    }
    if (!note.hasValidSerial()) {
        return "подозрение на подделку";
    }
    if (note.isDamaged()) {
        return "купюра повреждена";
    }
    return "";
}

bool BanknoteValidator::inspect(const Banknote& note) {
    bool isAccepted = rejectionReason(note).empty();
    if (isAccepted) {
        ++_acceptedCount;
    } else {
        ++_rejectedCount;
    }
    return isAccepted;
}

void BanknoteValidator::check(const Banknote& note) {
    if (!inspect(note)) {
        throw InvalidBanknoteException("Купюра не принята: " + rejectionReason(note));
    }
}

double BanknoteValidator::rejectionRate() const {
    int total = _acceptedCount + _rejectedCount;
    return total == 0 ? 0.0 : _rejectedCount * PERCENT / total;
}
