#include "TestData.h"
#include "bank/CashCollector.h"
#include "bank/Technician.h"
#include "doctest.h"
#include "exceptions/AtmException.h"
#include "operations/Withdrawal.h"

using doctest::Approx;

namespace {

std::vector<Banknote> someNotes() {
    return {Banknote(100, "BYN", "AB0000001"), Banknote(30, "BYN", "AB0000002"), Banknote(50, "BYN", "AB0000003")};
}

void startSession(testdata::AtmSetup& setup) {
    setup.atm->insertCard(setup.card);
    setup.atm->enterPin(testdata::PIN);
}

}  // namespace

TEST_CASE("Atm: создание и ошибки сеанса") {
    auto bank = testdata::bank();
    Address address("Минск", "ул. Тестовая", "1");
    FeePolicy fees(0, 1, 1, 0);
    CHECK_THROWS_AS(Atm("A", address, bank, fees, ""), AccessDeniedException);
    CHECK_THROWS_AS(Atm("A", address, nullptr, fees, "1"), AccountNotFoundException);
    testdata::AtmSetup setup = testdata::atmSetup();
    Atm& atm = *setup.atm;
    CHECK_THROWS_AS(atm.setDate(13, 2026), DeviceException);
    CHECK_THROWS_AS(atm.checkBalance(), SessionException);
    CHECK_THROWS_AS(atm.ejectCard(), SessionException);
    CHECK_THROWS_AS(atm.enterPin("1234"), SessionException);
    CHECK_THROWS_AS(atm.printReceipt(), TransactionException);
    atm.insertCard(setup.card);
    CHECK(atm.hasSession());
    CHECK(atm.getDisplay().lastMessage() == "Введите PIN-код");
    CHECK_THROWS_AS(atm.checkBalance(), SessionException);
    CHECK_THROWS_AS(atm.insertCard(setup.card), SessionException);
    CHECK_THROWS_AS(atm.enterPin("0000"), InvalidPinException);
    atm.enterPin("1234");
    CHECK(atm.getDisplay().lastMessage() == "Выберите операцию");
    CHECK(atm.getAddress().getCity() == "Минск");
    CHECK(atm.getCardReader().hasCard());
}

TEST_CASE("Atm: полный сценарий клиента") {
    testdata::AtmSetup setup = testdata::atmSetup();
    Atm& atm = *setup.atm;
    auto anna = setup.bank->openAccount("Анна", "BYN", 0);
    auto dollars = setup.bank->openAccount("Анна", "USD", 0);
    startSession(setup);
    CHECK(atm.checkBalance() == Approx(1000));
    NotesPlan notes = atm.withdraw(380);
    CHECK(notes.size() == 5);
    CHECK(notes[200] == 1);
    CHECK(setup.account->getBalance() == Approx(620));
    CHECK(atm.deposit(someNotes()) == 150);
    CHECK(setup.account->getBalance() == Approx(770));
    atm.transfer(anna->getNumber(), 100);
    CHECK(anna->getBalance() == Approx(100));
    CHECK(setup.account->getBalance() == Approx(669.5));
    atm.transfer(dollars->getNumber(), 300);
    CHECK(dollars->getBalance() == Approx(99));
    CHECK(setup.account->getBalance() == Approx(368));
    atm.payService(ServiceProvider("МТС", "Мобильная связь", 9, 0), "291234567", 20);
    CHECK(setup.account->getBalance() == Approx(348));
    UtilityBill bill(ServiceProvider("Энерго", "Коммунальные услуги", 10, 0), "1234567890", 50, 25);
    atm.payUtilityBill(bill, 27);
    CHECK(bill.isPaid());
    CHECK(setup.account->getBalance() == Approx(297.9));
    CHECK_THROWS_AS(atm.payUtilityBill(bill, 27), TransactionException);
    CHECK(atm.miniStatement(3).size() == 3);
    CHECK(atm.printReceipt().find("Оплата услуг") != std::string::npos);
    atm.changePin("1234", "4321");
    CHECK(setup.card->getPin().matches("4321"));
    CHECK(atm.ejectCard() == setup.card);
    CHECK_FALSE(atm.hasSession());
    CHECK(atm.getDisplay().lastMessage() == "Добро пожаловать! Вставьте карту");
}

TEST_CASE("Atm: журнал, SMS и отчёт после операций") {
    testdata::AtmSetup setup = testdata::atmSetup();
    Atm& atm = *setup.atm;
    startSession(setup);
    atm.checkBalance();
    atm.withdraw(380);
    atm.deposit(someNotes());
    const TransactionLog& log = atm.getLog();
    CHECK(log.getTransactions().size() == 3);
    CHECK(log.failedCount() == 0);
    CHECK(log.totalWithdrawn() == Approx(380));
    CHECK(log.countByType("Внесение наличных") == 1);
    CHECK(atm.getNotifier().getSentMessages().size() == 3);
    CHECK(atm.getProcessing().getApprovedCount() == 1);
    std::string report = atm.statusReport();
    CHECK(report.find("Банкомат ATM-T") != std::string::npos);
    CHECK(report.find("Операций: 3, отклонено: 0") != std::string::npos);
    CHECK(report.find("в купюроприёмнике: 150 BYN") != std::string::npos);
    CHECK(atm.getFeePolicy().getTransferPercent() == Approx(0.5));
}

