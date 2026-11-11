#include "bank/ProcessingCenter.h"

#include "exceptions/AtmException.h"

ProcessingCenter::ProcessingCenter(std::shared_ptr<Bank> bank, const FraudDetector& fraudDetector)
    : _bank(std::move(bank)), _fraudDetector(fraudDetector), _approvedCount(0), _declinedCount(0) {
    if (!_bank) {
        throw AccountNotFoundException("Процессинговому центру нужен банк");
    }
}

void ProcessingCenter::authorize(const Card& card, double amount, int month, int year) {
    try {
        card.validate(month, year);
        if (amount > card.availableFunds()) {
            throw InsufficientFundsException("Недостаточно средств на карте " + card.maskedNumber());
        }
        _fraudDetector.check(card.maskedNumber(), amount);
        ++_approvedCount;
    } catch (const AtmException&) {
        ++_declinedCount;
        throw;
    }
}

bool ProcessingCenter::isForeignCard(const Card& card) const {
    return !_bank->isOwnCard(card.getNumber());
}

double ProcessingCenter::approvalRate() const {
    int total = _approvedCount + _declinedCount;
    return total == 0 ? PERCENT : _approvedCount * PERCENT / total;
}

void ProcessingCenter::resetStatistics() {
    _approvedCount = 0;
    _declinedCount = 0;
}
