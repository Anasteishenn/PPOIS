#pragma once

#include "dishes/Dish.h"

/// @brief Закуска: холодная или горячая, из нескольких кусочков, которые можно разделить.
class Appetizer : public Dish {
public:
    Appetizer(const std::string& name, const Recipe& recipe, const PriceTag& price, const PlatingStyle& plating,
              bool isCold, int pieces);

    bool isCold() const { return _isCold; }
    int getPieces() const { return _pieces; }

    std::string course() const override;
    std::string serve() const override;
    /// Сколько кусочков достанется каждому, если делить на компанию.
    int piecesPerPerson(int people) const;

private:
    bool _isCold;
    int _pieces;
};
