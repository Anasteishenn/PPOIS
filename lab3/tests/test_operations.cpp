#include <memory>

#include "TestData.h"
#include "doctest.h"
#include "exceptions/AtmException.h"
#include "operations/BalanceInquiry.h"
#include "operations/Deposit.h"
#include "operations/ServicePayment.h"
#include "operations/Transfer.h"
#include "operations/Withdrawal.h"

using doctest::Approx;

namespace {
const std::string MASK = "**** **** **** 0010";

ServiceProvider mobile() {
    return ServiceProvider("МТС", "Мобильная связь", 9, 1.0);
}
}  // namespace

TEST_CASE("Transaction: статусы и описание") {
    CHECK_THROWS_AS(Withdrawal(1, MASK, -10, 0, false), InvalidAmountException);
    CHECK_THROWS_AS(Withdrawal(1, MASK, 10, -1, false), InvalidAmountException);
    Withdrawal withdrawal(1, MASK, 100, 2, true);
    CHECK(withdrawal.statusName() == "В обработке");
    CHECK(withdrawal.total() == Approx(102));
    CHECK(withdrawal.describe() == "Снятие наличных: 100.00 BYN (комиссия 2.00)");
    withdrawal.complete();
    CHECK(withdrawal.isSuccessful());
    CHECK(withdrawal.statusName() == "Успешно");
    CHECK_THROWS_AS(withdrawal.complete(), TransactionException);
    CHECK_THROWS_AS(withdrawal.fail("x"), TransactionException);
    Withdrawal failed(2, MASK, 50, 0, false);
    failed.fail("Нет денег");
    CHECK_FALSE(failed.isSuccessful());
    CHECK(failed.statusName() == "Отклонено");
    CHECK(failed.getFailReason() == "Нет денег");
    CHECK(failed.getStatus() == TransactionStatus::Failed);
    CHECK(failed.getId() == 2);
    CHECK(failed.getCardMask() == MASK);
    CHECK(failed.getFee() == Approx(0));
}

TEST_CASE("Withdrawal: купюры") {
    Withdrawal withdrawal(1, MASK, 250, 0, false);
    CHECK(withdrawal.describe() == "Снятие наличных: 250.00 BYN");
    CHECK_FALSE(withdrawal.isForeignCard());
    withdrawal.recordNotes({{100, 2}, {50, 1}});
    CHECK(withdrawal.notesCount() == 3);
    CHECK(withdrawal.notesDescription() == "2 x 100, 1 x 50");
    CHECK(withdrawal.describe() == "Снятие наличных: 250.00 BYN, купюры: 2 x 100, 1 x 50");
    CHECK(withdrawal.getNotes().size() == 2);
    CHECK(withdrawal.type() == "Снятие наличных");
}

TEST_CASE("Deposit, Transfer, BalanceInquiry: описания") {
    Deposit deposit(1, MASK, 150, 2, 1);
    CHECK(deposit.type() == "Внесение наличных");
    CHECK(deposit.hasRejected());
    CHECK(deposit.describe() == "Внесение наличных: 150.00 BYN, принято купюр: 2, возвращено: 1");
    Deposit clean(2, MASK, 100, 1, 0);
    CHECK_FALSE(clean.hasRejected());
    CHECK(clean.describe() == "Внесение наличных: 100.00 BYN, принято купюр: 1");
    CHECK(clean.getAcceptedNotes() == 1);
    CHECK(clean.getRejectedNotes() == 0);
    Transfer transfer(3, MASK, 100, 0.5, "BY01", "Анна");
    CHECK(transfer.type() == "Перевод");
    CHECK(transfer.describe() == "Перевод: 100.00 BYN (комиссия 0.50) -> Анна (счёт BY01)");
    CHECK(transfer.isToOwner("Анна"));
    CHECK_FALSE(transfer.isToOwner("Иван"));
    CHECK(transfer.getTargetAccount() == "BY01");
    CHECK(transfer.getTargetOwner() == "Анна");
    BalanceInquiry inquiry(4, MASK, 500, "BYN");
    CHECK(inquiry.type() == "Запрос баланса");
    CHECK(inquiry.describe() == "Запрос баланса: доступно 500.00 BYN");
    CHECK(inquiry.isEnoughFor(500));
    CHECK_FALSE(inquiry.isEnoughFor(501));
    CHECK(inquiry.getBalance() == Approx(500));
    CHECK(inquiry.getCurrencyCode() == "BYN");
}

