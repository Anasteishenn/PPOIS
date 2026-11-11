#include "bank/Address.h"

#include "exceptions/AtmException.h"

Address::Address(const std::string& city, const std::string& street, const std::string& building,
                 const std::string& place)
    : _city(city), _street(street), _building(building), _place(place) {
    if (city.empty() || street.empty()) {
        throw DeviceException("У адреса банкомата должны быть город и улица");
    }
}

std::string Address::format() const {
    std::string text = "г. " + _city + ", " + _street + ", " + _building;
    if (isIndoor()) {
        text += " (" + _place + ")";
    }
    return text;
}

bool Address::isSameCity(const Address& other) const {
    return _city == other._city;
}

bool Address::isIndoor() const {
    return !_place.empty();
}
