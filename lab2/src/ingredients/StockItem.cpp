#include "ingredients/StockItem.h"

#include <utility>

#include "exceptions/CulinaryException.h"

StockItem::StockItem(std::shared_ptr<Ingredient> ingredient, double quantity, double minQuantity,
                     const Supplier& supplier)
    : _ingredient(std::move(ingredient)), _quantity(quantity), _minQuantity(minQuantity), _supplier(supplier) {
    if (!_ingredient || quantity < 0 || minQuantity < 0) {
        throw InvalidIngredientException("Некорректная позиция склада");
    }
}

void StockItem::consume(double amount) {
    if (amount <= 0) {
        throw InvalidIngredientException("Списывать можно только положительное количество");
    }
    if (amount > _quantity) {
        throw InsufficientStockException("Не хватает на складе: " + _ingredient->getName());
    }
    _quantity -= amount;
}

void StockItem::restock(double amount) {
    if (amount <= 0) {
        throw InvalidIngredientException("Пополнять можно только положительное количество");
    }
    _quantity += amount;
}

void StockItem::receivePackage(const IngredientPackage& package) {
    if (package.getIngredient().getName() != _ingredient->getName()) {
        throw InvalidIngredientException("Поставка другого продукта: " + package.getIngredient().getName());
    }
    restock(package.totalAmount());
}

bool StockItem::needsRestock() const {
    return _quantity < _minQuantity;
}

double StockItem::amountToOrder() const {
    return needsRestock() ? _minQuantity * RESTOCK_FACTOR - _quantity : 0.0;
}

double StockItem::value() const {
    return _ingredient->costFor(_quantity);
}
