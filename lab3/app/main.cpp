/**
 * @file main.cpp
 * @brief Консольное меню банкомата. Только ввод-вывод: вся логика — в классах из src/.
 */
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "ConsoleUtils.h"
#include "DemoAtm.h"
#include "Format.h"
#include "exceptions/AtmException.h"

namespace {

const int EXIT_CHOICE = 0;
const int STATEMENT_SIZE = 5;
const int SERIAL_DIGITS = 7;

using demo::World;
using Action = void (*)(World&);

struct MenuItem {
    const char* title;
    Action action;
};

void insertCard(World& world) {
    for (std::size_t index = 0; index < world.cards.size(); ++index) {
        const demo::DemoCard& item = world.cards[index];
        std::cout << index + 1 << ". " << item.card->maskedNumber() << " — " << item.label << " (PIN " << item.pin
                  << ")\n";
    }
    int choice = console::readInt("Какую карту вставить: ");
    if (choice < 1 || choice > static_cast<int>(world.cards.size())) {
        std::cout << "Нет такой карты.\n";
        return;
    }
    world.atm->insertCard(world.cards[choice - 1].card);
    std::cout << "Карта принята. Введите PIN-код.\n";
}

void enterPin(World& world) {
    world.atm->enterPin(console::readNonEmptyLine("PIN-код: "));
    std::cout << "PIN-код верный.\n";
}

void showBalance(World& world) {
    std::cout << "Доступно: " << format::number(world.atm->checkBalance()) << " BYN\n";
}

void withdrawCash(World& world) {
    NotesPlan notes = world.atm->withdraw(console::readInt("Сумма (кратно 10 BYN): "));
    std::cout << "Возьмите деньги:\n";
    for (const auto& [denomination, count] : notes) {
        std::cout << "  " << denomination << " BYN x " << count << '\n';
    }
    std::cout << world.atm->getLog().lastTransaction()->describe() << '\n';
}

std::vector<Banknote> parseNotes(const std::string& line) {
    static int serialCounter = 0;
    std::vector<Banknote> notes;
    std::istringstream stream(line);
    int denomination = 0;
    while (stream >> denomination) {
        std::string digits = std::to_string(++serialCounter);
        std::string serial = "AB" + std::string(SERIAL_DIGITS - digits.size(), '0') + digits;
        notes.emplace_back(denomination, Atm::CURRENCY, serial);
    }
    return notes;
}

void depositCash(World& world) {
    std::string line = console::readNonEmptyLine("Номиналы купюр через пробел (например: 100 50 20): ");
    int accepted = world.atm->deposit(parseNotes(line));
    std::cout << "Зачислено: " << accepted << " BYN\n";
    int rejected = world.atm->getDepositModule().getLastRejectedCount();
    if (rejected > 0) {
        std::cout << "Не принято купюр: " << rejected << " — заберите их.\n";
    }
}

void transferMoney(World& world) {
    std::cout << "Счета для примера: BYATMB10001 (Иван, BYN), BYATMB10002 (Анна, BYN), BYATMB10004 (Анна, USD)\n";
    std::string account = console::readNonEmptyLine("Счёт получателя: ");
    world.atm->transfer(account, console::readInt("Сумма перевода (BYN): "));
    std::cout << world.atm->getLog().lastTransaction()->describe() << '\n';
}

void payService(World& world) {
    for (std::size_t index = 0; index < world.providers.size(); ++index) {
        const ServiceProvider& provider = world.providers[index];
        std::cout << index + 1 << ". " << provider.label() << ", лицевой счёт из " << provider.getAccountLength()
                  << " цифр\n";
    }
    int choice = console::readInt("Поставщик: ");
    if (choice < 1 || choice > static_cast<int>(world.providers.size())) {
        std::cout << "Нет такого поставщика.\n";
        return;
    }
    std::string account = console::readNonEmptyLine("Лицевой счёт (телефон без +375, договор): ");
    world.atm->payService(world.providers[choice - 1], account, console::readInt("Сумма (BYN): "));
    std::cout << "Оплачено. " << world.atm->getLog().lastTransaction()->describe() << '\n';
}

void payUtilityBill(World& world) {
    UtilityBill& bill = world.bills.front();
    std::cout << bill.getProvider().label() << ", счёт " << bill.getAccountNumber() << ": "
              << format::number(bill.getAmountDue()) << " BYN, оплатить до дня " << bill.getDueDay()
              << ". Сегодня день " << world.day << ", к оплате " << format::number(bill.totalToPay(world.day))
              << " BYN\n";
    world.atm->payUtilityBill(bill, world.day);
    std::cout << "Счёт оплачен.\n";
}

void changePin(World& world) {
    std::string oldPin = console::readNonEmptyLine("Старый PIN-код: ");
    world.atm->changePin(oldPin, console::readNonEmptyLine("Новый PIN-код (4 цифры): "));
    std::cout << "PIN-код изменён.\n";
}

void showStatement(World& world) {
    std::vector<StatementEntry> entries = world.atm->miniStatement(STATEMENT_SIZE);
    if (entries.empty()) {
        std::cout << "Операций по счёту пока нет.\n";
    }
    for (const StatementEntry& entry : entries) {
        std::cout << "  " << entry.format() << '\n';
    }
}

void printReceipt(World& world) {
    std::cout << world.atm->printReceipt() << '\n';
}

void takeCard(World& world) {
    std::cout << "Заберите карту " << world.atm->ejectCard()->maskedNumber() << '\n';
}

void waitTime(World& world) {
    bool isCaptured = world.atm->tick(console::readInt("Сколько секунд прошло: "));
    std::cout << (isCaptured ? "Клиент не забрал карту — карта изъята.\n" : "Ничего не произошло.\n");
}

void showStatus(World& world) {
    std::cout << world.atm->statusReport() << '\n' << world.atm->getFeePolicy().describe() << '\n';
    std::cout << "Отправлено SMS: " << world.atm->getNotifier().getSentMessages().size() << '\n';
}

void encash(World& world) {
    world.collector.startShift(demo::EMPLOYEE_CODE);
    std::string code = console::readNonEmptyLine("Служебный код банкомата (7777): ");
    EncashmentReport report = world.collector.encash(*world.atm, code, world.day);
    world.collector.endShift();
    std::cout << report.summary() << '\n';
}

void breakDevice(World& world) {
    std::vector<AtmDevice*> devices = world.atm->devices();
    for (std::size_t index = 0; index < devices.size(); ++index) {
        std::cout << index + 1 << ". " << devices[index]->statusLine() << '\n';
    }
    int choice = console::readInt("Какое устройство сломать: ");
    if (choice < 1 || choice > static_cast<int>(devices.size())) {
        std::cout << "Нет такого устройства.\n";
        return;
    }
    devices[choice - 1]->reportError("сбой (демонстрация)");
    std::cout << devices[choice - 1]->statusLine() << '\n';
}

void callTechnician(World& world) {
    for (const std::string& problem : world.technician.diagnose(*world.atm)) {
        std::cout << "  Неисправно: " << problem << '\n';
    }
    world.technician.startShift(demo::EMPLOYEE_CODE);
    std::string code = console::readNonEmptyLine("Служебный код банкомата (7777): ");
    int repaired = world.technician.repairAll(*world.atm, code, world.maintenance, world.day);
    world.technician.endShift();
    std::cout << "Отремонтировано устройств: " << repaired << '\n';
    for (const MaintenanceRecord& record : world.maintenance.getRecords()) {
        std::cout << "  " << record.summary() << '\n';
    }
}

void nextDay(World& world) {
    ++world.day;
    for (const demo::DemoCard& item : world.cards) {
        item.card->startNewDay();
    }
    std::cout << "Наступил день " << world.day << ". Дневные лимиты карт обнулены.\n";
}

const std::vector<MenuItem> MENU_ITEMS = {
    {"Вставить карту", insertCard},
    {"Ввести PIN-код", enterPin},
    {"Баланс", showBalance},
    {"Снять наличные", withdrawCash},
    {"Внести наличные", depositCash},
    {"Перевод на счёт", transferMoney},
    {"Оплата услуг (мобильная связь, интернет)", payService},
    {"Оплата коммунального счёта", payUtilityBill},
    {"Сменить PIN-код", changePin},
    {"Мини-выписка", showStatement},
    {"Напечатать чек", printReceipt},
    {"Забрать карту", takeCard},
    {"Подождать (проверка тайм-аута)", waitTime},
    {"Состояние банкомата", showStatus},
    {"Инкассация", encash},
    {"Сломать устройство (демонстрация)", breakDevice},
    {"Вызвать техника", callTechnician},
    {"Следующий день", nextDay},
};

void printMainMenu(const World& world) {
    std::cout << "\n=== Банкомат " << world.atm->getId() << " (" << world.atm->getAddress().format() << ") ===\n";
    std::cout << "Экран: " << world.atm->getDisplay().lastMessage() << '\n';
    for (std::size_t index = 0; index < MENU_ITEMS.size(); ++index) {
        std::cout << index + 1 << ". " << MENU_ITEMS[index].title << '\n';
    }
    std::cout << EXIT_CHOICE << ". Выход\n";
}

void runAction(World& world, int choice) {
    if (choice < 1 || choice > static_cast<int>(MENU_ITEMS.size())) {
        std::cout << "Нет такого пункта.\n";
        return;
    }
    try {
        MENU_ITEMS[choice - 1].action(world);
    } catch (const AtmException& error) {
        console::printError(error);
    }
}

}  // namespace

int main() {
    console::setupEncoding();
    World world = demo::createWorld();
    try {
        printMainMenu(world);
        int choice = console::readInt("Выберите пункт: ");
        while (choice != EXIT_CHOICE) {
            runAction(world, choice);
            printMainMenu(world);
            choice = console::readInt("Выберите пункт: ");
        }
    } catch (const console::InputClosed&) {
        std::cout << '\n';
    }
    return 0;
}
