#include "business/WasteLog.h"

#include "Format.h"
#include "exceptions/CulinaryException.h"

WasteLog::WasteLog() : _totalCost(0.0), _totalAmount(0.0) {}

void WasteLog::logWaste(const Ingredient& ingredient, double amount, const std::string& reason) {
    if (amount <= 0) {
        throw InvalidIngredientException("Списывать можно только положительное количество");
    }
    _totalAmount += amount;
    _totalCost += ingredient.costFor(amount);
    _entries.push_back(ingredient.getName() + ": " + format::number(amount) + " " + ingredient.unit() + " — " +
                       reason);
}

bool WasteLog::logExpired(const PerishableIngredient& ingredient, double amount, int today) {
    if (!ingredient.isExpired(today)) {
        return false;
    }
    logWaste(ingredient, amount, "истёк срок годности");
    return true;
}

double WasteLog::wastePercent(double purchasedAmount) const {
    if (purchasedAmount <= 0) {
        throw InvalidIngredientException("Объём закупки должен быть положительным");
    }
    return _totalAmount / purchasedAmount * PERCENT;
}

bool WasteLog::isAboveNorm(double purchasedAmount) const {
    return wastePercent(purchasedAmount) > NORM_PERCENT;
}