TEST_CASE("Atm: отказы при снятии наличных") {
    testdata::AtmSetup setup = testdata::atmSetup(5000);
    Atm& atm = *setup.atm;
    startSession(setup);
    CHECK_THROWS_AS(atm.withdraw(15), CashShortageException);
    CHECK(atm.getLog().getTransactions().empty());
    CHECK_THROWS_AS(atm.withdraw(1200), LimitExceededException);
    CHECK(atm.getLog().failedCount() == 1);
    CHECK(atm.getCamera().getSnapshots().size() == 1);
    CHECK(atm.getDisplay().lastMessage().rfind("Операция отклонена", 0) == 0);
    CHECK(setup.account->getBalance() == Approx(5000));
    atm.getDispenser().reportError("замятие купюр");
    CHECK_THROWS_AS(atm.withdraw(100), DeviceException);

    testdata::AtmSetup poor = testdata::atmSetup(100);
    startSession(poor);
    CHECK_THROWS_AS(poor.atm->withdraw(200), InsufficientFundsException);
    CHECK(poor.atm->getProcessing().getDeclinedCount() == 1);
}

TEST_CASE("Atm: чужая карта платит комиссию") {
    testdata::AtmSetup setup = testdata::atmSetup();
    auto foreignAccount = testdata::account(500, "OTHER-1");
    auto foreignCard = testdata::debitCard(foreignAccount, testdata::CARD_5);
    setup.atm->insertCard(foreignCard);
    setup.atm->enterPin(testdata::PIN);
    setup.atm->withdraw(100);
    CHECK(foreignAccount->getBalance() == Approx(398));
    auto withdrawal = std::dynamic_pointer_cast<Withdrawal>(setup.atm->getLog().lastTransaction());
    REQUIRE(withdrawal != nullptr);
    CHECK(withdrawal->isForeignCard());
    CHECK(withdrawal->getFee() == Approx(2));
    CHECK(setup.atm->getNotifier().getSentMessages().empty());
}

TEST_CASE("Atm: заблокированная, просроченная карта и неверный PIN") {
    testdata::AtmSetup setup = testdata::atmSetup();
    Atm& atm = *setup.atm;
    setup.card->block();
    CHECK_THROWS_AS(atm.insertCard(setup.card), CardBlockedException);
    CHECK(atm.getCardReader().capturedCount() == 1);
    CHECK_FALSE(atm.hasSession());

    testdata::AtmSetup expired = testdata::atmSetup();
    expired.atm->setDate(1, 2029);
    CHECK_THROWS_AS(expired.atm->insertCard(expired.card), CardExpiredException);
    CHECK_FALSE(expired.atm->getCardReader().hasCard());
    CHECK(expired.atm->getCardReader().capturedCount() == 0);

    testdata::AtmSetup locked = testdata::atmSetup();
    locked.atm->insertCard(locked.card);
    CHECK_THROWS_AS(locked.atm->enterPin("0000"), InvalidPinException);
    CHECK_THROWS_AS(locked.atm->enterPin("0000"), InvalidPinException);
    CHECK_THROWS_AS(locked.atm->enterPin("0000"), PinLockedException);
    CHECK(locked.card->isBlocked());
    CHECK(locked.atm->getCardReader().capturedCount() == 1);
    CHECK_FALSE(locked.atm->hasSession());
}

TEST_CASE("Atm: блокировка при смене PIN и тайм-аут") {
    testdata::AtmSetup setup = testdata::atmSetup();
    startSession(setup);
    CHECK_THROWS_AS(setup.atm->changePin("0000", "1111"), InvalidPinException);
    CHECK_THROWS_AS(setup.atm->changePin("0000", "1111"), InvalidPinException);
    CHECK_THROWS_AS(setup.atm->changePin("0000", "1111"), PinLockedException);
    CHECK(setup.atm->getCardReader().capturedCount() == 1);

    testdata::AtmSetup slow = testdata::atmSetup();
    CHECK_FALSE(slow.atm->tick(10));
    slow.atm->insertCard(slow.card);
    CHECK_FALSE(slow.atm->tick(30));
    CHECK(slow.atm->tick(Atm::SESSION_TIMEOUT_SECONDS));
    CHECK(slow.atm->getCardReader().capturedCount() == 1);
    CHECK_FALSE(slow.atm->hasSession());
}

