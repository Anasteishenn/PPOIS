#include "operations/TransactionLog.h"

#include <algorithm>

#include "exceptions/AtmException.h"
#include "operations/Withdrawal.h"

TransactionLog::TransactionLog() : _nextId(1) {}

int TransactionLog::nextId() {
    return _nextId++;
}

void TransactionLog::record(std::shared_ptr<Transaction> transaction) {
    if (!transaction) {
        throw TransactionException("Пустая операция");
    }
    _transactions.push_back(std::move(transaction));
}

std::shared_ptr<Transaction> TransactionLog::findById(int id) const {
    for (const std::shared_ptr<Transaction>& transaction : _transactions) {
        if (transaction->getId() == id) {
            return transaction;
        }
    }
    throw TransactionException("Операция №" + std::to_string(id) + " не найдена");
}

std::shared_ptr<Transaction> TransactionLog::lastTransaction() const {
    if (_transactions.empty()) {
        throw TransactionException("Операций ещё не было");
    }
    return _transactions.back();
}

double TransactionLog::totalWithdrawn() const {
    double total = 0.0;
    for (const std::shared_ptr<Transaction>& transaction : _transactions) {
        // dynamic_pointer_cast вернёт пустой указатель, если операция — не снятие.
        auto withdrawal = std::dynamic_pointer_cast<Withdrawal>(transaction);
        if (withdrawal && withdrawal->isSuccessful()) {
            total += withdrawal->getAmount();
        }
    }
    return total;
}

int TransactionLog::countByType(const std::string& type) const {
    return static_cast<int>(std::count_if(_transactions.begin(), _transactions.end(),
                                          [&type](const auto& transaction) { return transaction->type() == type; }));
}

int TransactionLog::failedCount() const {
    return static_cast<int>(std::count_if(_transactions.begin(), _transactions.end(), [](const auto& transaction) {
        return transaction->getStatus() == TransactionStatus::Failed;
    }));
}

std::vector<std::shared_ptr<Transaction>> TransactionLog::forCard(const std::string& cardMask) const {
    std::vector<std::shared_ptr<Transaction>> result;
    for (const std::shared_ptr<Transaction>& transaction : _transactions) {
        if (transaction->getCardMask() == cardMask) {
            result.push_back(transaction);
        }
    }
    return result;
}
