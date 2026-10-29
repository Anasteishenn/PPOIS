#pragma once

#include <deque>

#include "business/KitchenOrder.h"

/// @brief Очередь заказов на кухне. Срочные заказы встают в начало очереди.
class OrderQueue {
public:
    explicit OrderQueue(int maxSize);

    const std::deque<KitchenOrder>& getOrders() const { return _orders; }
    int getProcessedCount() const { return _processedCount; }

    /// Поставить заказ в очередь. Бросает OrderException, если очередь переполнена.
    void enqueue(const KitchenOrder& order);
    /// Забрать следующий заказ. Бросает OrderException, если очередь пуста.
    KitchenOrder next();
    /// Найти заказ в очереди по номеру. Бросает OrderException, если не найден.
    const KitchenOrder& find(int orderId) const;
    /// Сколько заказов ждёт.
    int pendingCount() const;
    /// Заполнена ли очередь.
    bool isFull() const;
    /// Пуста ли очередь.
    bool isEmpty() const;

private:
    std::deque<KitchenOrder> _orders;
    int _maxSize;
    int _processedCount;
};
