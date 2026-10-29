/**
 * @file main.cpp
 * @brief Консольное меню ресторана. Только ввод-вывод: вся логика — в классах из src/.
 */
#include <iostream>
#include <string>
#include <vector>

#include "ConsoleUtils.h"
#include "DemoKitchen.h"
#include "Format.h"
#include "exceptions/CulinaryException.h"

namespace {

const int EXIT_CHOICE = 0;
const int YES_ANSWER = 1;

using Action = void (*)(Kitchen&);

struct MenuItem {
    const char* title;
    Action action;
};

void printDishes(const Menu& menu) {
    const std::vector<std::shared_ptr<Dish>>& dishes = menu.getDishes();
    for (std::size_t index = 0; index < dishes.size(); ++index) {
        const Dish& dish = *dishes[index];
        std::cout << index + 1 << ". [" << dish.course() << "] " << dish.getName() << " — " << dish.getPrice().format()
                  << ", " << format::number(dish.caloriesPerServing()) << " ккал"
                  << (dish.isAvailable() ? "" : " (нет в наличии)") << '\n';
    }
}

void showMenu(Kitchen& kitchen) {
    printDishes(kitchen.getMenu());
    for (const ComboMeal& combo : kitchen.getMenu().getCombos()) {
        std::cout << "Комбо «" << combo.getTitle() << "»: " << format::number(combo.totalPrice())
                  << " BYN, экономия " << format::number(combo.savings()) << " BYN\n";
    }
}

void showTables(Kitchen& kitchen) {
    for (const RestaurantTable& table : kitchen.getTables()) {
        std::cout << "Столик №" << table.getNumber() << ": мест " << table.getSeats() << ", "
                  << (table.isOccupied() ? "занят (" + std::to_string(table.getGuests()) + " гост.)" : "свободен")
                  << '\n';
    }
}

void seatGuests(Kitchen& kitchen) {
    int tableNumber = kitchen.seatGuests(console::readInt("Сколько гостей: "));
    std::cout << "Гости сели за столик №" << tableNumber << '\n';
}

std::vector<std::string> chooseDishes(const Menu& menu) {
    printDishes(menu);
    const std::vector<std::shared_ptr<Dish>>& dishes = menu.getDishes();
    std::vector<std::string> names;
    int choice = console::readInt("Номер блюда (0 — закончить): ");
    while (choice != EXIT_CHOICE) {
        if (choice > 0 && choice <= static_cast<int>(dishes.size())) {
            names.push_back(dishes[choice - 1]->getName());
        } else {
            std::cout << "Нет такого блюда.\n";
        }
        choice = console::readInt("Номер блюда (0 — закончить): ");
    }
    return names;
}

void placeOrder(Kitchen& kitchen) {
    int tableNumber = console::readInt("Номер столика: ");
    std::vector<std::string> names = chooseDishes(kitchen.getMenu());
    bool isUrgent = console::readInt("Срочный заказ? (1 — да, 0 — нет): ") == YES_ANSWER;
    int orderId = kitchen.placeOrder(tableNumber, names, isUrgent);
    std::cout << "Заказ №" << orderId << " принят. В очереди: " << kitchen.getQueue().pendingCount() << '\n';
}

void cookNextOrder(Kitchen& kitchen) {
    for (const std::string& line : kitchen.processNextOrder()) {
        std::cout << line << '\n';
    }
}

void printReadyOrders(const Kitchen& kitchen) {
    for (const KitchenOrder& order : kitchen.getReadyOrders()) {
        std::cout << "Заказ №" << order.getId() << ", столик №" << order.getTableNumber() << ", сумма "
                  << format::number(order.total()) << " BYN\n";
    }
}

void payBill(Kitchen& kitchen) {
    if (kitchen.getReadyOrders().empty()) {
        std::cout << "Нет готовых заказов.\n";
        return;
    }
    printReadyOrders(kitchen);
    int orderId = console::readInt("Номер заказа: ");
    Bill bill = kitchen.createBill(orderId, console::readInt("Чаевые, %: "));
    std::cout << "Блюда: " << format::number(bill.subtotal()) << ", чаевые: " << format::number(bill.tip())
              << ", итого: " << format::number(bill.total()) << " BYN\n";
    int amount = console::readInt("Гость даёт (BYN): ");
    while (amount < bill.total()) {
        amount = console::readInt("Не хватает. Гость даёт (BYN): ");
    }
    std::cout << "Оплачено. Сдача: " << format::number(kitchen.acceptPayment(bill, amount)) << " BYN\n";
}

void releaseTable(Kitchen& kitchen) {
    int tableNumber = console::readInt("Номер столика: ");
    kitchen.releaseTable(tableNumber);
    std::cout << "Столик №" << tableNumber << " свободен.\n";
}

void showStock(Kitchen& kitchen) {
    const std::vector<StockItem>& stock = kitchen.getInventory().getStock();
    for (const StockItem& item : stock) {
        std::cout << item.getIngredient().describe() << ": " << format::number(item.getQuantity())
                  << (item.needsRestock() ? "  <- пора дозаказать" : "") << '\n';
    }
    ShoppingList list = kitchen.makeShoppingList(stock.front().getSupplier(), console::readInt("Бюджет закупки: "));
    std::cout << "Поставщик: " << list.getSupplier().contactCard() << '\n';
    for (const auto& [name, amount] : list.getItems()) {
        std::cout << "  купить " << name << ": " << format::number(amount) << '\n';
    }
    std::cout << "Стоимость: " << format::number(list.getEstimatedCost()) << " BYN, "
              << (list.isWithinBudget() ? "в рамках бюджета" : "превышает бюджет") << '\n';
}

void writeOffExpired(Kitchen& kitchen) {
    int count = kitchen.writeOffExpired(console::readInt("Какой сегодня день (номер): "));
    std::cout << "Списано позиций: " << count << '\n';
    for (const std::string& entry : kitchen.getWasteLog().getEntries()) {
        std::cout << "  " << entry << '\n';
    }
}

void showWithoutAllergen(Kitchen& kitchen) {
    std::string allergen = console::readNonEmptyLine("Аллерген (Глютен, Лактоза, Яйца): ");
    for (const std::shared_ptr<Dish>& dish : kitchen.getMenu().withoutAllergen(allergen)) {
        std::cout << "  " << dish->getName() << '\n';
    }
}

void showReport(Kitchen& kitchen) {
    std::cout << kitchen.dailyReport() << '\n';
}

const std::vector<MenuItem> MENU_ITEMS = {
    {"Показать меню", showMenu},
    {"Показать столики", showTables},
    {"Посадить гостей", seatGuests},
    {"Принять заказ", placeOrder},
    {"Приготовить следующий заказ", cookNextOrder},
    {"Выставить счёт и принять оплату", payBill},
    {"Освободить столик", releaseTable},
    {"Склад и список закупки", showStock},
    {"Списать просроченные продукты", writeOffExpired},
    {"Блюда без аллергена", showWithoutAllergen},
    {"Отчёт за день", showReport},
};

void printMainMenu(const Kitchen& kitchen) {
    std::cout << "\n=== Ресторан «" << kitchen.getName() << "» ===\n";
    for (std::size_t index = 0; index < MENU_ITEMS.size(); ++index) {
        std::cout << index + 1 << ". " << MENU_ITEMS[index].title << '\n';
    }
    std::cout << EXIT_CHOICE << ". Выход\n";
}

void runAction(Kitchen& kitchen, int choice) {
    if (choice < 1 || choice > static_cast<int>(MENU_ITEMS.size())) {
        std::cout << "Нет такого пункта.\n";
        return;
    }
    try {
        MENU_ITEMS[choice - 1].action(kitchen);
    } catch (const CulinaryException& error) {
        console::printError(error);
    }
}

}  // namespace

int main() {
    console::setupEncoding();
    Kitchen kitchen = demo::createKitchen();
    try {
        printMainMenu(kitchen);
        int choice = console::readInt("Выберите пункт: ");
        while (choice != EXIT_CHOICE) {
            runAction(kitchen, choice);
            printMainMenu(kitchen);
            choice = console::readInt("Выберите пункт: ");
        }
    } catch (const console::InputClosed&) {
        std::cout << '\n';
    }
    return 0;
}
