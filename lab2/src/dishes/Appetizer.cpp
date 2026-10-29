#include "dishes/Appetizer.h"

#include "exceptions/CulinaryException.h"

Appetizer::Appetizer(const std::string& name, const Recipe& recipe, const PriceTag& price,
                     const PlatingStyle& plating, bool isCold, int pieces)
    : Dish(name, recipe, price, plating), _isCold(isCold), _pieces(pieces) {}

std::string Appetizer::course() const {
    return "Закуска";
}

std::string Appetizer::serve() const {
    return (_isCold ? "Холодная закуска. " : "Горячая закуска. ") + Dish::serve();
}

int Appetizer::piecesPerPerson(int people) const {
    if (people <= 0) {
        throw TableException("Число гостей должно быть положительным");
    }
    return _pieces / people;
}
