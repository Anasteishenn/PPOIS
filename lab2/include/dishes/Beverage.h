#pragma once

#include "dishes/Dish.h"

/// @brief Напиток: объём, алкогольный или нет, температура подачи.
class Beverage : public Dish {
public:
    /// С этого возраста можно заказывать алкоголь.
    static constexpr int ADULT_AGE = 18;
    /// С этой температуры напиток считается горячим.
    static constexpr int HOT_TEMPERATURE = 60;

    Beverage(const std::string& name, const Recipe& recipe, const PriceTag& price, const PlatingStyle& plating,
             int volumeMl, bool isAlcoholic, int servingTemperature);

    int getVolumeMl() const { return _volumeMl; }
    bool isAlcoholic() const { return _isAlcoholic; }
    int getServingTemperature() const { return _servingTemperature; }

    std::string course() const override;
    std::string serve() const override;
    /// Проверить возраст гостя. Бросает AgeRestrictionException, если алкоголь и гость младше ADULT_AGE.
    void checkAge(int guestAge) const;
    /// Горячий ли напиток.
    bool isHot() const;

private:
    int _volumeMl;
    bool _isAlcoholic;
    int _servingTemperature;
};
