#include "bank/Atm.h"

#include "exceptions/AtmException.h"
#include "operations/BalanceInquiry.h"
#include "operations/Deposit.h"
#include "operations/ServicePayment.h"
#include "operations/Transfer.h"
#include "operations/Withdrawal.h"

Atm::Atm(const std::string& id, const Address& address, const std::shared_ptr<Bank>& bank, const FeePolicy& feePolicy,
         const std::string& serviceCode)
    : _id(id),
      _address(address),
      _bank(bank),
      _processing(bank, FraudDetector(FRAUD_THRESHOLD, FRAUD_WINDOW)),
      _feePolicy(feePolicy),
      _cardReader(id + "-CR"),
      _keypad(id + "-KP", true),
      _display(id + "-DS"),
      _dispenser(id + "-CD"),
      _depositModule(id + "-DM", BanknoteValidator(CURRENCY)),
      _printer(id + "-PR", PAPER_ROLL_RECEIPTS),
      _camera(id + "-CAM"),
      _notifier(SMS_PRICE),
      _serviceCode(serviceCode),
      _month(START_MONTH),
      _year(START_YEAR) {
    if (id.empty() || serviceCode.empty()) {
        throw AccessDeniedException("У банкомата должны быть номер и служебный код");
    }
    _camera.startRecording();
    _display.show(_display.welcomeText());
}

void Atm::setDate(int month, int year) {
    if (month < 1 || month > MONTHS_IN_YEAR) {
        throw DeviceException("Неверный месяц: " + std::to_string(month));
    }
    _month = month;
    _year = year;
}

void Atm::installCassette(const CashCassette& cassette) {
    _dispenser.addCassette(cassette);
}

void Atm::insertCard(const std::shared_ptr<Card>& card) {
    if (_session) {
        throw SessionException("Банкомат уже обслуживает клиента");
    }
    _cardReader.insertCard(card);
    try {
        card->validate(_month, _year);
    } catch (const CardBlockedException&) {
        captureCard("Изъята заблокированная карта " + card->maskedNumber());
        throw;
    } catch (const CardExpiredException&) {
        _cardReader.ejectCard();
        throw;
    }
    _session = std::make_unique<Session>(card, SESSION_TIMEOUT_SECONDS);
    _display.show("Введите PIN-код");
}

void Atm::enterPin(const std::string& pin) {
    Session& session = activeSession();
    _keypad.typeText(pin);
    try {
        session.authenticate(_keypad.takeInput());
    } catch (const PinLockedException&) {
        captureCard("Карта изъята: PIN-код заблокирован");
        throw;
    }
    _display.show("Выберите операцию");
}

double Atm::checkBalance() {
    Card& card = authorizedCard();
    double balance = card.availableFunds();
    std::string currency = card.getAccount()->getCurrency().getCode();
    execute(std::make_shared<BalanceInquiry>(_log.nextId(), card.maskedNumber(), balance, currency), []() {});
    return balance;
}

NotesPlan Atm::withdraw(int amount) {
    Card& card = authorizedCard();
    _dispenser.ensureWorking();
    _dispenser.planNotes(amount);  // заранее проверяем, что купюр хватит
    bool isOwnCard = _bank->isOwnCard(card.getNumber());
    double fee = _feePolicy.withdrawalFee(amount, isOwnCard);
    auto withdrawal = std::make_shared<Withdrawal>(_log.nextId(), card.maskedNumber(), amount, fee, !isOwnCard);
    execute(withdrawal, [&]() {
        _processing.authorize(card, withdrawal->total(), _month, _year);
        card.registerCashWithdrawal(amount);
        card.charge(withdrawal->total(), "Снятие наличных, банкомат " + _id);
        withdrawal->recordNotes(_dispenser.dispense(amount));
    });
    return withdrawal->getNotes();
}

int Atm::deposit(const std::vector<Banknote>& notes) {
    Card& card = authorizedCard();
    if (card.getAccount()->isFrozen()) {
        throw AccountFrozenException("Счёт карты заморожен, внесение невозможно");
    }
    int accepted = _depositModule.acceptNotes(notes);
    int rejected = _depositModule.getLastRejectedCount();
    if (accepted == 0) {
        throw InvalidBanknoteException("Ни одна купюра не принята, заберите купюры");
    }
    int acceptedCount = static_cast<int>(notes.size()) - rejected;
    auto deposit = std::make_shared<Deposit>(_log.nextId(), card.maskedNumber(), accepted, acceptedCount, rejected);
    execute(deposit, [&]() { card.topUp(accepted, "Внесение наличных, банкомат " + _id); });
    return accepted;
}

void Atm::transfer(const std::string& targetAccount, double amount) {
    Card& card = authorizedCard();
    if (amount <= 0) {
        throw InvalidAmountException("Сумма перевода должна быть больше нуля");
    }
    std::shared_ptr<BankAccount> target = _bank->findAccount(targetAccount);
    if (target->isFrozen()) {
        throw AccountFrozenException("Счёт получателя заморожен");
    }
    double fee = _feePolicy.transferFee(amount);
    auto transfer = std::make_shared<Transfer>(_log.nextId(), card.maskedNumber(), amount, fee, targetAccount,
                                               target->getOwnerName());
    std::string currency = card.getAccount()->getCurrency().getCode();
    execute(transfer, [&]() {
        chargeCard(card, transfer->total(), "Перевод на счёт " + targetAccount);
        _bank->receiveTransfer(targetAccount, amount, currency, card.getHolderName());
    });
}

