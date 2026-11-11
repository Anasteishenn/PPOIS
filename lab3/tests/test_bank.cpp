#include "TestData.h"
#include "bank/AtmNetwork.h"
#include "bank/CashCollector.h"
#include "bank/Technician.h"
#include "doctest.h"
#include "exceptions/AtmException.h"
#include "operations/Withdrawal.h"

using doctest::Approx;

TEST_CASE("Address: формат и сравнение") {
    CHECK_THROWS_AS(Address("", "ул. Ленина", "1"), DeviceException);
    Address indoor("Минск", "ул. Ленина", "5", "ТЦ Галерея");
    Address outdoor("Минск", "пр. Победителей", "10");
    Address brest("Брест", "ул. Советская", "1");
    CHECK(indoor.format() == "г. Минск, ул. Ленина, 5 (ТЦ Галерея)");
    CHECK(outdoor.format() == "г. Минск, пр. Победителей, 10");
    CHECK(indoor.isIndoor());
    CHECK_FALSE(outdoor.isIndoor());
    CHECK(indoor.isSameCity(outdoor));
    CHECK_FALSE(indoor.isSameCity(brest));
    CHECK(indoor.getStreet() == "ул. Ленина");
    CHECK(indoor.getBuilding() == "5");
    CHECK(indoor.getPlace() == "ТЦ Галерея");
}

TEST_CASE("Bank: счета и карты") {
    CHECK_THROWS_AS(Bank("", "B", CurrencyExchange(0)), AccountNotFoundException);
    auto bank = testdata::bank();
    CHECK_THROWS_AS(bank->openAccount("Иван", "EUR", 0), CurrencyException);
    auto account = bank->openAccount("Иван", "BYN", 100);
    CHECK(account->getNumber() == "BYTEST10001");
    CHECK(bank->findAccount("BYTEST10001") == account);
    CHECK_THROWS_AS(bank->findAccount("NOPE"), AccountNotFoundException);
    CHECK_THROWS_AS(bank->addAccount(account), AccountNotFoundException);
    CHECK_THROWS_AS(bank->addAccount(nullptr), AccountNotFoundException);
    auto holder = std::make_shared<CardHolder>("Иван", "MP1", "+375291234567");
    auto card = testdata::debitCard(account);
    CHECK_THROWS_AS(bank->issueCard(nullptr, holder), InvalidCardException);
    CHECK_THROWS_AS(bank->issueCard(testdata::debitCard(testdata::account(), testdata::CARD_2), holder),
                    AccountNotFoundException);
    bank->issueCard(card, holder);
    CHECK_THROWS_AS(bank->issueCard(card, holder), InvalidCardException);
    CHECK(bank->isOwnCard(testdata::CARD_1));
    CHECK_FALSE(bank->isOwnCard(testdata::CARD_2));
    CHECK(bank->findCard(testdata::CARD_1) == card);
    CHECK_THROWS_AS(bank->findCard(testdata::CARD_2), InvalidCardException);
    CHECK(bank->findHolderOf(testdata::CARD_1) == holder);
    CHECK(bank->findHolderOf(testdata::CARD_2) == nullptr);
    bank->addClient(holder);
    CHECK(bank->getClients().size() == 1);
    bank->blockCard(testdata::CARD_1);
    CHECK(card->isBlocked());
    CHECK(bank->getName() == "Тест-Банк");
    CHECK(bank->getBic() == "TEST");
}

TEST_CASE("Bank: переводы с конвертацией") {
    auto bank = testdata::bank();
    auto ivan = bank->openAccount("Иван", "BYN", 1000);
    auto anna = bank->openAccount("Анна", "BYN", 0);
    auto dollars = bank->openAccount("Анна", "USD", 10);
    CHECK(bank->transfer(ivan->getNumber(), anna->getNumber(), 100) == Approx(100));
    CHECK(anna->getBalance() == Approx(100));
    CHECK(bank->transfer(ivan->getNumber(), dollars->getNumber(), 300) == Approx(99));
    CHECK(dollars->getBalance() == Approx(109));
    CHECK(ivan->getBalance() == Approx(600));
    dollars->freeze();
    CHECK_THROWS_AS(bank->transfer(ivan->getNumber(), dollars->getNumber(), 30), AccountFrozenException);
    CHECK(ivan->getBalance() == Approx(600));
    CHECK(bank->receiveTransfer(anna->getNumber(), 50, "BYN", "Пётр") == Approx(50));
    CHECK(bank->totalDepositsInByn() == Approx(600 + 150 + 109 * 3));
    CHECK(bank->getExchange().getOperationsCount() == 1);
}

