#pragma once

#include <map>
#include <string>

#include "appliances/KitchenInventory.h"
#include "ingredients/StockItem.h"
#include "ingredients/Supplier.h"

/// @brief Список закупки у поставщика: что и сколько купить, укладываемся ли в бюджет.
class ShoppingList {
public:
    ShoppingList(const Supplier& supplier, double budget);

    const std::map<std::string, double>& getItems() const { return _items; }
    double getBudget() const { return _budget; }
    double getEstimatedCost() const { return _estimatedCost; }
    const Supplier& getSupplier() const { return _supplier; }

    /// Добавить в список то, что нужно дозаказать по позиции склада.
    void addItem(const StockItem& item);
    /// Заполнить список всем, что заканчивается на складе.
    void fillFrom(const KitchenInventory& inventory);
    /// Укладывается ли закупка в бюджет.
    bool isWithinBudget() const;
    /// Оформить закупку: список очищается, возвращается день доставки.
    /// Бросает PaymentException, если список пуст или превышен бюджет.
    int checkout(int today);

private:
    Supplier _supplier;
    double _budget;
    std::map<std::string, double> _items;
    double _estimatedCost;
};
