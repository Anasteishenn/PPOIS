#include "dishes/MainCourse.h"

MainCourse::MainCourse(const std::string& name, const Recipe& recipe, const PriceTag& price,
                       const PlatingStyle& plating, const std::string& sideDish, bool isSpicy)
    : Dish(name, recipe, price, plating), _sideDish(sideDish), _isSpicy(isSpicy) {}

std::string MainCourse::course() const {
    return "Основное блюдо";
}

std::string MainCourse::serve() const {
    return _sideDish.empty() ? Dish::serve() : Dish::serve() + ", гарнир: " + _sideDish;
}

std::string MainCourse::pairWith(const Beverage& beverage) const {
    bool goodPair = !(_isSpicy && beverage.isHot());
    return getName() + " + " + beverage.getName() + (goodPair ? ": хорошее сочетание" : ": не рекомендуем");
}

std::string MainCourse::spiceWarning() const {
    return _isSpicy ? "Острое блюдо!" : "";
}