TEST_CASE("FraudDetector: крупные суммы и серии операций") {
    CHECK_THROWS_AS(FraudDetector(0, 3), InvalidAmountException);
    FraudDetector detector(1000, 3);
    CHECK(detector.isSuspicious(1500));
    CHECK_FALSE(detector.isSuspicious(500));
    CHECK_THROWS_AS(detector.check("*0010", 1500), FraudDetectedException);
    detector.check("*0010", 800);
    detector.check("*0010", 800);
    CHECK(detector.recentTotal() == Approx(1600));
    CHECK_THROWS_AS(detector.check("*0010", 500), FraudDetectedException);
    CHECK(detector.getAlerts().size() == 2);
    detector.reset();
    CHECK(detector.recentTotal() == Approx(0));
    for (int index = 0; index < 5; ++index) {
        detector.record(100);
    }
    CHECK(detector.recentTotal() == Approx(300));
    CHECK(detector.getLargeAmountThreshold() == Approx(1000));
}

TEST_CASE("ProcessingCenter: одобрение и отказ") {
    CHECK_THROWS_AS(ProcessingCenter(nullptr, FraudDetector(1000, 3)), AccountNotFoundException);
    auto bank = testdata::bank();
    auto holder = std::make_shared<CardHolder>("Иван", "MP1", "+375291234567");
    auto card = testdata::debitCard(bank->openAccount("Иван", "BYN", 500));
    bank->issueCard(card, holder);
    ProcessingCenter processing(bank, FraudDetector(1000, 3));
    CHECK(processing.approvalRate() == Approx(100));
    processing.authorize(*card, 100, 11, 2026);
    CHECK_THROWS_AS(processing.authorize(*card, 600, 11, 2026), InsufficientFundsException);
    CHECK_THROWS_AS(processing.authorize(*card, 100, 1, 2030), CardExpiredException);
    CHECK(processing.getApprovedCount() == 1);
    CHECK(processing.getDeclinedCount() == 2);
    CHECK(processing.approvalRate() == Approx(100.0 / 3));
    CHECK_FALSE(processing.isForeignCard(*card));
    CHECK(processing.isForeignCard(*testdata::debitCard(testdata::account(), testdata::CARD_2)));
    CHECK(processing.getFraudDetector().recentTotal() == Approx(100));
    processing.resetStatistics();
    CHECK(processing.getDeclinedCount() == 0);
}

TEST_CASE("SmsNotifier: оповещения") {
    CHECK_THROWS_AS(SmsNotifier(-1), InvalidAmountException);
    SmsNotifier notifier(0.05);
    CardHolder ivan("Иван", "MP1", "+375291234567");
    CardHolder noPhone("Пётр", "MP2", "123");
    Withdrawal withdrawal(1, "**** 0010", 100, 0, false);
    CHECK(notifier.notify(ivan, withdrawal));
    CHECK_FALSE(notifier.notify(noPhone, withdrawal));
    notifier.disable();
    CHECK_FALSE(notifier.isEnabled());
    CHECK_FALSE(notifier.notify(ivan, withdrawal));
    notifier.enable();
    CHECK(notifier.notify(ivan, withdrawal));
    CHECK(notifier.getSentMessages().front() == "+375291234567: Снятие наличных: 100.00 BYN, карта **** 0010");
    CHECK(notifier.totalCost() == Approx(0.1));
    CHECK(notifier.messagesTo("+375291234567") == 2);
    CHECK(notifier.messagesTo("+375440000000") == 0);
}

