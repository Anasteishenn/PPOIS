#include "business/Bill.h"

#include "exceptions/CulinaryException.h"

Bill::Bill(const KitchenOrder& order, double tipPercent)
    : _order(order), _tipPercent(tipPercent), _paidAmount(0.0), _isPaid(false) {
    if (order.getStatus() != OrderStatus::Ready) {
        throw OrderException("Счёт выставляется только за готовый заказ");
    }
    if (tipPercent < 0 || tipPercent > MAX_TIP_PERCENT) {
        throw PaymentException("Чаевые должны быть от 0 до 50%");
    }
}

double Bill::subtotal() const {
    return _order.total();
}

double Bill::tip() const {
    return subtotal() * _tipPercent / PERCENT;
}

double Bill::total() const {
    return subtotal() + tip();
}

void Bill::pay(double amount) {
    if (_isPaid) {
        throw PaymentException("Счёт уже оплачен");
    }
    if (amount < total()) {
        throw PaymentException("Недостаточно денег для оплаты");
    }
    _paidAmount = amount;
    _isPaid = true;
}

double Bill::change() const {
    return _isPaid ? _paidAmount - total() : 0.0;
}

double Bill::splitBetween(int people) const {
    if (people <= 0) {
        throw PaymentException("Делить счёт можно только на положительное число гостей");
    }
    return total() / people;
}