void Atm::payService(const ServiceProvider& provider, const std::string& payerAccount, double amount) {
    Card& card = authorizedCard();
    if (amount <= 0) {
        throw InvalidAmountException("Сумма платежа должна быть больше нуля");
    }
    double fee = _feePolicy.paymentFee(amount, provider);
    auto payment =
        std::make_shared<ServicePayment>(_log.nextId(), card.maskedNumber(), amount, fee, provider, payerAccount);
    execute(payment, [&]() { chargeCard(card, payment->total(), "Оплата: " + provider.label()); });
}

void Atm::payUtilityBill(UtilityBill& bill, int day) {
    if (bill.isPaid()) {
        throw TransactionException("Счёт уже оплачен");
    }
    double amount = bill.totalToPay(day);
    payService(bill.getProvider(), bill.getAccountNumber(), amount);
    bill.markPaid(amount, day);
}

void Atm::changePin(const std::string& oldPin, const std::string& newPin) {
    Card& card = authorizedCard();
    try {
        card.changePin(oldPin, newPin);
    } catch (const PinLockedException&) {
        captureCard("Карта изъята: PIN-код заблокирован при смене");
        throw;
    }
    activeSession().registerOperation();
    _display.show("PIN-код изменён");
}

std::vector<StatementEntry> Atm::miniStatement(int count) {
    Card& card = authorizedCard();
    activeSession().registerOperation();
    return card.getAccount()->getStatement().lastEntries(count);
}

std::string Atm::printReceipt() {
    std::shared_ptr<Transaction> last = _log.lastTransaction();
    Receipt receipt(_id, last->getId(), last->getCardMask());
    receipt.addTransaction(*last);
    return _printer.print(receipt);
}

std::shared_ptr<Card> Atm::ejectCard() {
    if (!_session) {
        throw SessionException("Карта не вставлена");
    }
    _session.reset();
    std::shared_ptr<Card> card = _cardReader.ejectCard();
    _display.show(_display.welcomeText());
    return card;
}

bool Atm::tick(int seconds) {
    if (!_session) {
        return false;
    }
    _session->tick(seconds);
    if (!_session->isExpired()) {
        return false;
    }
    captureCard("Карта изъята: клиент не забрал её вовремя");
    return true;
}

void Atm::enterServiceMode(const std::string& serviceCode) const {
    if (serviceCode != _serviceCode) {
        throw AccessDeniedException("Неверный служебный код банкомата " + _id);
    }
    if (_session) {
        throw SessionException("Нельзя обслуживать банкомат во время сеанса клиента");
    }
}

std::vector<AtmDevice*> Atm::devices() {
    return {&_cardReader, &_keypad, &_display, &_dispenser, &_depositModule, &_printer, &_camera};
}

std::vector<const AtmDevice*> Atm::devices() const {
    return {&_cardReader, &_keypad, &_display, &_dispenser, &_depositModule, &_printer, &_camera};
}

bool Atm::isOperational() const {
    for (const AtmDevice* device : devices()) {
        if (!device->selfTest()) {
            return false;
        }
    }
    return _dispenser.totalCash() > 0;
}

std::string Atm::statusReport() const {
    std::string report = "Банкомат " + _id + ", " + _address.format() + "\n";
    for (const AtmDevice* device : devices()) {
        report += "  " + device->statusLine() + "\n";
    }
    report += "Наличных: " + std::to_string(_dispenser.totalCash()) + " BYN, в купюроприёмнике: " +
              std::to_string(_depositModule.storedValue()) + " BYN\n";
    report += "Операций: " + std::to_string(_log.getTransactions().size()) + ", отклонено: " +
              std::to_string(_log.failedCount()) + ", изъято карт: " + std::to_string(_cardReader.capturedCount());
    return report;
}

Session& Atm::activeSession() {
    if (!_session) {
        throw SessionException("Вставьте карту");
    }
    return *_session;
}

Card& Atm::authorizedCard() {
    Session& session = activeSession();
    session.requireAuthenticated();
    return *session.getCard();
}

void Atm::execute(const std::shared_ptr<Transaction>& transaction, const std::function<void()>& steps) {
    _log.record(transaction);
    try {
        steps();
        transaction->complete();
    } catch (const AtmException& error) {
        transaction->fail(error.what());
        recordEvent("Отказ в операции №" + std::to_string(transaction->getId()) + ": " + error.what());
        _display.show("Операция отклонена: " + std::string(error.what()));
        throw;
    }
    activeSession().registerOperation();
    _display.show(transaction->describe());
    notifyHolder(*transaction);
}

void Atm::chargeCard(Card& card, double total, const std::string& description) {
    _processing.authorize(card, total, _month, _year);
    card.charge(total, description);
}

void Atm::captureCard(const std::string& reason) {
    _session.reset();
    _cardReader.captureCard();
    recordEvent(reason);
    _display.show("Карта задержана. Обратитесь в банк");
}

void Atm::recordEvent(const std::string& event) {
    if (_camera.isRecording()) {
        _camera.takeSnapshot(event);
    }
}

void Atm::notifyHolder(const Transaction& transaction) {
    std::shared_ptr<CardHolder> holder = _bank->findHolderOf(activeSession().getCard()->getNumber());
    if (holder) {
        _notifier.notify(*holder, transaction);
    }
}
