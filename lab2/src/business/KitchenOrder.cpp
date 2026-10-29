#include "business/KitchenOrder.h"

#include <utility>

#include "exceptions/CulinaryException.h"

KitchenOrder::KitchenOrder(int id, const RestaurantTable& table, bool isUrgent)
    : _id(id), _tableNumber(table.getNumber()), _status(OrderStatus::New), _isUrgent(isUrgent) {}

void KitchenOrder::addDish(std::shared_ptr<Dish> dish) {
    if (_status != OrderStatus::New) {
        throw OrderException("В заказ, который уже готовится, нельзя добавить блюдо");
    }
    if (!dish) {
        throw DishNotFoundException("Нельзя добавить пустое блюдо в заказ");
    }
    _dishes.push_back(std::move(dish));
}

void KitchenOrder::startCooking(const std::string& responsible) {
    if (_status != OrderStatus::New || _dishes.empty()) {
        throw OrderException("Начать готовить можно только новый непустой заказ");
    }
    _assignedCook = responsible;
    _status = OrderStatus::Cooking;
}

void KitchenOrder::markReady() {
    if (_status != OrderStatus::Cooking) {
        throw OrderException("Готовым можно отметить только заказ, который готовится");
    }
    _status = OrderStatus::Ready;
}

void KitchenOrder::cancel() {
    if (_status == OrderStatus::Ready) {
        throw OrderException("Готовый заказ нельзя отменить");
    }
    _status = OrderStatus::Cancelled;
}

double KitchenOrder::total() const {
    double sum = 0.0;
    for (const std::shared_ptr<Dish>& dish : _dishes) {
        sum += dish->getPrice().finalPrice();
    }
    return sum;
}

std::string KitchenOrder::statusName() const {
    switch (_status) {
        case OrderStatus::New:
            return "Новый";
        case OrderStatus::Cooking:
            return "Готовится";
        case OrderStatus::Ready:
            return "Готов";
        default:
            return "Отменён";
    }
}
