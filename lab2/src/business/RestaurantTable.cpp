#include "business/RestaurantTable.h"

#include <string>

#include "exceptions/CulinaryException.h"

RestaurantTable::RestaurantTable(int number, int seats)
    : _number(number), _seats(seats), _guests(0), _isOccupied(false) {
    if (number <= 0 || seats <= 0) {
        throw TableException("У столика должен быть положительный номер и число мест");
    }
}

bool RestaurantTable::canSeat(int guests) const {
    return !_isOccupied && guests > 0 && guests <= _seats;
}

void RestaurantTable::seat(int guests) {
    if (_isOccupied) {
        throw TableException("Столик занят: №" + std::to_string(_number));
    }
    if (guests <= 0 || guests > _seats) {
        throw TableException("За столиком №" + std::to_string(_number) + " только " + std::to_string(_seats) +
                             " мест");
    }
    _guests = guests;
    _isOccupied = true;
}

void RestaurantTable::release() {
    _guests = 0;
    _isOccupied = false;
}
