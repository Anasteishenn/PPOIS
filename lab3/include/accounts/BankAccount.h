#pragma once

#include <string>

#include "accounts/AccountStatement.h"
#include "accounts/Currency.h"

/// @brief Банковский (текущий) счёт клиента: баланс, валюта, выписка, заморозка.
class BankAccount {
public:
    /// Бросает InvalidAmountException, если начальный баланс < 0.
    BankAccount(const std::string& number, const std::string& ownerName, const Currency& currency,
                double initialBalance = 0.0);
    virtual ~BankAccount() = default;

    const std::string& getNumber() const { return _number; }
    const std::string& getOwnerName() const { return _ownerName; }
    const Currency& getCurrency() const { return _currency; }
    double getBalance() const { return _balance; }
    bool isFrozen() const { return _isFrozen; }
    const AccountStatement& getStatement() const { return _statement; }

    /// Тип счёта словами.
    virtual std::string type() const;
    /// Сколько денег можно снять (у сберегательного счёта есть неснижаемый остаток).
    virtual double availableFunds() const;
    /// Хватит ли денег, чтобы снять сумму.
    bool canWithdraw(double amount) const;
    /// Зачислить деньги. Бросает InvalidAmountException или AccountFrozenException.
    void deposit(double amount, const std::string& description);
    /// Списать деньги. Бросает InvalidAmountException, AccountFrozenException, InsufficientFundsException.
    void withdraw(double amount, const std::string& description);
    /// Перевести деньги на другой счёт той же валюты. Бросает CurrencyException, если валюты разные.
    void transferTo(BankAccount& target, double amount);
    /// Заморозить счёт (операции запрещены).
    void freeze();
    /// Разморозить счёт.
    void unfreeze();

protected:
    /// Начислить деньги без проверок (используется для процентов по вкладу).
    void addToBalance(double amount, const std::string& description);

private:
    void checkOperation(double amount) const;

    std::string _number;
    std::string _ownerName;
    Currency _currency;
    double _balance;
    bool _isFrozen;
    AccountStatement _statement;
};
