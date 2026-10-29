#include "ingredients/Supplier.h"

#include "Format.h"
#include "exceptions/CulinaryException.h"

namespace {
constexpr double AVERAGE_DIVIDER = 2.0;
}

Supplier::Supplier(const std::string& name, const std::string& phone, const std::string& city, int deliveryDays,
                   double rating)
    : _name(name), _phone(phone), _city(city), _deliveryDays(deliveryDays), _rating(rating) {
    if (name.empty() || deliveryDays < 0) {
        throw InvalidIngredientException("У поставщика должно быть имя и неотрицательный срок доставки");
    }
    if (rating < MIN_RATING || rating > MAX_RATING) {
        throw InvalidRatingException("Рейтинг поставщика должен быть от 0 до 5");
    }
}

void Supplier::rate(double score) {
    if (score < MIN_RATING || score > MAX_RATING) {
        throw InvalidRatingException("Оценка поставщика должна быть от 0 до 5");
    }
    _rating = (_rating + score) / AVERAGE_DIVIDER;
}

bool Supplier::isReliable() const {
    return _rating >= RELIABLE_RATING;
}

int Supplier::deliveryDay(int today) const {
    return today + _deliveryDays;
}

std::string Supplier::contactCard() const {
    return _name + ", " + _city + ", тел. " + _phone + ", рейтинг " + format::number(_rating);
}
