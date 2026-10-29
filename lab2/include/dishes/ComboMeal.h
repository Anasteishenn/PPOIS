#pragma once

#include <memory>
#include <string>
#include <vector>

#include "dishes/Dish.h"

/// @brief Комбо-набор: несколько блюд со скидкой (например, «Бизнес-ланч»).
class ComboMeal {
public:
    ComboMeal(const std::string& title, double discountPercent);

    const std::string& getTitle() const { return _title; }
    const std::vector<std::shared_ptr<Dish>>& getDishes() const { return _dishes; }

    /// Добавить блюдо в набор.
    void addDish(std::shared_ptr<Dish> dish);
    /// Сумма цен блюд без скидки набора.
    double fullPrice() const;
    /// Цена набора со скидкой.
    double totalPrice() const;
    /// Сколько гость экономит, беря набор.
    double savings() const;
    /// Калорийность всего набора (по одной порции каждого блюда).
    double totalCalories() const;
    /// Есть ли в наборе напиток.
    bool hasBeverage() const;

private:
    std::string _title;
    std::vector<std::shared_ptr<Dish>> _dishes;
    double _discountPercent;
};
