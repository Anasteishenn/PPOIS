#include "DemoAtm.h"

#include "accounts/SavingsAccount.h"
#include "cards/CreditCard.h"
#include "cards/DebitCard.h"
#include "cards/PrepaidCard.h"

namespace demo {

namespace {

const int CURRENT_MONTH = 11;
const int CURRENT_YEAR = 2026;
const int FIRST_DAY = 1;
const int CASSETTE_CAPACITY = 500;
const int MAINTENANCE_INTERVAL_DAYS = 30;

std::shared_ptr<Bank> createBank() {
    CurrencyExchange exchange(1.0);
    exchange.addCurrency(Currency("BYN", "р.", 1.0));
    exchange.addCurrency(Currency("USD", "$", 3.25));
    exchange.addCurrency(Currency("EUR", "€", 3.5));
    return std::make_shared<Bank>("АТМ-Банк", "ATMB", exchange);
}

CashbackProgram createCashback() {
    CashbackProgram cashback(1.0, 5.0, 30.0);
    cashback.addBonusCategory("Кафе");
    cashback.addBonusCategory("Такси");
    return cashback;
}

void addIvan(World& world) {
    auto ivan = std::make_shared<CardHolder>("Иван Петров", "MP1234567", "+375291234567");
    auto account = world.bank->openAccount(ivan->getFullName(), "BYN", 1500.0);
    auto debit = std::make_shared<DebitCard>("4000000000000010", "IVAN PETROV", 12, 2028, "1234", account,
                                             createCashback());
    auto expired = std::make_shared<DebitCard>("4000000000000036", "IVAN PETROV", 10, 2025, "0000", account,
                                               createCashback());
    world.bank->issueCard(debit, ivan);
    world.bank->issueCard(expired, ivan);
    world.cards.push_back({"Дебетовая, Иван Петров", "1234", debit});
    world.cards.push_back({"Дебетовая, Иван Петров (срок истёк)", "0000", expired});
}

void addAnna(World& world) {
    auto anna = std::make_shared<CardHolder>("Анна Смирнова", "MP7654321", "+375447654321");
    auto account = world.bank->openAccount(anna->getFullName(), "BYN", 200.0);
    auto giftAccount = world.bank->openAccount(anna->getFullName(), "BYN", 100.0);
    auto credit = std::make_shared<CreditCard>("5100000000000040", "ANNA SMIRNOVA", 6, 2027, "4321", account,
                                               1000.0, 2.5);
    auto prepaid = std::make_shared<PrepaidCard>("4000000000000028", "ANNA SMIRNOVA", 3, 2027, "1111", giftAccount,
                                                 true);
    world.bank->issueCard(credit, anna);
    world.bank->issueCard(prepaid, anna);
    world.bank->openAccount(anna->getFullName(), "USD", 50.0);
    world.bank->addAccount(std::make_shared<SavingsAccount>("BYATMBDEP1", anna->getFullName(),
                                                            world.bank->getExchange().findCurrency("BYN"), 3000.0,
                                                            9.5, 500.0));
    world.cards.push_back({"Кредитная, Анна Смирнова", "4321", credit});
    world.cards.push_back({"Предоплаченная, Анна Смирнова", "1111", prepaid});
}

void addForeignCard(World& world) {
    Currency byn("BYN", "р.", 1.0);
    auto account = std::make_shared<BankAccount>("BYOTHR0001", "John Smith", byn, 800.0);
    auto card = std::make_shared<DebitCard>("5200000000000056", "JOHN SMITH", 1, 2029, "9999", account,
                                            CashbackProgram(0.0, 0.0, 0.0));
    world.cards.push_back({"Карта чужого банка, John Smith", "9999", card});
}

std::shared_ptr<Atm> createAtm(const std::shared_ptr<Bank>& bank) {
    Address address("Минск", "пр. Независимости", "65", "БГУИР, корпус 1");
    FeePolicy fees(0.0, 1.5, 2.0, 0.5);
    auto atm = std::make_shared<Atm>("ATM-001", address, bank, fees, SERVICE_CODE);
    atm->setDate(CURRENT_MONTH, CURRENT_YEAR);
    atm->installCassette(CashCassette(200, 50, CASSETTE_CAPACITY, Atm::CURRENCY));
    atm->installCassette(CashCassette(100, 100, CASSETTE_CAPACITY, Atm::CURRENCY));
    atm->installCassette(CashCassette(50, 100, CASSETTE_CAPACITY, Atm::CURRENCY));
    atm->installCassette(CashCassette(20, 100, CASSETTE_CAPACITY, Atm::CURRENCY));
    atm->installCassette(CashCassette(10, 100, CASSETTE_CAPACITY, Atm::CURRENCY));
    return atm;
}

}  // namespace

World createWorld() {
    std::shared_ptr<Bank> bank = createBank();
    World world{bank,
                createAtm(bank),
                {},
                {ServiceProvider("МТС", "Мобильная связь", 9, 0.0), ServiceProvider("byfly", "Интернет", 8, 1.0)},
                {},
                CashCollector("Сергей Ковалёв", "C-01", EMPLOYEE_CODE),
                Technician("Пётр Лис", "T-07", EMPLOYEE_CODE, "Электроника банкоматов", 50.0),
                MaintenanceLog("ATM-001", MAINTENANCE_INTERVAL_DAYS),
                FIRST_DAY};
    ServiceProvider utilities("Минскэнерго", "Коммунальные услуги", 10, 0.0);
    world.bills.emplace_back(utilities, "1234567890", 85.40, 25);
    addIvan(world);
    addAnna(world);
    addForeignCard(world);
    return world;
}

}  // namespace demo