TEST_CASE("ServiceProvider и ServicePayment") {
    CHECK_THROWS_AS(ServiceProvider("", "x", 9, 0), InvalidAmountException);
    CHECK_THROWS_AS(ServiceProvider("A", "x", 0, 0), InvalidAmountException);
    CHECK_THROWS_AS(ServiceProvider("A", "x", 9, 6), InvalidAmountException);
    ServiceProvider provider = mobile();
    CHECK(provider.isValidAccount("291234567"));
    CHECK_FALSE(provider.isValidAccount("29123456"));
    CHECK_FALSE(provider.isValidAccount("29123456a"));
    CHECK(provider.commissionFor(200) == Approx(2));
    CHECK(provider.label() == "МТС (Мобильная связь)");
    CHECK(provider.getName() == "МТС");
    CHECK(provider.getCategory() == "Мобильная связь");
    CHECK(provider.getAccountLength() == 9);
    CHECK(provider.getCommissionPercent() == Approx(1));
    CHECK_THROWS_AS(ServicePayment(1, MASK, 10, 0, provider, "123"), AccountNotFoundException);
    ServicePayment payment(1, MASK, 10, 0.1, provider, "291234567");
    CHECK(payment.type() == "Оплата услуг");
    CHECK(payment.describe() ==
          "Оплата услуг: 10.00 BYN (комиссия 0.10), МТС (Мобильная связь), лицевой счёт 291234567");
    CHECK(payment.isCategory("Мобильная связь"));
    CHECK_FALSE(payment.isCategory("Интернет"));
    CHECK(payment.getProvider().getName() == "МТС");
    CHECK(payment.getPayerAccount() == "291234567");
}

TEST_CASE("UtilityBill: просрочка, пеня, оплата") {
    ServiceProvider utilities("Энерго", "Коммунальные услуги", 10, 0);
    CHECK_THROWS_AS(UtilityBill(utilities, "123", 50, 25), AccountNotFoundException);
    CHECK_THROWS_AS(UtilityBill(utilities, "1234567890", 0, 25), InvalidAmountException);
    UtilityBill bill(utilities, "1234567890", 100, 25);
    CHECK_FALSE(bill.isOverdue(25));
    CHECK(bill.penalty(20) == Approx(0));
    CHECK(bill.isOverdue(30));
    CHECK(bill.penalty(30) == Approx(0.5));
    CHECK(bill.totalToPay(30) == Approx(100.5));
    CHECK_THROWS_AS(bill.markPaid(100, 30), InvalidAmountException);
    bill.markPaid(100.5, 30);
    CHECK(bill.isPaid());
    CHECK_FALSE(bill.isOverdue(30));
    CHECK_THROWS_AS(bill.markPaid(200, 30), TransactionException);
    CHECK(bill.getAccountNumber() == "1234567890");
    CHECK(bill.getAmountDue() == Approx(100));
    CHECK(bill.getDueDay() == 25);
    CHECK(bill.getProvider().getName() == "Энерго");
}

TEST_CASE("Receipt: текст чека") {
    Receipt receipt("ATM-1", 7, MASK);
    Withdrawal withdrawal(7, MASK, 100, 0, false);
    withdrawal.fail("Нет денег");
    receipt.addTransaction(withdrawal);
    receipt.addLine("Хорошего дня");
    CHECK(receipt.lineCount() == 4);
    CHECK(receipt.getLines()[2] == "Статус: Отклонено (Нет денег)");
    std::string text = receipt.text();
    CHECK(text.find("Банкомат: ATM-1") != std::string::npos);
    CHECK(text.find("Операция №7") != std::string::npos);
    CHECK(text.find("Итого списано: 100.00 BYN") != std::string::npos);
    Withdrawal success(8, MASK, 50, 0, false);
    success.complete();
    Receipt second("ATM-1", 8, MASK);
    second.addTransaction(success);
    CHECK(second.getLines()[2] == "Статус: Успешно");
    CHECK(second.getAtmId() == "ATM-1");
    CHECK(second.getTransactionId() == 8);
    CHECK(second.getCardMask() == MASK);
}

