#include "dishes/Dessert.h"

Dessert::Dessert(const std::string& name, const Recipe& recipe, const PriceTag& price, const PlatingStyle& plating,
                 double sugarGrams, bool isFrozen)
    : Dish(name, recipe, price, plating), _sugarGrams(sugarGrams), _isFrozen(isFrozen) {}

std::string Dessert::course() const {
    return "Десерт";
}

std::string Dessert::serve() const {
    return _isFrozen ? Dish::serve() + ", подаётся охлаждённым" : Dish::serve();
}

bool Dessert::isSugarFree() const {
    return _sugarGrams < SUGAR_FREE_LIMIT;
}
