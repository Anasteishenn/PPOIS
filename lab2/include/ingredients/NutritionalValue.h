#pragma once

/// @brief Пищевая ценность: калории, белки, жиры, углеводы.
class NutritionalValue {
public:
    /// Ниже этого числа калорий продукт считается низкокалорийным.
    static constexpr double LOW_CALORIE_LIMIT = 100.0;

    NutritionalValue(double calories = 0.0, double proteins = 0.0, double fats = 0.0, double carbs = 0.0);

    double getCalories() const { return _calories; }
    double getProteins() const { return _proteins; }
    double getFats() const { return _fats; }
    double getCarbs() const { return _carbs; }

    /// Умножает все показатели на коэффициент (например, пересчёт на другую массу).
    NutritionalValue scale(double factor) const;
    /// Складывает две пищевые ценности (итог по блюду).
    NutritionalValue add(const NutritionalValue& other) const;
    /// Проверяет, что калорий меньше LOW_CALORIE_LIMIT.
    bool isLowCalorie() const;

private:
    double _calories;
    double _proteins;
    double _fats;
    double _carbs;
};
