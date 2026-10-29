#pragma once

#include <memory>
#include <string>
#include <vector>

#include "appliances/KitchenAppliance.h"
#include "ingredients/StockItem.h"
#include "recipes/Recipe.h"

/// @brief Инвентарь кухни: все приборы и склад продуктов.
class KitchenInventory {
public:
    explicit KitchenInventory(const std::string& location);

    const std::string& getLocation() const { return _location; }
    const std::vector<std::shared_ptr<KitchenAppliance>>& getAppliances() const { return _appliances; }
    const std::vector<StockItem>& getStock() const { return _stock; }

    /// Добавить прибор.
    void addAppliance(std::shared_ptr<KitchenAppliance> appliance);
    /// Найти прибор по типу. Бросает ApplianceException, если такого нет.
    std::shared_ptr<KitchenAppliance> findAppliance(const std::string& applianceType) const;
    /// Добавить позицию на склад.
    void addStock(const StockItem& item);
    /// Найти позицию склада по названию продукта. Бросает InsufficientStockException, если нет.
    StockItem& findStock(const std::string& ingredientName);
    /// Сколько продукта есть на складе (0, если нет совсем).
    double availableAmount(const std::string& ingredientName) const;
    /// Хватает ли продуктов, чтобы приготовить рецепт.
    bool hasEnoughFor(const Recipe& recipe) const;
    /// Списать со склада всё, что нужно для рецепта.
    void cookRecipe(const Recipe& recipe);
    /// Позиции, которые пора дозаказать.
    std::vector<StockItem> itemsToRestock() const;
    /// Стоимость всего склада.
    double totalStockValue() const;

private:
    std::string _location;
    std::vector<std::shared_ptr<KitchenAppliance>> _appliances;
    std::vector<StockItem> _stock;
};
