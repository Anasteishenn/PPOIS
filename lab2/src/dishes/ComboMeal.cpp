#include "dishes/ComboMeal.h"

#include <utility>

#include "dishes/Beverage.h"
#include "dishes/PriceTag.h"
#include "exceptions/CulinaryException.h"

ComboMeal::ComboMeal(const std::string& title, double discountPercent)
    : _title(title), _discountPercent(discountPercent) {
    if (discountPercent < 0 || discountPercent > PriceTag::MAX_DISCOUNT) {
        throw InvalidPriceException("Скидка набора должна быть от 0 до 90%");
    }
}

void ComboMeal::addDish(std::shared_ptr<Dish> dish) {
    if (!dish) {
        throw DishNotFoundException("Нельзя добавить пустое блюдо в набор");
    }
    _dishes.push_back(std::move(dish));
}

double ComboMeal::fullPrice() const {
    double sum = 0.0;
    for (const std::shared_ptr<Dish>& dish : _dishes) {
        sum += dish->getPrice().finalPrice();
    }
    return sum;
}

double ComboMeal::totalPrice() const {
    return fullPrice() * (PriceTag::PERCENT - _discountPercent) / PriceTag::PERCENT;
}

double ComboMeal::savings() const {
    return fullPrice() - totalPrice();
}

double ComboMeal::totalCalories() const {
    double sum = 0.0;
    for (const std::shared_ptr<Dish>& dish : _dishes) {
        sum += dish->caloriesPerServing();
    }
    return sum;
}

bool ComboMeal::hasBeverage() const {
    for (const std::shared_ptr<Dish>& dish : _dishes) {
        if (std::dynamic_pointer_cast<Beverage>(dish)) {
            return true;
        }
    }
    return false;
}
