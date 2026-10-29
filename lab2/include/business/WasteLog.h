#pragma once

#include <string>
#include <vector>

#include "ingredients/Ingredient.h"
#include "ingredients/PerishableIngredient.h"

/// @brief Журнал списаний: что выбросили, сколько и почему.
class WasteLog {
public:
    /// Допустимая доля отходов от закупки, %.
    static constexpr double NORM_PERCENT = 5.0;
    static constexpr double PERCENT = 100.0;

    WasteLog();

    const std::vector<std::string>& getEntries() const { return _entries; }
    double getTotalCost() const { return _totalCost; }
    double getTotalAmount() const { return _totalAmount; }

    /// Записать списание продукта с причиной.
    void logWaste(const Ingredient& ingredient, double amount, const std::string& reason);
    /// Списать продукт, если он просрочен. Возвращает true, если списали.
    bool logExpired(const PerishableIngredient& ingredient, double amount, int today);
    /// Какой процент от закупленного ушёл в отходы.
    double wastePercent(double purchasedAmount) const;
    /// Превышена ли норма отходов.
    bool isAboveNorm(double purchasedAmount) const;

private:
    std::vector<std::string> _entries;
    double _totalCost;
    double _totalAmount;
};
