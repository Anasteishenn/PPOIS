#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "accounts/BankAccount.h"
#include "accounts/CurrencyExchange.h"
#include "cards/Card.h"
#include "cards/CardHolder.h"

/// @brief Банк: счета, карты, клиенты и обмен валют. Банкоматы обращаются к нему за данными.
class Bank {
public:
    /// С этого числа начинаются номера новых счетов.
    static constexpr int ACCOUNT_NUMBER_BASE = 10000;

    /// Бросает AccountNotFoundException, если название или БИК пустые.
    Bank(const std::string& name, const std::string& bic, const CurrencyExchange& exchange);

    const std::string& getName() const { return _name; }
    const std::string& getBic() const { return _bic; }
    CurrencyExchange& getExchange() { return _exchange; }
    const std::vector<std::shared_ptr<CardHolder>>& getClients() const { return _clients; }

    /// Открыть текущий счёт в валюте из таблицы обмена. Номер счёта присваивает банк.
    std::shared_ptr<BankAccount> openAccount(const std::string& ownerName, const std::string& currencyCode,
                                             double initialBalance);
    /// Зарегистрировать готовый счёт (например, вклад). Бросает AccountNotFoundException, если номер занят.
    void addAccount(std::shared_ptr<BankAccount> account);
    /// Найти счёт. Бросает AccountNotFoundException.
    std::shared_ptr<BankAccount> findAccount(const std::string& number) const;
    /// Добавить клиента (повторно тот же клиент не добавляется).
    void addClient(std::shared_ptr<CardHolder> holder);
    /// Выпустить карту клиенту. Счёт карты должен быть открыт в этом банке.
    void issueCard(const std::shared_ptr<Card>& card, const std::shared_ptr<CardHolder>& holder);
    /// Найти карту. Бросает InvalidCardException.
    std::shared_ptr<Card> findCard(const std::string& number) const;
    /// Выпущена ли карта этим банком (иначе — чужая карта, комиссия выше).
    bool isOwnCard(const std::string& number) const;
    /// Заблокировать карту по номеру.
    void blockCard(const std::string& number);
    /// Найти владельца карты (nullptr, если не нашли).
    std::shared_ptr<CardHolder> findHolderOf(const std::string& cardNumber) const;
    /// Зачислить перевод на счёт с конвертацией валюты. Возвращает зачисленную сумму.
    double receiveTransfer(const std::string& targetNumber, double amount, const std::string& currencyCode,
                           const std::string& senderName);
    /// Перевод между двумя счетами банка. Возвращает зачисленную сумму.
    double transfer(const std::string& fromNumber, const std::string& toNumber, double amount);
    /// Все деньги клиентов в пересчёте на рубли.
    double totalDepositsInByn() const;

private:
    std::string _name;
    std::string _bic;
    CurrencyExchange _exchange;
    std::map<std::string, std::shared_ptr<BankAccount>> _accounts;
    std::map<std::string, std::shared_ptr<Card>> _cards;
    std::vector<std::shared_ptr<CardHolder>> _clients;
    int _nextAccountNumber;
};
