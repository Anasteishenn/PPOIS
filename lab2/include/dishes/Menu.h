#pragma once

#include <memory>
#include <string>
#include <vector>

#include "dishes/ComboMeal.h"
#include "dishes/Dish.h"

/// @brief Меню ресторана: блюда и комбо-наборы.
class Menu {
public:
    Menu(const std::string& title, const std::string& season);

    const std::string& getTitle() const { return _title; }
    const std::string& getSeason() const { return _season; }
    const std::vector<std::shared_ptr<Dish>>& getDishes() const { return _dishes; }
    const std::vector<ComboMeal>& getCombos() const { return _combos; }

    /// Добавить блюдо в меню.
    void addDish(std::shared_ptr<Dish> dish);
    /// Убрать блюдо из меню. Бросает DishNotFoundException, если его нет.
    void removeDish(const std::string& dishName);
    /// Найти блюдо по названию. Бросает DishNotFoundException, если его нет.
    std::shared_ptr<Dish> findDish(const std::string& dishName) const;
    /// Добавить комбо-набор.
    void addCombo(const ComboMeal& combo);
    /// Блюда, которые сейчас есть в наличии.
    std::vector<std::shared_ptr<Dish>> availableDishes() const;
    /// Самое дешёвое блюдо. Бросает DishNotFoundException, если меню пустое.
    std::shared_ptr<Dish> cheapestDish() const;
    /// Блюда без указанного аллергена (для гостя с аллергией).
    std::vector<std::shared_ptr<Dish>> withoutAllergen(const std::string& allergenName) const;
    /// Вегетарианские блюда.
    std::vector<std::shared_ptr<Dish>> vegetarianDishes() const;

private:
    std::string _title;
    std::string _season;
    std::vector<std::shared_ptr<Dish>> _dishes;
    std::vector<ComboMeal> _combos;
};
