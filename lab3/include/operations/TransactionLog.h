#pragma once

#include <memory>
#include <string>
#include <vector>

#include "operations/Transaction.h"

/// @brief Журнал операций банкомата: выдаёт номера операций и хранит все операции.
class TransactionLog {
public:
    TransactionLog();

    const std::vector<std::shared_ptr<Transaction>>& getTransactions() const { return _transactions; }

    /// Новый уникальный номер операции.
    int nextId();
    /// Записать операцию в журнал. Бросает TransactionException, если операция пустая.
    void record(std::shared_ptr<Transaction> transaction);
    /// Найти операцию по номеру. Бросает TransactionException, если её нет.
    std::shared_ptr<Transaction> findById(int id) const;
    /// Последняя операция. Бросает TransactionException, если журнал пуст.
    std::shared_ptr<Transaction> lastTransaction() const;
    /// Сколько наличных выдано успешными снятиями.
    double totalWithdrawn() const;
    /// Сколько операций такого вида.
    int countByType(const std::string& type) const;
    /// Сколько операций отклонено.
    int failedCount() const;
    /// Все операции по карте.
    std::vector<std::shared_ptr<Transaction>> forCard(const std::string& cardMask) const;

private:
    std::vector<std::shared_ptr<Transaction>> _transactions;
    int _nextId;
};
