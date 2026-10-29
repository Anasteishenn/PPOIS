#pragma once

/// @brief Столик в зале: номер, число мест, сколько гостей сидит, занят ли.
class RestaurantTable {
public:
    RestaurantTable(int number, int seats);

    int getNumber() const { return _number; }
    int getSeats() const { return _seats; }
    int getGuests() const { return _guests; }
    bool isOccupied() const { return _isOccupied; }

    /// Можно ли посадить столько гостей (столик свободен и мест хватает).
    bool canSeat(int guests) const;
    /// Посадить гостей. Бросает TableException, если столик занят или мест мало.
    void seat(int guests);
    /// Гости ушли — столик свободен.
    void release();

private:
    int _number;
    int _seats;
    int _guests;
    bool _isOccupied;
};
