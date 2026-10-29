#pragma once

#include <memory>
#include <string>
#include <vector>

#include "business/RestaurantTable.h"
#include "dishes/Dish.h"

/// Статус заказа. Переходы: New -> Cooking -> Ready; отменить можно, пока заказ не готов.
enum class OrderStatus { New, Cooking, Ready, Cancelled };

/// @brief Заказ на кухню: номер, столик, блюда, статус, ответственный и срочность.
class KitchenOrder {
public:
    KitchenOrder(int id, const RestaurantTable& table, bool isUrgent = false);

    int getId() const { return _id; }
    int getTableNumber() const { return _tableNumber; }
    const std::vector<std::shared_ptr<Dish>>& getDishes() const { return _dishes; }
    OrderStatus getStatus() const { return _status; }
    const std::string& getAssignedCook() const { return _assignedCook; }
    bool isUrgent() const { return _isUrgent; }

    /// Добавить блюдо (только в новый заказ).
    void addDish(std::shared_ptr<Dish> dish);
    /// Начать готовить. Заказ должен быть новым и непустым.
    void startCooking(const std::string& responsible);
    /// Заказ готов. Можно только из статуса Cooking.
    void markReady();
    /// Отменить заказ. Готовый заказ отменить нельзя.
    void cancel();
    /// Сумма заказа.
    double total() const;
    /// Статус словами: «Новый», «Готовится», «Готов», «Отменён».
    std::string statusName() const;

private:
    int _id;
    int _tableNumber;
    std::vector<std::shared_ptr<Dish>> _dishes;
    OrderStatus _status;
    std::string _assignedCook;
    bool _isUrgent;
};
