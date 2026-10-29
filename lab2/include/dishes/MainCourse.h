#pragma once

#include "dishes/Beverage.h"
#include "dishes/Dish.h"

/// @brief Основное блюдо: с гарниром, может быть острым.
class MainCourse : public Dish {
public:
    MainCourse(const std::string& name, const Recipe& recipe, const PriceTag& price, const PlatingStyle& plating,
               const std::string& sideDish, bool isSpicy);

    const std::string& getSideDish() const { return _sideDish; }
    bool isSpicy() const { return _isSpicy; }

    std::string course() const override;
    std::string serve() const override;
    /// Рекомендация официанта: подходит ли напиток к блюду (острое не советуют с горячим напитком).
    std::string pairWith(const Beverage& beverage) const;
    /// Предупреждение для меню, если блюдо острое (иначе пустая строка).
    std::string spiceWarning() const;

private:
    std::string _sideDish;
    bool _isSpicy;
};
