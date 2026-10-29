#include "business/OrderQueue.h"

#include <string>

#include "exceptions/CulinaryException.h"

OrderQueue::OrderQueue(int maxSize) : _maxSize(maxSize), _processedCount(0) {
    if (maxSize <= 0) {
        throw OrderException("Размер очереди должен быть положительным");
    }
}

void OrderQueue::enqueue(const KitchenOrder& order) {
    if (isFull()) {
        throw OrderException("Очередь заказов переполнена");
    }
    if (order.isUrgent()) {
        _orders.push_front(order);
    } else {
        _orders.push_back(order);
    }
}

KitchenOrder OrderQueue::next() {
    if (isEmpty()) {
        throw OrderException("Очередь заказов пуста");
    }
    KitchenOrder order = _orders.front();
    _orders.pop_front();
    ++_processedCount;
    return order;
}

const KitchenOrder& OrderQueue::find(int orderId) const {
    for (const KitchenOrder& order : _orders) {
        if (order.getId() == orderId) {
            return order;
        }
    }
    throw OrderException("Заказ не найден: №" + std::to_string(orderId));
}

int OrderQueue::pendingCount() const {
    return static_cast<int>(_orders.size());
}

bool OrderQueue::isFull() const {
    return pendingCount() >= _maxSize;
}

bool OrderQueue::isEmpty() const {
    return _orders.empty();
}
