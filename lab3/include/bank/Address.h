#pragma once

#include <string>

/// @brief Адрес банкомата: город, улица, дом и место установки (например, «ТЦ Galleria, 1 этаж»).
class Address {
public:
    /// Бросает DeviceException, если город или улица пустые.
    Address(const std::string& city, const std::string& street, const std::string& building,
            const std::string& place = "");

    const std::string& getCity() const { return _city; }
    const std::string& getStreet() const { return _street; }
    const std::string& getBuilding() const { return _building; }
    const std::string& getPlace() const { return _place; }

    /// Адрес одной строкой: «г. Минск, ул. Ленина, 5 (ТЦ Galleria)».
    std::string format() const;
    /// В одном ли городе два адреса.
    bool isSameCity(const Address& other) const;
    /// Стоит ли банкомат внутри помещения (указано место установки).
    bool isIndoor() const;

private:
    std::string _city;
    std::string _street;
    std::string _building;
    std::string _place;
};
