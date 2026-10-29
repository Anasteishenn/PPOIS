#include "appliances/KitchenInventory.h"

#include <utility>

#include "exceptions/CulinaryException.h"

KitchenInventory::KitchenInventory(const std::string& location) : _location(location) {}

void KitchenInventory::addAppliance(std::shared_ptr<KitchenAppliance> appliance) {
    if (!appliance) {
        throw ApplianceException("Нельзя добавить пустой прибор");
    }
    _appliances.push_back(std::move(appliance));
}

std::shared_ptr<KitchenAppliance> KitchenInventory::findAppliance(const std::string& applianceType) const {
    for (const std::shared_ptr<KitchenAppliance>& appliance : _appliances) {
        if (appliance->type() == applianceType) {
            return appliance;
        }
    }
    throw ApplianceException("На кухне нет прибора: " + applianceType);
}

void KitchenInventory::addStock(const StockItem& item) {
    _stock.push_back(item);
}

StockItem& KitchenInventory::findStock(const std::string& ingredientName) {
    for (StockItem& item : _stock) {
        if (item.getIngredient().getName() == ingredientName) {
            return item;
        }
    }
    throw InsufficientStockException("Нет на складе: " + ingredientName);
}

double KitchenInventory::availableAmount(const std::string& ingredientName) const {
    for (const StockItem& item : _stock) {
        if (item.getIngredient().getName() == ingredientName) {
            return item.getQuantity();
        }
    }
    return 0.0;
}

bool KitchenInventory::hasEnoughFor(const Recipe& recipe) const {
    for (const IngredientProportion& proportion : recipe.getProportions()) {
        if (availableAmount(proportion.getIngredient().getName()) < proportion.getAmount()) {
            return false;
        }
    }
    return true;
}

void KitchenInventory::cookRecipe(const Recipe& recipe) {
    if (!hasEnoughFor(recipe)) {
        throw InsufficientStockException("Не хватает продуктов для рецепта: " + recipe.getTitle());
    }
    for (const IngredientProportion& proportion : recipe.getProportions()) {
        findStock(proportion.getIngredient().getName()).consume(proportion.getAmount());
    }
}

std::vector<StockItem> KitchenInventory::itemsToRestock() const {
    std::vector<StockItem> result;
    for (const StockItem& item : _stock) {
        if (item.needsRestock()) {
            result.push_back(item);
        }
    }
    return result;
}

double KitchenInventory::totalStockValue() const {
    double total = 0.0;
    for (const StockItem& item : _stock) {
        total += item.value();
    }
    return total;
}
