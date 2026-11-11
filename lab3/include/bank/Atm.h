#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "accounts/StatementEntry.h"
#include "bank/Address.h"
#include "bank/Bank.h"
#include "bank/ProcessingCenter.h"
#include "bank/SmsNotifier.h"
#include "hardware/CardReader.h"
#include "hardware/CashDispenser.h"
#include "hardware/DepositModule.h"
#include "hardware/Display.h"
#include "hardware/Keypad.h"
#include "hardware/ReceiptPrinter.h"
#include "hardware/SecurityCamera.h"
#include "operations/FeePolicy.h"
#include "operations/Session.h"
#include "operations/TransactionLog.h"
#include "operations/UtilityBill.h"

/// @brief Банкомат: собирает устройства, сеанс клиента, журнал операций и связь с банком.
class Atm {
public:
    static constexpr int SESSION_TIMEOUT_SECONDS = 90;
    /// Сумма, выше которой операция считается подозрительной, BYN.
    static constexpr double FRAUD_THRESHOLD = 3000.0;
    /// Сколько последних операций смотрит антифрод.
    static constexpr int FRAUD_WINDOW = 5;
    /// Сколько чеков помещается в рулон бумаги.
    static constexpr int PAPER_ROLL_RECEIPTS = 500;
    static constexpr double SMS_PRICE = 0.05;
    static constexpr int MONTHS_IN_YEAR = 12;
    static constexpr int START_MONTH = 1;
    static constexpr int START_YEAR = 2026;
    /// Валюта, с которой работает банкомат.
    static constexpr const char* CURRENCY = "BYN";

    /// Бросает AccessDeniedException, если номер банкомата или служебный код пустые.
    Atm(const std::string& id, const Address& address, const std::shared_ptr<Bank>& bank, const FeePolicy& feePolicy,
        const std::string& serviceCode);

    const std::string& getId() const { return _id; }
    const Address& getAddress() const { return _address; }
    const FeePolicy& getFeePolicy() const { return _feePolicy; }
    const TransactionLog& getLog() const { return _log; }
    const ProcessingCenter& getProcessing() const { return _processing; }
    const SmsNotifier& getNotifier() const { return _notifier; }
    const Display& getDisplay() const { return _display; }
    const CardReader& getCardReader() const { return _cardReader; }
    const SecurityCamera& getCamera() const { return _camera; }
    const CashDispenser& getDispenser() const { return _dispenser; }
    CashDispenser& getDispenser() { return _dispenser; }
    DepositModule& getDepositModule() { return _depositModule; }
    ReceiptPrinter& getPrinter() { return _printer; }
    bool hasSession() const { return _session != nullptr; }

    /// Установить текущую дату (нужна для проверки срока карты). Бросает DeviceException.
    void setDate(int month, int year);
    /// Установить кассету с купюрами.
    void installCassette(const CashCassette& cassette);
    /// Вставить карту. Заблокированная карта изымается, просроченная — возвращается.
    void insertCard(const std::shared_ptr<Card>& card);
    /// Ввести PIN через клавиатуру. После последней неверной попытки карта изымается.
    void enterPin(const std::string& pin);
    /// Узнать доступный остаток.
    double checkBalance();
    /// Снять наличные. Возвращает выданные купюры.
    NotesPlan withdraw(int amount);
    /// Внести наличные. Возвращает принятую сумму (плохие купюры возвращаются).
    int deposit(const std::vector<Banknote>& notes);
    /// Перевести деньги с карты на счёт.
    void transfer(const std::string& targetAccount, double amount);
    /// Оплатить услугу (мобильная связь, интернет...).
    void payService(const ServiceProvider& provider, const std::string& payerAccount, double amount);
    /// Оплатить коммунальный счёт вместе с пеней на заданный день.
    void payUtilityBill(UtilityBill& bill, int day);
    /// Сменить PIN-код.
    void changePin(const std::string& oldPin, const std::string& newPin);
    /// Мини-выписка: последние операции по счёту карты.
    std::vector<StatementEntry> miniStatement(int count);
    /// Напечатать чек по последней операции.
    std::string printReceipt();
    /// Вернуть карту и закончить сеанс.
    std::shared_ptr<Card> ejectCard();
    /// Прошло время. Если клиент бездействует слишком долго — карта изымается. Возвращает true, если изъята.
    bool tick(int seconds);
    /// Войти в служебный режим. Бросает AccessDeniedException или SessionException (идёт сеанс клиента).
    void enterServiceMode(const std::string& serviceCode) const;
    /// Все устройства банкомата.
    std::vector<AtmDevice*> devices();
    /// Все устройства банкомата (только для чтения).
    std::vector<const AtmDevice*> devices() const;
    /// Может ли банкомат работать: все устройства исправны и есть наличные.
    bool isOperational() const;
    /// Отчёт о состоянии банкомата.
    std::string statusReport() const;

private:
    Session& activeSession();
    Card& authorizedCard();
    void execute(const std::shared_ptr<Transaction>& transaction, const std::function<void()>& steps);
    void chargeCard(Card& card, double total, const std::string& description);
    void captureCard(const std::string& reason);
    void recordEvent(const std::string& event);
    void notifyHolder(const Transaction& transaction);

    std::string _id;
    Address _address;
    std::shared_ptr<Bank> _bank;
    ProcessingCenter _processing;
    FeePolicy _feePolicy;
    CardReader _cardReader;
    Keypad _keypad;
    Display _display;
    CashDispenser _dispenser;
    DepositModule _depositModule;
    ReceiptPrinter _printer;
    SecurityCamera _camera;
    TransactionLog _log;
    SmsNotifier _notifier;
    std::unique_ptr<Session> _session;
    std::string _serviceCode;
    int _month;
    int _year;
};
