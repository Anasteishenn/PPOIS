#include "TestData.h"
#include "cards/CardHolder.h"
#include "cards/CreditCard.h"
#include "cards/PrepaidCard.h"
#include "doctest.h"
#include "exceptions/AtmException.h"

using doctest::Approx;

TEST_CASE("PinCode: формат, проверка, блокировка") {
    CHECK_THROWS_AS(PinCode("12a4"), InvalidPinException);
    CHECK_THROWS_AS(PinCode("123"), InvalidPinException);
    CHECK_THROWS_AS(PinCode("1234", 0), InvalidPinException);
    CHECK(PinCode::isValidFormat("0000"));
    CHECK_FALSE(PinCode::isValidFormat("12345"));
    PinCode pin("1234");
    CHECK(pin.matches("1234"));
    CHECK_FALSE(pin.matches("4321"));
    CHECK_THROWS_AS(pin.verify("1111"), InvalidPinException);
    CHECK(pin.attemptsLeft() == 2);
    pin.verify("1234");
    CHECK(pin.getFailedAttempts() == 0);
    CHECK_THROWS_AS(pin.verify("1111"), InvalidPinException);
    CHECK_THROWS_AS(pin.verify("1111"), InvalidPinException);
    CHECK_THROWS_AS(pin.verify("1111"), PinLockedException);
    CHECK(pin.isLocked());
    CHECK(pin.attemptsLeft() == 0);
    CHECK_THROWS_AS(pin.verify("1234"), PinLockedException);
    pin.reset();
    CHECK_FALSE(pin.isLocked());
    CHECK(pin.getMaxAttempts() == PinCode::DEFAULT_MAX_ATTEMPTS);
}

TEST_CASE("PinCode: смена PIN") {
    PinCode pin("1234");
    CHECK_THROWS_AS(pin.change("0000", "5678"), InvalidPinException);
    CHECK_THROWS_AS(pin.change("1234", "56"), InvalidPinException);
    CHECK_THROWS_AS(pin.change("1234", "1234"), InvalidPinException);
    pin.change("1234", "5678");
    CHECK(pin.matches("5678"));
    CHECK_FALSE(pin.matches("1234"));
}

TEST_CASE("CardLimit: дневной лимит и число операций") {
    CHECK_THROWS_AS(CardLimit(0), InvalidAmountException);
    CardLimit limit(1000, 2);
    CHECK(limit.canWithdraw(500));
    limit.registerWithdrawal(600);
    CHECK(limit.remaining() == Approx(400));
    CHECK_THROWS_AS(limit.registerWithdrawal(500), LimitExceededException);
    limit.registerWithdrawal(400);
    CHECK(limit.getOperationsToday() == 2);
    CHECK_FALSE(limit.canWithdraw(0));
    CHECK_THROWS_AS(limit.registerWithdrawal(0), LimitExceededException);
    limit.resetDay();
    CHECK(limit.getWithdrawnToday() == Approx(0));
    CHECK(limit.canWithdraw(1000));
    CHECK_THROWS_AS(limit.changeLimit(0), InvalidAmountException);
    limit.changeLimit(2000);
    CHECK(limit.getDailyLimit() == Approx(2000));
    CHECK(limit.getMaxOperationsPerDay() == 2);
}

TEST_CASE("CashbackProgram: проценты по категориям и потолок") {
    CHECK_THROWS_AS(CashbackProgram(-1, 5, 30), InvalidAmountException);
    CashbackProgram cashback(1, 5, 30);
    cashback.addBonusCategory("Кафе");
    CHECK(cashback.isBonusCategory("Кафе"));
    CHECK(cashback.percentFor("Кафе") == Approx(5));
    CHECK(cashback.percentFor("Магазин") == Approx(1));
    CHECK(cashback.calculate(100, "Кафе", 0) == Approx(5));
    CHECK(cashback.calculate(1000, "Кафе", 0) == Approx(30));
    CHECK(cashback.calculate(100, "Магазин", 29.5) == Approx(0.5));
    CHECK(cashback.calculate(100, "Магазин", 30) == Approx(0));
    CHECK(cashback.calculate(-5, "Кафе", 0) == Approx(0));
    CHECK(cashback.getBasePercent() == Approx(1));
    CHECK(cashback.getBonusPercent() == Approx(5));
    CHECK(cashback.getMonthlyCap() == Approx(30));
}

