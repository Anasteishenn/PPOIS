#include "business/Kitchen.h"

#include <map>

#include "Format.h"
#include "exceptions/CulinaryException.h"
#include "ingredients/PerishableIngredient.h"

Kitchen::Kitchen(const std::string& name, const Menu& menu, const KitchenInventory& inventory, const Chef& chef)
    : _name(name),
      _menu(menu),
      _inventory(inventory),
      _chef(chef),
      _queue(MAX_QUEUE_SIZE),
      _nextOrderId(1),
      _revenue(0.0) {}

void Kitchen::addTable(const RestaurantTable& table) {
    _tables.push_back(table);
}

int Kitchen::seatGuests(int guests) {
    for (RestaurantTable& table : _tables) {
        if (table.canSeat(guests)) {
            table.seat(guests);
            return table.getNumber();
        }
    }
    throw TableException("Нет свободного столика на " + std::to_string(guests) + " гостей");
}

int Kitchen::placeOrder(int tableNumber, const std::vector<std::string>& dishNames, bool isUrgent) {
    const RestaurantTable& table = findTable(tableNumber);
    if (!table.isOccupied()) {
        throw TableException("За столиком №" + std::to_string(tableNumber) + " никто не сидит");
    }
    KitchenOrder order(_nextOrderId, table, isUrgent);
    for (const std::string& dishName : dishNames) {
        std::shared_ptr<Dish> dish = _menu.findDish(dishName);
        if (!dish->isAvailable()) {
            throw DishNotAvailableException("Блюдо закончилось: " + dishName);
        }
        order.addDish(dish);
    }
    _queue.enqueue(order);
    return _nextOrderId++;
}

std::vector<std::string> Kitchen::processNextOrder() {
    KitchenOrder order = _queue.next();
    std::string title = "Заказ №" + std::to_string(order.getId());
    if (!canFulfill(order)) {
        order.cancel();
        return {title + " отменён: не хватает продуктов"};
    }
    order.startCooking(_chef.getName());
    std::vector<std::string> log = {title + " (столик №" + std::to_string(order.getTableNumber()) + ")"};
    for (const std::shared_ptr<Dish>& dish : order.getDishes()) {
        std::shared_ptr<Cook> cook = _chef.chooseCook(*dish);
        _inventory.cookRecipe(dish->getRecipe());
        std::vector<std::string> steps = cook->cook(*dish);
        log.insert(log.end(), steps.begin(), steps.end());
    }
    order.markReady();
    _readyOrders.push_back(order);
    log.push_back(title + " готов");
    return log;
}

Bill Kitchen::createBill(int orderId, double tipPercent) {
    for (auto it = _readyOrders.begin(); it != _readyOrders.end(); ++it) {
        if (it->getId() == orderId) {
            Bill bill(*it, tipPercent);
            _readyOrders.erase(it);
            return bill;
        }
    }
    throw OrderException("Нет готового заказа №" + std::to_string(orderId));
}

double Kitchen::acceptPayment(Bill& bill, double amount) {
    bill.pay(amount);
    _revenue += bill.total();
    return bill.change();
}

void Kitchen::releaseTable(int tableNumber) {
    findTable(tableNumber).release();
}

int Kitchen::writeOffExpired(int today) {
    int count = 0;
    for (const StockItem& item : _inventory.getStock()) {
        const auto* perishable = dynamic_cast<const PerishableIngredient*>(&item.getIngredient());
        double quantity = item.getQuantity();
        if (perishable != nullptr && quantity > 0 && _wasteLog.logExpired(*perishable, quantity, today)) {
            _inventory.findStock(perishable->getName()).consume(quantity);
            ++count;
        }
    }
    return count;
}

ShoppingList Kitchen::makeShoppingList(const Supplier& supplier, double budget) const {
    ShoppingList list(supplier, budget);
    list.fillFrom(_inventory);
    return list;
}

std::string Kitchen::dailyReport() const {
    int occupied = 0;
    for (const RestaurantTable& table : _tables) {
        occupied += table.isOccupied() ? 1 : 0;
    }
    return "Ресторан «" + _name + "»\n" +
           "Занято столиков: " + std::to_string(occupied) + " из " + std::to_string(_tables.size()) + "\n" +
           "Заказов в очереди: " + std::to_string(_queue.pendingCount()) + "\n" +
           "Обработано заказов: " + std::to_string(_queue.getProcessedCount()) + "\n" +
           "Готовы к оплате: " + std::to_string(_readyOrders.size()) + "\n" +
           "Выручка: " + format::number(_revenue) + " BYN\n" +
           "Стоимость склада: " + format::number(_inventory.totalStockValue()) + " BYN\n" +
           "Списано продуктов на: " + format::number(_wasteLog.getTotalCost()) + " BYN";
}

RestaurantTable& Kitchen::findTable(int tableNumber) {
    for (RestaurantTable& table : _tables) {
        if (table.getNumber() == tableNumber) {
            return table;
        }
    }
    throw TableException("Нет столика №" + std::to_string(tableNumber));
}

bool Kitchen::canFulfill(const KitchenOrder& order) const {
    std::map<std::string, double> needed;
    for (const std::shared_ptr<Dish>& dish : order.getDishes()) {
        for (const IngredientProportion& proportion : dish->getRecipe().getProportions()) {
            needed[proportion.getIngredient().getName()] += proportion.getAmount();
        }
    }
    for (const auto& [ingredientName, amount] : needed) {
        if (_inventory.availableAmount(ingredientName) < amount) {
            return false;
        }
    }
    return true;
}
