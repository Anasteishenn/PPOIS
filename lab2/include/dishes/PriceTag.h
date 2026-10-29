#pragma once

#include <string>

/// @brief Ценник блюда: базовая цена, скидка в процентах и валюта.
class PriceTag {
public:
    /// Максимально допустимая скидка, %.
    static constexpr double MAX_DISCOUNT = 90.0;
    static constexpr double PERCENT = 100.0;

    explicit PriceTag(double basePrice, const std::string& currency = "BYN");

    double getBasePrice() const { return _basePrice; }
    double getDiscountPercent() const { return _discountPercent; }
    const std::string& getCurrency() const { return _currency; }

    /// Цена с учётом скидки.
    double finalPrice() const;
    /// Установить скидку (от 0 до MAX_DISCOUNT процентов).
    void applyDiscount(double percent);
    /// Текст ценника, например «12.50 BYN» или «10.00 BYN (скидка 20.00%)».
    std::string format() const;

private:
    double _basePrice;
    double _discountPercent;
    std::string _currency;
};
