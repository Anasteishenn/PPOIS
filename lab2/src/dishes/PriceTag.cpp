#include "dishes/PriceTag.h"

#include "Format.h"
#include "exceptions/CulinaryException.h"

PriceTag::PriceTag(double basePrice, const std::string& currency)
    : _basePrice(basePrice), _discountPercent(0.0), _currency(currency) {
    if (basePrice < 0) {
        throw InvalidPriceException("Цена не может быть отрицательной");
    }
}

double PriceTag::finalPrice() const {
    return _basePrice * (PERCENT - _discountPercent) / PERCENT;
}

void PriceTag::applyDiscount(double percent) {
    if (percent < 0 || percent > MAX_DISCOUNT) {
        throw InvalidPriceException("Скидка должна быть от 0 до 90%");
    }
    _discountPercent = percent;
}

std::string PriceTag::format() const {
    std::string text = format::number(finalPrice()) + " " + _currency;
    if (_discountPercent > 0) {
        text += " (скидка " + format::number(_discountPercent) + "%)";
    }
    return text;
}