TEST_CASE("TransactionLog: номера, поиск, статистика") {
    TransactionLog log;
    CHECK(log.nextId() == 1);
    CHECK(log.nextId() == 2);
    CHECK_THROWS_AS(log.record(nullptr), TransactionException);
    CHECK_THROWS_AS(log.lastTransaction(), TransactionException);
    auto withdrawal = std::make_shared<Withdrawal>(1, MASK, 100, 0, false);
    withdrawal->complete();
    auto failed = std::make_shared<Withdrawal>(2, MASK, 500, 0, false);
    failed->fail("Лимит");
    auto deposit = std::make_shared<Deposit>(3, "**** **** **** 0028", 50, 1, 0);
    log.record(withdrawal);
    log.record(failed);
    log.record(deposit);
    CHECK(log.findById(2) == failed);
    CHECK_THROWS_AS(log.findById(99), TransactionException);
    CHECK(log.lastTransaction() == deposit);
    CHECK(log.totalWithdrawn() == Approx(100));
    CHECK(log.countByType("Снятие наличных") == 2);
    CHECK(log.failedCount() == 1);
    CHECK(log.forCard(MASK).size() == 2);
    CHECK(log.getTransactions().size() == 3);
}

TEST_CASE("FeePolicy: комиссии") {
    CHECK_THROWS_AS(FeePolicy(-1, 0, 0, 0), InvalidAmountException);
    FeePolicy fees(0.5, 1.5, 2.0, 0.5);
    CHECK(fees.withdrawalFee(1000, true) == Approx(0.5));
    CHECK(fees.withdrawalFee(1000, false) == Approx(15));
    CHECK(fees.withdrawalFee(50, false) == Approx(2));
    CHECK(fees.transferFee(200) == Approx(1));
    CHECK(fees.paymentFee(200, mobile()) == Approx(2));
    CHECK(fees.describe().find("чужой картой: 1.50%") != std::string::npos);
    CHECK(fees.getOwnBankFee() == Approx(0.5));
    CHECK(fees.getForeignPercent() == Approx(1.5));
    CHECK(fees.getMinForeignFee() == Approx(2));
    CHECK(fees.getTransferPercent() == Approx(0.5));
}

TEST_CASE("Session: PIN, тайм-аут, закрытие") {
    CHECK_THROWS_AS(Session(nullptr), SessionException);
    auto card = testdata::debitCard(testdata::account());
    CHECK_THROWS_AS(Session(card, 0), SessionException);
    Session session(card, 60);
    CHECK(session.getCard() == card);
    CHECK_THROWS_AS(session.requireAuthenticated(), SessionException);
    CHECK_THROWS_AS(session.registerOperation(), SessionException);
    CHECK_THROWS_AS(session.authenticate("0000"), InvalidPinException);
    session.authenticate("1234");
    CHECK(session.isAuthenticated());
    session.tick(-5);
    session.tick(30);
    CHECK(session.getSecondsIdle() == 30);
    session.registerOperation();
    CHECK(session.getOperationsCount() == 1);
    CHECK(session.getSecondsIdle() == 0);
    session.tick(60);
    CHECK(session.isExpired());
    CHECK_FALSE(session.isActive());
    CHECK_THROWS_AS(session.requireAuthenticated(), SessionException);
    Session closed(card);
    closed.close();
    CHECK(closed.isClosed());
    CHECK_FALSE(closed.isActive());
    CHECK_THROWS_AS(closed.authenticate("1234"), SessionException);
}
