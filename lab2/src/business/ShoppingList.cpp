#include "business/ShoppingList.h"

#include "exceptions/CulinaryException.h"

ShoppingList::ShoppingList(const Supplier& supplier, double budget)
    : _supplier(supplier), _budget(budget), _estimatedCost(0.0) {
    if (budget < 0) {
        throw PaymentException("Бюджет не может быть отрицательным");
    }
}

void ShoppingList::addItem(const StockItem& item) {
    double amount = item.amountToOrder();
    if (amount <= 0) {
        return;
    }
    _items[item.getIngredient().getName()] += amount;
    _estimatedCost += item.getIngredient().costFor(amount);
}

void ShoppingList::fillFrom(const KitchenInventory& inventory) {
    for (const StockItem& item : inventory.itemsToRestock()) {
        addItem(item);
    }
}

bool ShoppingList::isWithinBudget() const {
    return _estimatedCost <= _budget;
}

int ShoppingList::checkout(int today) {
    if (_items.empty()) {
        throw PaymentException("Список покупок пуст");
    }
    if (!isWithinBudget()) {
        throw PaymentException("Закупка превышает бюджет");
    }
    _items.clear();
    _estimatedCost = 0.0;
    return _supplier.deliveryDay(today);
}
