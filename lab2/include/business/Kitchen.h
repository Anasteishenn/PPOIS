#pragma once

#include <string>
#include <vector>

#include "appliances/KitchenInventory.h"
#include "business/Bill.h"
#include "business/Chef.h"
#include "business/KitchenOrder.h"
#include "business/OrderQueue.h"
#include "business/RestaurantTable.h"
#include "business/ShoppingList.h"
#include "business/WasteLog.h"
#include "dishes/Menu.h"
#include "ingredients/Supplier.h"

/// @brief Ресторан целиком: меню, склад, шеф с поварами, столики, очередь заказов, выручка.
/// Это главный класс, через который работает консольное меню.
class Kitchen {
public:
    /// Сколько заказов может одновременно ждать в очереди.
    static constexpr int MAX_QUEUE_SIZE = 20;

    Kitchen(const std::string& name, const Menu& menu, const KitchenInventory& inventory, const Chef& chef);

    const std::string& getName() const { return _name; }
    const Menu& getMenu() const { return _menu; }
    const KitchenInventory& getInventory() const { return _inventory; }
    const Chef& getChef() const { return _chef; }
    const OrderQueue& getQueue() const { return _queue; }
    const std::vector<RestaurantTable>& getTables() const { return _tables; }
    const std::vector<KitchenOrder>& getReadyOrders() const { return _readyOrders; }
    const WasteLog& getWasteLog() const { return _wasteLog; }
    double getRevenue() const { return _revenue; }

    /// Добавить столик в зал.
    void addTable(const RestaurantTable& table);
    /// Посадить гостей за первый подходящий свободный столик. Возвращает номер столика.
    int seatGuests(int guests);
    /// Принять заказ от занятого столика. Возвращает номер заказа.
    int placeOrder(int tableNumber, const std::vector<std::string>& dishNames, bool isUrgent);
    /// Приготовить следующий заказ из очереди. Возвращает журнал готовки.
    std::vector<std::string> processNextOrder();
    /// Выставить счёт за готовый заказ.
    Bill createBill(int orderId, double tipPercent);
    /// Принять оплату по счёту. Возвращает сдачу.
    double acceptPayment(Bill& bill, double amount);
    /// Освободить столик.
    void releaseTable(int tableNumber);
    /// Списать со склада всё просроченное. Возвращает число списанных позиций.
    int writeOffExpired(int today);
    /// Составить список закупки у поставщика по тому, что заканчивается.
    ShoppingList makeShoppingList(const Supplier& supplier, double budget) const;
    /// Отчёт за день: столики, заказы, выручка, склад, отходы.
    std::string dailyReport() const;

private:
    /// Найти столик по номеру. Бросает TableException, если такого нет.
    RestaurantTable& findTable(int tableNumber);
    /// Хватит ли продуктов на весь заказ (одинаковые продукты разных блюд суммируются).
    bool canFulfill(const KitchenOrder& order) const;

    std::string _name;
    Menu _menu;
    KitchenInventory _inventory;
    Chef _chef;
    OrderQueue _queue;
    std::vector<RestaurantTable> _tables;
    std::vector<KitchenOrder> _readyOrders;
    WasteLog _wasteLog;
    int _nextOrderId;
    double _revenue;
};