TEST_CASE("Card: номер по алгоритму Луна и данные карты") {
    auto account = testdata::account();
    CHECK(Card::isValidNumber(testdata::CARD_1));
    CHECK_FALSE(Card::isValidNumber("4000000000000011"));
    CHECK_FALSE(Card::isValidNumber("40000000000000a0"));
    CHECK_FALSE(Card::isValidNumber("1234"));
    CHECK_THROWS_AS(testdata::debitCard(account, "4000000000000011"), InvalidCardException);
    CashbackProgram cashback(0, 0, 0);
    CHECK_THROWS_AS(DebitCard(testdata::CARD_1, "", 1, 2030, "1234", account, cashback), InvalidCardException);
    CHECK_THROWS_AS(DebitCard(testdata::CARD_1, "I", 13, 2030, "1234", account, cashback), InvalidCardException);
    CHECK_THROWS_AS(DebitCard(testdata::CARD_1, "I", 1, 2030, "1234", nullptr, cashback), InvalidCardException);
    auto card = testdata::debitCard(account);
    CHECK(card->maskedNumber() == "**** **** **** 0010");
    CHECK(card->getHolderName() == "IVAN PETROV");
    CHECK(card->getExpiryMonth() == 12);
    CHECK(card->getExpiryYear() == 2028);
    CHECK(card->getPin().matches("1234"));
}

TEST_CASE("Card: срок действия, блокировка, PIN") {
    auto card = testdata::debitCard(testdata::account());
    CHECK_FALSE(card->isExpired(12, 2028));
    CHECK_FALSE(card->isExpired(1, 2028));
    CHECK(card->isExpired(1, 2029));
    CHECK_THROWS_AS(card->validate(1, 2029), CardExpiredException);
    card->validate(11, 2026);
    card->block();
    CHECK_THROWS_AS(card->validate(11, 2026), CardBlockedException);
    CHECK_THROWS_AS(card->authenticate("1234"), CardBlockedException);
    card->unblock();
    card->authenticate("1234");
    CHECK_THROWS_AS(card->authenticate("0000"), InvalidPinException);
    CHECK_THROWS_AS(card->authenticate("0000"), InvalidPinException);
    CHECK_THROWS_AS(card->authenticate("0000"), PinLockedException);
    CHECK(card->isBlocked());
    card->unblock();
    CHECK_FALSE(card->isBlocked());
    card->changePin("1234", "4321");
    CHECK(card->getPin().matches("4321"));
    CHECK_THROWS_AS(card->changePin("0000", "1111"), InvalidPinException);
    CHECK_THROWS_AS(card->changePin("0000", "1111"), InvalidPinException);
    CHECK_THROWS_AS(card->changePin("0000", "1111"), PinLockedException);
    CHECK(card->isBlocked());
}

TEST_CASE("Card: деньги и лимиты") {
    auto account = testdata::account(2000);
    auto card = testdata::debitCard(account);
    CHECK(card->availableFunds() == Approx(2000));
    card->charge(100, "Покупка");
    CHECK(account->getBalance() == Approx(1900));
    card->topUp(50, "Пополнение");
    CHECK(account->getBalance() == Approx(1950));
    card->registerCashWithdrawal(900);
    CHECK_THROWS_AS(card->registerCashWithdrawal(200), LimitExceededException);
    card->startNewDay();
    card->registerCashWithdrawal(200);
    CHECK(card->getLimit().getWithdrawnToday() == Approx(200));
    CHECK(card->getAccount() == account);
}

TEST_CASE("DebitCard: кэшбэк") {
    auto account = testdata::account(100);
    auto card = testdata::debitCard(account);
    CHECK(card->type() == "Дебетовая");
    CHECK(card->transferCashbackToAccount() == Approx(0));
    CHECK(card->earnCashback(100, "Кафе") == Approx(1));
    CHECK_FALSE(card->getCashbackProgram().isBonusCategory("Кафе"));
    DebitCard bonusCard(testdata::CARD_2, "I", 1, 2030, "1234", account, CashbackProgram(1, 5, 30));
    CHECK(bonusCard.earnCashback(200, "Магазин") == Approx(2));
    CHECK(bonusCard.getCashbackEarned() == Approx(2));
    CHECK(bonusCard.transferCashbackToAccount() == Approx(2));
    CHECK(account->getBalance() == Approx(102));
    CHECK(bonusCard.getCashbackEarned() == Approx(0));
}

