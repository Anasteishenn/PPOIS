#pragma once

#include <set>
#include <string>

/// @brief Программа кэшбэка: базовый процент, повышенный процент в избранных категориях, потолок за месяц.
class CashbackProgram {
public:
    static constexpr double PERCENT = 100.0;

    /// Бросает InvalidAmountException, если проценты или потолок отрицательные.
    CashbackProgram(double basePercent, double bonusPercent, double monthlyCap);

    double getBasePercent() const { return _basePercent; }
    double getBonusPercent() const { return _bonusPercent; }
    double getMonthlyCap() const { return _monthlyCap; }

    /// Добавить категорию с повышенным кэшбэком (например, «Кафе»).
    void addBonusCategory(const std::string& category);
    /// Входит ли категория в повышенный кэшбэк.
    bool isBonusCategory(const std::string& category) const;
    /// Процент кэшбэка для категории.
    double percentFor(const std::string& category) const;
    /// Кэшбэк за покупку с учётом того, сколько уже начислено в этом месяце (не больше потолка).
    double calculate(double amount, const std::string& category, double earnedThisMonth) const;

private:
    double _basePercent;
    double _bonusPercent;
    double _monthlyCap;
    std::set<std::string> _bonusCategories;
};
