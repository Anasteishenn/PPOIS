#pragma once

#include "business/KitchenOrder.h"

/// @brief Счёт за готовый заказ: сумма, чаевые, оплата и сдача.
class Bill {
public:
    /// Максимальные чаевые, %.
    static constexpr double MAX_TIP_PERCENT = 50.0;
    static constexpr double PERCENT = 100.0;

    /// Счёт можно выставить только за готовый заказ.
    Bill(const KitchenOrder& order, double tipPercent);

    const KitchenOrder& getOrder() const { return _order; }
    bool isPaid() const { return _isPaid; }
    double getPaidAmount() const { return _paidAmount; }

    /// Сумма блюд.
    double subtotal() const;
    /// Чаевые.
    double tip() const;
    /// Итого к оплате.
    double total() const;
    /// Оплатить. Бросает PaymentException, если денег мало или счёт уже оплачен.
    void pay(double amount);
    /// Сдача (0, если ещё не оплачено).
    double change() const;
    /// Сколько платит каждый, если делить счёт поровну.
    double splitBetween(int people) const;

private:
    KitchenOrder _order;
    double _tipPercent;
    double _paidAmount;
    bool _isPaid;
};
