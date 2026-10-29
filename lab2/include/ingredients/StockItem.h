#pragma once

#include <memory>

#include "ingredients/Ingredient.h"
#include "ingredients/IngredientPackage.h"
#include "ingredients/Supplier.h"

/// @brief Позиция на складе: какой продукт, сколько его есть, минимальный запас и кто поставляет.
class StockItem {
public:
    /// Дозаказываем столько, чтобы запас стал равен минимуму, умноженному на этот коэффициент.
    static constexpr double RESTOCK_FACTOR = 2.0;

    StockItem(std::shared_ptr<Ingredient> ingredient, double quantity, double minQuantity, const Supplier& supplier);

    const Ingredient& getIngredient() const { return *_ingredient; }
    double getQuantity() const { return _quantity; }
    const Supplier& getSupplier() const { return _supplier; }

    /// Списать продукт со склада. Бросает InsufficientStockException, если не хватает.
    void consume(double amount);
    /// Пополнить склад на заданное количество.
    void restock(double amount);
    /// Принять поставку упаковок того же продукта.
    void receivePackage(const IngredientPackage& package);
    /// Пора ли дозаказывать (запас ниже минимума).
    bool needsRestock() const;
    /// Сколько нужно дозаказать (0, если запаса хватает).
    double amountToOrder() const;
    /// Стоимость всего запаса.
    double value() const;

private:
    std::shared_ptr<Ingredient> _ingredient;
    double _quantity;
    double _minQuantity;
    Supplier _supplier;
};
