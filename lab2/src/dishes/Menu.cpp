#include "dishes/Menu.h"

#include <utility>

#include "exceptions/CulinaryException.h"

Menu::Menu(const std::string& title, const std::string& season) : _title(title), _season(season) {}

void Menu::addDish(std::shared_ptr<Dish> dish) {
    if (!dish) {
        throw DishNotFoundException("Нельзя добавить пустое блюдо в меню");
    }
    _dishes.push_back(std::move(dish));
}

void Menu::removeDish(const std::string& dishName) {
    for (auto it = _dishes.begin(); it != _dishes.end(); ++it) {
        if ((*it)->getName() == dishName) {
            _dishes.erase(it);
            return;
        }
    }
    throw DishNotFoundException("В меню нет блюда: " + dishName);
}

std::shared_ptr<Dish> Menu::findDish(const std::string& dishName) const {
    for (const std::shared_ptr<Dish>& dish : _dishes) {
        if (dish->getName() == dishName) {
            return dish;
        }
    }
    throw DishNotFoundException("В меню нет блюда: " + dishName);
}

void Menu::addCombo(const ComboMeal& combo) {
    _combos.push_back(combo);
}

std::vector<std::shared_ptr<Dish>> Menu::availableDishes() const {
    std::vector<std::shared_ptr<Dish>> result;
    for (const std::shared_ptr<Dish>& dish : _dishes) {
        if (dish->isAvailable()) {
            result.push_back(dish);
        }
    }
    return result;
}

std::shared_ptr<Dish> Menu::cheapestDish() const {
    if (_dishes.empty()) {
        throw DishNotFoundException("Меню пустое");
    }
    std::shared_ptr<Dish> cheapest = _dishes.front();
    for (const std::shared_ptr<Dish>& dish : _dishes) {
        if (dish->getPrice().finalPrice() < cheapest->getPrice().finalPrice()) {
            cheapest = dish;
        }
    }
    return cheapest;
}

std::vector<std::shared_ptr<Dish>> Menu::withoutAllergen(const std::string& allergenName) const {
    std::vector<std::shared_ptr<Dish>> result;
    for (const std::shared_ptr<Dish>& dish : _dishes) {
        if (!dish->hasAllergen(allergenName)) {
            result.push_back(dish);
        }
    }
    return result;
}

std::vector<std::shared_ptr<Dish>> Menu::vegetarianDishes() const {
    std::vector<std::shared_ptr<Dish>> result;
    for (const std::shared_ptr<Dish>& dish : _dishes) {
        if (dish->isVegetarian()) {
            result.push_back(dish);
        }
    }
    return result;
}
