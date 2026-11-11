#pragma once

#include "operations/Transaction.h"

/// @brief Перевод с карты на счёт: номер счёта и имя получателя.
class Transfer : public Transaction {
public:
    Transfer(int id, const std::string& cardMask, double amount, double fee, const std::string& targetAccount,
             const std::string& targetOwner);

    const std::string& getTargetAccount() const { return _targetAccount; }
    const std::string& getTargetOwner() const { return _targetOwner; }

    std::string type() const override;
    /// Описание с получателем.
    std::string describe() const override;
    /// Переводит ли клиент деньги сам себе.
    bool isToOwner(const std::string& ownerName) const;

private:
    std::string _targetAccount;
    std::string _targetOwner;
};
