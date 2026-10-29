#pragma once

#include <string>

/// @brief Поставщик продуктов: контакты, рейтинг, срок доставки.
class Supplier {
public:
    static constexpr double MIN_RATING = 0.0;
    static constexpr double MAX_RATING = 5.0;
    /// С таким рейтингом и выше поставщик считается надёжным.
    static constexpr double RELIABLE_RATING = 4.0;

    Supplier(const std::string& name, const std::string& phone, const std::string& city, int deliveryDays,
             double rating = MAX_RATING);

    const std::string& getName() const { return _name; }
    double getRating() const { return _rating; }
    int getDeliveryDays() const { return _deliveryDays; }

    /// Учитывает новую оценку: рейтинг становится средним старого и новой оценки.
    void rate(double score);
    /// Надёжный ли поставщик (рейтинг >= RELIABLE_RATING).
    bool isReliable() const;
    /// В какой день придёт поставка, если заказать сегодня.
    int deliveryDay(int today) const;
    /// Строка с контактами для списка покупок.
    std::string contactCard() const;

private:
    std::string _name;
    std::string _phone;
    std::string _city;
    int _deliveryDays;
    double _rating;
};