TEST_CASE("Atm: внесение, перевод и оплата с ошибками") {
    testdata::AtmSetup setup = testdata::atmSetup();
    Atm& atm = *setup.atm;
    auto anna = setup.bank->openAccount("Анна", "BYN", 0);
    startSession(setup);
    CHECK_THROWS_AS(atm.deposit({Banknote(30, "BYN", "AB0000001")}), InvalidBanknoteException);
    CHECK_THROWS_AS(atm.transfer(anna->getNumber(), 0), InvalidAmountException);
    CHECK_THROWS_AS(atm.transfer("NOPE", 10), AccountNotFoundException);
    CHECK_THROWS_AS(atm.payService(ServiceProvider("МТС", "Связь", 9, 0), "291234567", 0), InvalidAmountException);
    anna->freeze();
    CHECK_THROWS_AS(atm.transfer(anna->getNumber(), 10), AccountFrozenException);
    setup.account->freeze();
    CHECK_THROWS_AS(atm.deposit(someNotes()), AccountFrozenException);
    CHECK(atm.getLog().getTransactions().empty());
}

TEST_CASE("Atm: антифрод блокирует крупный перевод") {
    testdata::AtmSetup setup = testdata::atmSetup(10000);
    auto anna = setup.bank->openAccount("Анна", "BYN", 0);
    startSession(setup);
    CHECK_THROWS_AS(setup.atm->transfer(anna->getNumber(), 3500), FraudDetectedException);
    CHECK(setup.atm->getProcessing().getFraudDetector().getAlerts().size() == 1);
    CHECK(setup.atm->getLog().failedCount() == 1);
    CHECK(setup.account->getBalance() == Approx(10000));
    CHECK(anna->getBalance() == Approx(0));
}

TEST_CASE("Atm: служебный режим и исправность") {
    testdata::AtmSetup setup = testdata::atmSetup();
    Atm& atm = *setup.atm;
    CHECK(atm.isOperational());
    CHECK_THROWS_AS(atm.enterServiceMode("0000"), AccessDeniedException);
    atm.enterServiceMode(testdata::SERVICE_CODE);
    atm.insertCard(setup.card);
    CHECK_THROWS_AS(atm.enterServiceMode(testdata::SERVICE_CODE), SessionException);
    atm.getPrinter().reportError("нет бумаги");
    CHECK_FALSE(atm.isOperational());
    CHECK(atm.devices().size() == 7);
    Atm empty("ATM-E", Address("Минск", "ул. Пустая", "1"), setup.bank, FeePolicy(0, 1, 1, 0), "1");
    CHECK_FALSE(empty.isOperational());
}

TEST_CASE("CashCollector: инкассация банкомата") {
    testdata::AtmSetup setup = testdata::atmSetup();
    Atm& atm = *setup.atm;
    startSession(setup);
    atm.withdraw(1000);
    atm.deposit(someNotes());
    atm.ejectCard();
    CashCollector collector("Сергей", "C-01", "2468");
    CHECK_THROWS_AS(collector.encash(atm, testdata::SERVICE_CODE, 3), AccessDeniedException);
    collector.startShift("2468");
    CHECK_THROWS_AS(collector.encash(atm, "0000", 3), AccessDeniedException);
    EncashmentReport report = collector.encash(atm, testdata::SERVICE_CODE, 3);
    CHECK(report.getLoadedAmount() == Approx(17000 + 8000 + 4000 + 1600 + 800));
    CHECK(report.getCollectedAmount() == Approx(150));
    CHECK(atm.getDepositModule().storedValue() == 0);
    CHECK(collector.getVisits() == 1);
    CHECK(collector.getCollectedTotal() == Approx(150));
    CashDispenser spare("SPARE");
    spare.addCassette(CashCassette(50, 0, 10, "BYN"));
    CHECK(collector.loadCassette(spare, 50, 4) == 200);
    CHECK(collector.getLoadedTotal() == Approx(31400 + 200));
}

TEST_CASE("Technician: диагностика и ремонт") {
    CHECK_THROWS_AS(Technician("Пётр", "T-1", "1", "Электроника", -1), InvalidAmountException);
    testdata::AtmSetup setup = testdata::atmSetup();
    Atm& atm = *setup.atm;
    Technician technician("Пётр", "T-1", "2468", "Электроника", 50);
    MaintenanceLog log("ATM-T", 30);
    CHECK(technician.role() == "Техник");
    CHECK(technician.diagnose(atm).empty());
    atm.getPrinter().reportError("нет бумаги");
    CHECK(technician.diagnose(atm).size() == 1);
    CHECK_THROWS_AS(technician.repairAll(atm, testdata::SERVICE_CODE, log, 5), AccessDeniedException);
    technician.startShift("2468");
    CHECK_THROWS_AS(technician.repairAll(atm, "0000", log, 5), AccessDeniedException);
    CHECK(technician.repairAll(atm, testdata::SERVICE_CODE, log, 5) == 1);
    CHECK(atm.getPrinter().isWorking());
    CHECK(log.getRecords().size() == 2);
    CHECK(log.recordsFor("Принтер чеков").front().getDescription() == "Ремонт: нет бумаги");
    CHECK(log.totalCost() == Approx(50));
    CHECK_THROWS_AS(technician.repair(atm.getPrinter(), log, 6), DeviceException);
    CHECK(technician.getRepairsDone() == 1);
    CHECK(technician.getSpecialization() == "Электроника");
    CHECK(technician.getCallOutFee() == Approx(50));
}