TEST_CASE("Employee: доступ и смена") {
    CHECK_THROWS_AS(CashCollector("", "C-01", "2468"), AccessDeniedException);
    CashCollector collector("Сергей", "C-01", "2468");
    CHECK(collector.role() == "Инкассатор");
    CHECK(collector.badge() == "Инкассатор Сергей (таб. № C-01)");
    CHECK_THROWS_AS(collector.verifyAccess("0000"), AccessDeniedException);
    CHECK_THROWS_AS(collector.startShift("0000"), AccessDeniedException);
    CHECK_THROWS_AS(collector.requireOnDuty(), AccessDeniedException);
    collector.startShift("2468");
    CHECK(collector.isOnDuty());
    collector.requireOnDuty();
    collector.endShift();
    CHECK_FALSE(collector.isOnDuty());
    CHECK(collector.getEmployeeId() == "C-01");
}

TEST_CASE("EncashmentReport, MaintenanceRecord, MaintenanceLog") {
    EncashmentReport report("ATM-1", "Сергей", 3);
    CHECK_THROWS_AS(report.addLoaded(-1), InvalidAmountException);
    CHECK_THROWS_AS(report.addCollected(-1), InvalidAmountException);
    report.addLoaded(1000);
    report.addCollected(300);
    CHECK(report.netChange() == Approx(700));
    CHECK(report.summary() ==
          "Инкассация банкомата ATM-1, день 3, инкассатор Сергей: загружено 1000.00, забрано 300.00 BYN");
    CHECK(report.getAtmId() == "ATM-1");
    CHECK(report.getCollectorName() == "Сергей");
    CHECK(report.getDay() == 3);
    CHECK_THROWS_AS(MaintenanceRecord(-1, "Экран", "x", "Пётр", 0), InvalidAmountException);
    MaintenanceRecord expensive(10, "Экран", "Замена матрицы", "Пётр", 150);
    CHECK(expensive.isExpensive());
    CHECK(expensive.daysSince(15) == 5);
    CHECK(expensive.summary() == "День 10: Экран — Замена матрицы (Пётр, 150.00 BYN)");
    CHECK_THROWS_AS(MaintenanceLog("ATM-1", 0), InvalidAmountException);
    MaintenanceLog log("ATM-1", 30);
    CHECK(log.isServiceDue(30));
    log.addRecord(expensive);
    log.addRecord(MaintenanceRecord(5, "Принтер чеков", "Бумага", "Пётр", 20));
    CHECK(log.getLastServiceDay() == 10);
    CHECK_FALSE(log.isServiceDue(39));
    CHECK(log.isServiceDue(40));
    CHECK(log.totalCost() == Approx(170));
    CHECK(log.recordsFor("Экран").size() == 1);
    CHECK(log.recordsFor("Камера").empty());
    CHECK(log.getAtmId() == "ATM-1");
    CHECK(log.getRecords().size() == 2);
}

TEST_CASE("AtmNetwork: поиск и контроль наличных") {
    CHECK_THROWS_AS(AtmNetwork("Банк", -1), InvalidAmountException);
    AtmNetwork network("Тест-Банк", 5000);
    auto first = testdata::atmSetup().atm;
    auto bank = testdata::bank();
    auto second = std::make_shared<Atm>("ATM-B", Address("Брест", "ул. Советская", "1"), bank,
                                        FeePolicy(0, 1, 1, 0), "1111");
    CHECK_THROWS_AS(network.addAtm(nullptr), DeviceException);
    network.addAtm(first);
    network.addAtm(second);
    CHECK_THROWS_AS(network.addAtm(first), DeviceException);
    CHECK(network.findById("ATM-B") == second);
    CHECK_THROWS_AS(network.findById("ATM-X"), DeviceException);
    CHECK(network.atmsInCity("Минск").size() == 1);
    CHECK(network.needingEncashment().size() == 1);
    CHECK(network.totalCash() == 7600);
    CHECK(network.operationalCount() == 1);
    CHECK(network.getBankName() == "Тест-Банк");
    CHECK(network.getAtms().size() == 2);
}