TEST_CASE("CreditCard: свои деньги, кредит, проценты, погашение") {
    auto account = testdata::account(100);
    CHECK_THROWS_AS(CreditCard(testdata::CARD_4, "A", 1, 2030, "1234", account, -1, 2), InvalidAmountException);
    CreditCard card(testdata::CARD_4, "ANNA", 1, 2030, "1234", account, 500, 2);
    CHECK(card.type() == "Кредитная");
    CHECK(card.availableFunds() == Approx(600));
    CHECK_THROWS_AS(card.charge(0, "x"), InvalidAmountException);
    CHECK_THROWS_AS(card.charge(700, "x"), InsufficientFundsException);
    card.charge(300, "Покупка");
    CHECK(account->getBalance() == Approx(0));
    CHECK(card.getDebt() == Approx(200));
    CHECK(card.availableFunds() == Approx(300));
    card.charge(100, "Покупка");
    CHECK(card.getDebt() == Approx(300));
    CHECK(card.monthlyInterest() == Approx(6));
    CHECK(card.minimumPayment() == Approx(15));
    card.accrueInterest();
    CHECK(card.getDebt() == Approx(306));
    CHECK_THROWS_AS(card.repayDebt(0), InvalidAmountException);
    CHECK_THROWS_AS(card.repayDebt(400), InvalidAmountException);
    card.topUp(100, "Пополнение");
    card.repayDebt(100);
    CHECK(card.getDebt() == Approx(206));
    CHECK(account->getBalance() == Approx(0));
    CHECK(card.getCreditLimit() == Approx(500));
    CHECK(card.getMonthlyRatePercent() == Approx(2));
}

TEST_CASE("PrepaidCard: пополнение и остаток") {
    auto account = testdata::account(0);
    PrepaidCard gift(testdata::CARD_2, "GIFT", 1, 2030, "1234", account, false);
    CHECK(gift.type() == "Предоплаченная");
    CHECK(gift.isEmpty());
    CHECK_FALSE(gift.isRechargeable());
    CHECK_THROWS_AS(gift.recharge(50), InvalidCardException);
    PrepaidCard rechargeable(testdata::CARD_3, "GIFT", 1, 2030, "1234", account, true);
    rechargeable.recharge(50);
    CHECK(rechargeable.getRechargeCount() == 1);
    CHECK_FALSE(rechargeable.isEmpty());
}

TEST_CASE("CardHolder: карты клиента") {
    CHECK_THROWS_AS(CardHolder("", "MP1", "+375291234567"), InvalidCardException);
    CardHolder holder("Иван Петров", "MP1234567", "+375291234567");
    CHECK_THROWS_AS(holder.addCard(nullptr), InvalidCardException);
    auto first = testdata::debitCard(testdata::account(300));
    auto second = testdata::debitCard(testdata::account(200), testdata::CARD_2);
    holder.addCard(first);
    holder.addCard(second);
    CHECK_THROWS_AS(holder.addCard(first), InvalidCardException);
    CHECK(holder.findCard(testdata::CARD_2) == second);
    CHECK_THROWS_AS(holder.findCard(testdata::CARD_3), InvalidCardException);
    CHECK(holder.totalAvailable() == Approx(500));
    second->block();
    CHECK(holder.totalAvailable() == Approx(300));
    CHECK(holder.activeCardsCount() == 1);
    holder.removeCard(testdata::CARD_2);
    CHECK(holder.getCards().size() == 1);
    CHECK_THROWS_AS(holder.removeCard(testdata::CARD_2), InvalidCardException);
    CHECK(holder.hasValidPhone());
    CHECK_FALSE(CardHolder("А", "1", "80291234567").hasValidPhone());
    CHECK_FALSE(CardHolder("А", "1", "+37529123456a").hasValidPhone());
    CHECK_FALSE(holder.upgradeToVip(10000));
    CHECK(holder.upgradeToVip(100));
    CHECK(holder.isVip());
    CHECK(holder.getFullName() == "Иван Петров");
    CHECK(holder.getPassportNumber() == "MP1234567");
    CHECK(holder.getPhone() == "+375291234567");
}
