#pragma once

#include <memory>
#include <string>

#include "accounts/BankAccount.h"
#include "cards/CardLimit.h"
#include "cards/PinCode.h"

/// @brief Абстрактная банковская карта: номер, владелец, срок действия, PIN, лимиты и привязанный счёт.
class Card {
public:
    /// Длина номера карты.
    static constexpr std::size_t NUMBER_LENGTH = 16;
    /// Сколько последних цифр номера видно в маскированном номере.
    static constexpr std::size_t VISIBLE_DIGITS = 4;
    static constexpr int MONTHS_IN_YEAR = 12;
    /// Дневной лимит снятия по умолчанию, BYN.
    static constexpr double DEFAULT_DAILY_LIMIT = 1000.0;

    /// Бросает InvalidCardException: неверный номер (алгоритм Луна), нет владельца, неверный месяц, нет счёта.
    Card(const std::string& number, const std::string& holderName, int expiryMonth, int expiryYear,
         const std::string& pin, std::shared_ptr<BankAccount> account);
    virtual ~Card() = default;

    const std::string& getNumber() const { return _number; }
    const std::string& getHolderName() const { return _holderName; }
    int getExpiryMonth() const { return _expiryMonth; }
    int getExpiryYear() const { return _expiryYear; }
    bool isBlocked() const { return _isBlocked; }
    const PinCode& getPin() const { return _pin; }
    const CardLimit& getLimit() const { return _limit; }
    std::shared_ptr<BankAccount> getAccount() const { return _account; }

    /// Тип карты словами (дебетовая, кредитная, предоплаченная).
    virtual std::string type() const = 0;
    /// Сколько денег доступно по карте.
    virtual double availableFunds() const;
    /// Списать деньги с карты.
    virtual void charge(double amount, const std::string& description);

    /// Проверка номера карты по алгоритму Луна (16 цифр, контрольная сумма делится на 10).
    static bool isValidNumber(const std::string& number);
    /// Истёк ли срок действия на заданную дату (месяц, год).
    bool isExpired(int month, int year) const;
    /// Проверить, что картой можно пользоваться. Бросает CardBlockedException или CardExpiredException.
    void validate(int month, int year) const;
    /// Проверить PIN. После последней неверной попытки карта блокируется.
    void authenticate(const std::string& pin);
    /// Сменить PIN-код.
    void changePin(const std::string& oldPin, const std::string& newPin);
    /// Учесть снятие наличных в дневном лимите. Бросает LimitExceededException.
    void registerCashWithdrawal(double amount);
    /// Зачислить деньги на счёт карты.
    void topUp(double amount, const std::string& description);
    /// Номер со звёздочками: «**** **** **** 1234».
    std::string maskedNumber() const;
    /// Заблокировать карту.
    void block();
    /// Разблокировать карту (и сбросить ошибки PIN).
    void unblock();
    /// Новый день: обнулить дневные лимиты.
    void startNewDay();

protected:
    BankAccount& account() const { return *_account; }

private:
    std::string _number;
    std::string _holderName;
    int _expiryMonth;
    int _expiryYear;
    bool _isBlocked;
    PinCode _pin;
    CardLimit _limit;
    std::shared_ptr<BankAccount> _account;
};
