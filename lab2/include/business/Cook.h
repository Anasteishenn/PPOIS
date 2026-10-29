#pragma once

#include <string>
#include <vector>

#include "appliances/KitchenAppliance.h"
#include "business/KitchenStaff.h"
#include "dishes/Dish.h"

/// @brief Повар: работает на своём участке (станции) и специализируется на разделе меню.
class Cook : public KitchenStaff {
public:
    /// specialty — раздел меню («Закуска», «Десерт»...). Пустая строка — готовит всё.
    Cook(const std::string& name, int experienceYears, double hourlyRate, const std::string& station,
         const std::string& specialty);

    const std::string& getStation() const { return _station; }
    const std::string& getSpecialty() const { return _specialty; }
    int getDishesCooked() const { return _dishesCooked; }

    std::string role() const override;
    /// Умеет ли повар готовить это блюдо (совпадает ли раздел меню со специализацией).
    bool canCook(const Dish& dish) const;
    /// Приготовить блюдо. Бросает StaffException, если блюдо не по специализации.
    std::vector<std::string> cook(const Dish& dish);
    /// Включить прибор и поработать на нём заданное число минут.
    std::string useAppliance(KitchenAppliance& appliance, int minutes) const;

private:
    std::string _station;
    std::string _specialty;
    int _dishesCooked;
};
