#include "TestData.h"
#include "accounts/AccountStatement.h"
#include "accounts/CurrencyExchange.h"
#include "accounts/SavingsAccount.h"
#include "doctest.h"
#include "exceptions/AtmException.h"

using doctest::Approx;

TEST_CASE("Currency: проверка, пересчёт, формат") {
    CHECK_THROWS_AS(Currency("US", "$", 3.0), CurrencyException);
    CHECK_THROWS_AS(Currency("USD", "$", 0.0), CurrencyException);
    Currency dollar = testdata::usd();
    CHECK(dollar.getCode() == "USD");
    CHECK(dollar.getSymbol() == "$");
    CHECK(dollar.toByn(10) == Approx(30));
    CHECK(dollar.fromByn(30) == Approx(10));
    CHECK(dollar.format(12.5) == "12.50 $");
    CHECK_THROWS_AS(dollar.updateRate(-1), CurrencyException);
    dollar.updateRate(3.5);
    CHECK(dollar.getRateToByn() == Approx(3.5));
    CHECK(dollar.isSame(testdata::usd()));
    CHECK_FALSE(dollar.isSame(testdata::byn()));
}

TEST_CASE("CurrencyExchange: таблица валют и конвертация с комиссией") {
    CHECK_THROWS_AS(CurrencyExchange(-1), CurrencyException);
    CHECK_THROWS_AS(CurrencyExchange(11), CurrencyException);
    CurrencyExchange exchange(1.0);
    exchange.addCurrency(testdata::byn());
    exchange.addCurrency(testdata::usd());
    CHECK(exchange.supports("USD"));
    CHECK_FALSE(exchange.supports("EUR"));
    CHECK_THROWS_AS(exchange.findCurrency("EUR"), CurrencyException);
    CHECK(exchange.commissionFor(100) == Approx(1));
    CHECK(exchange.convert(100, "USD", "BYN") == Approx(297));
    CHECK(exchange.convert(300, "BYN", "USD") == Approx(99));
    CHECK(exchange.getOperationsCount() == 2);
    CHECK(exchange.convert(50, "BYN", "BYN") == Approx(50));
    CHECK(exchange.getOperationsCount() == 2);
    CHECK_THROWS_AS(exchange.convert(0, "BYN", "USD"), InvalidAmountException);
    exchange.addCurrency(Currency("USD", "$", 3.5));
    CHECK(exchange.findCurrency("USD").getRateToByn() == Approx(3.5));
    CHECK(exchange.getCommissionPercent() == Approx(1.0));
}

TEST_CASE("StatementEntry: поступление и списание") {
    StatementEntry income(1, "Пополнение", 100, 600);
    CHECK(income.isIncome());
    CHECK_FALSE(income.isExpense());
    CHECK(income.format() == "#1 Пополнение: +100.00 (остаток 600.00)");
    StatementEntry expense(2, "Снятие", -50, 550);
    CHECK(expense.isExpense());
    CHECK(expense.format() == "#2 Снятие: -50.00 (остаток 550.00)");
    CHECK(expense.getNumber() == 2);
    CHECK(expense.getDescription() == "Снятие");
    CHECK(expense.getBalanceAfter() == Approx(550));
}

TEST_CASE("AccountStatement: ограничение размера и итоги") {
    CHECK_THROWS_AS(AccountStatement("A", 0), InvalidAmountException);
    AccountStatement statement("A", 3);
    statement.addEntry("Пополнение", 100, 100);
    statement.addEntry("Снятие", -30, 70);
    statement.addEntry("Пополнение", 50, 120);
    statement.addEntry("Снятие", -20, 100);
    CHECK(statement.getEntries().size() == 3);
    CHECK(statement.getEntries().front().getNumber() == 2);
    CHECK(statement.getTotalAdded() == 4);
    CHECK(statement.lastEntries(2).size() == 2);
    CHECK(statement.lastEntries(2).back().getNumber() == 4);
    CHECK(statement.lastEntries(0).empty());
    CHECK(statement.lastEntries(10).size() == 3);
    CHECK(statement.totalIncome() == Approx(50));
    CHECK(statement.totalExpense() == Approx(50));
    CHECK(statement.getAccountNumber() == "A");
}

TEST_CASE("BankAccount: пополнение, снятие, заморозка, перевод") {
    CHECK_THROWS_AS(BankAccount("", "Иван", testdata::byn()), AccountNotFoundException);
    CHECK_THROWS_AS(BankAccount("A", "Иван", testdata::byn(), -1), InvalidAmountException);
    BankAccount account("A", "Иван", testdata::byn(), 100);
    CHECK(account.type() == "Текущий счёт");
    CHECK_THROWS_AS(account.deposit(0, "x"), InvalidAmountException);
    account.deposit(50, "Пополнение");
    CHECK(account.getBalance() == Approx(150));
    CHECK_THROWS_AS(account.withdraw(200, "x"), InsufficientFundsException);
    account.withdraw(30, "Снятие");
    CHECK(account.getBalance() == Approx(120));
    CHECK(account.canWithdraw(120));
    CHECK_FALSE(account.canWithdraw(0));
    account.freeze();
    CHECK(account.isFrozen());
    CHECK_FALSE(account.canWithdraw(10));
    CHECK_THROWS_AS(account.deposit(10, "x"), AccountFrozenException);
    account.unfreeze();
    CHECK(account.getStatement().getEntries().size() == 2);
    CHECK(account.getOwnerName() == "Иван");
    CHECK(account.getCurrency().getCode() == "BYN");
}

TEST_CASE("BankAccount: перевод между счетами") {
    BankAccount from("A", "Иван", testdata::byn(), 100);
    BankAccount to("B", "Анна", testdata::byn(), 0);
    BankAccount dollars("C", "Анна", testdata::usd(), 0);
    CHECK_THROWS_AS(from.transferTo(dollars, 10), CurrencyException);
    to.freeze();
    CHECK_THROWS_AS(from.transferTo(to, 10), AccountFrozenException);
    CHECK(from.getBalance() == Approx(100));
    to.unfreeze();
    from.transferTo(to, 40);
    CHECK(from.getBalance() == Approx(60));
    CHECK(to.getBalance() == Approx(40));
    CHECK(to.getStatement().getEntries().back().getDescription() == "Перевод со счёта A");
}

TEST_CASE("SavingsAccount: неснижаемый остаток и проценты") {
    CHECK_THROWS_AS(SavingsAccount("S", "Анна", testdata::byn(), 100, -1, 0), InvalidAmountException);
    SavingsAccount savings("S", "Анна", testdata::byn(), 1200, 12, 200);
    CHECK(savings.type() == "Сберегательный счёт");
    CHECK(savings.availableFunds() == Approx(1000));
    CHECK_THROWS_AS(savings.withdraw(1001, "x"), InsufficientFundsException);
    CHECK(savings.interestForMonths(1) == Approx(12));
    CHECK_THROWS_AS(savings.interestForMonths(0), InvalidAmountException);
    CHECK(savings.accrueMonthlyInterest() == Approx(12));
    CHECK(savings.getBalance() == Approx(1212));
    CHECK(savings.getYearRatePercent() == Approx(12));
    CHECK(savings.getMinBalance() == Approx(200));
    SavingsAccount noInterest("S2", "Анна", testdata::byn(), 100, 0, 200);
    CHECK(noInterest.availableFunds() == Approx(0));
    CHECK(noInterest.accrueMonthlyInterest() == Approx(0));
    CHECK(noInterest.getStatement().getEntries().empty());
}
