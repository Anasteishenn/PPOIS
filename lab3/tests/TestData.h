/**
 * @file TestData.h
 * @brief Готовые объекты для тестов: валюты, счета, карты, банк и банкомат.
 */
#pragma once

#include <memory>
#include <string>

#include "bank/Atm.h"
#include "bank/Bank.h"
#include "cards/DebitCard.h"
#include "doctest.h"

/// Как печатать умные указатели в сообщениях doctest (нужно для CHECK(a == b) с shared_ptr).
namespace doctest {
template <typename T>
struct StringMaker<std::shared_ptr<T>> {
    static String convert(const std::shared_ptr<T>& pointer) { return pointer ? "объект" : "nullptr"; }
};
}  // namespace doctest

namespace testdata {

// Номера карт, проходящие проверку по алгоритму Луна.
const std::string CARD_1 = "4000000000000010";
const std::string CARD_2 = "4000000000000028";
const std::string CARD_3 = "4000000000000036";
const std::string CARD_4 = "5100000000000040";
const std::string CARD_5 = "5200000000000056";
const std::string PIN = "1234";
const std::string SERVICE_CODE = "7777";

inline Currency byn() {
    return Currency("BYN", "р.", 1.0);
}

inline Currency usd() {
    return Currency("USD", "$", 3.0);
}

inline std::shared_ptr<BankAccount> account(double balance = 1000.0, const std::string& number = "ACC-1") {
    return std::make_shared<BankAccount>(number, "Иван Петров", byn(), balance);
}

inline std::shared_ptr<DebitCard> debitCard(std::shared_ptr<BankAccount> cardAccount,
                                            const std::string& number = CARD_1) {
    return std::make_shared<DebitCard>(number, "IVAN PETROV", 12, 2028, PIN, std::move(cardAccount),
                                       CashbackProgram(1.0, 5.0, 30.0));
}

inline std::shared_ptr<Bank> bank() {
    CurrencyExchange exchange(1.0);
    exchange.addCurrency(byn());
    exchange.addCurrency(usd());
    return std::make_shared<Bank>("Тест-Банк", "TEST", exchange);
}

/// Банк, клиент с картой (счёт BYTEST10001) и банкомат с наличными.
struct AtmSetup {
    std::shared_ptr<Bank> bank;
    std::shared_ptr<Atm> atm;
    std::shared_ptr<CardHolder> holder;
    std::shared_ptr<BankAccount> account;
    std::shared_ptr<DebitCard> card;
};

inline AtmSetup atmSetup(double balance = 1000.0) {
    AtmSetup setup;
    setup.bank = bank();
    setup.holder = std::make_shared<CardHolder>("Иван Петров", "MP1234567", "+375291234567");
    setup.account = setup.bank->openAccount("Иван Петров", "BYN", balance);
    setup.card = debitCard(setup.account);
    setup.bank->issueCard(setup.card, setup.holder);
    setup.atm = std::make_shared<Atm>("ATM-T", Address("Минск", "ул. Тестовая", "1"), setup.bank,
                                      FeePolicy(0.0, 1.5, 2.0, 0.5), SERVICE_CODE);
    setup.atm->setDate(11, 2026);
    setup.atm->installCassette(CashCassette(200, 20, 100, Atm::CURRENCY));
    setup.atm->installCassette(CashCassette(100, 20, 100, Atm::CURRENCY));
    setup.atm->installCassette(CashCassette(50, 20, 100, Atm::CURRENCY));
    setup.atm->installCassette(CashCassette(20, 20, 100, Atm::CURRENCY));
    setup.atm->installCassette(CashCassette(10, 20, 100, Atm::CURRENCY));
    return setup;
}

}  // namespace testdata
