#include "dishes/Beverage.h"

#include "exceptions/CulinaryException.h"

Beverage::Beverage(const std::string& name, const Recipe& recipe, const PriceTag& price,
                   const PlatingStyle& plating, int volumeMl, bool isAlcoholic, int servingTemperature)
    : Dish(name, recipe, price, plating),
      _volumeMl(volumeMl),
      _isAlcoholic(isAlcoholic),
      _servingTemperature(servingTemperature) {
    if (volumeMl <= 0) {
        throw DishNotFoundException("Объём напитка должен быть положительным");
    }
}

std::string Beverage::course() const {
    return "Напиток";
}

std::string Beverage::serve() const {
    std::string text = Dish::serve() + ", " + std::to_string(_volumeMl) + " мл";
    return isHot() ? text + ", горячий" : text;
}

void Beverage::checkAge(int guestAge) const {
    if (_isAlcoholic && guestAge < ADULT_AGE) {
        throw AgeRestrictionException("Алкоголь продаётся только с 18 лет: " + getName());
    }
}

bool Beverage::isHot() const {
    return _servingTemperature >= HOT_TEMPERATURE;
}
