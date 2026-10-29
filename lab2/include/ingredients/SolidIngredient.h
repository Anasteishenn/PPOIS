#pragma once

#include "ingredients/Ingredient.h"

/// @brief Твёрдый ингредиент (лук, картофель, мясо). Измеряется в граммах, можно нарезать.
class SolidIngredient : public Ingredient {
public:
    SolidIngredient(const std::string& name, double pricePerGram, const NutritionalValue& nutrition,
                    double pieceWeight);

    double getPieceWeight() const { return _pieceWeight; }
    bool isChopped() const { return _isChopped; }

    std::string unit() const override;
    /// Описание с пометкой «нарезан», если ингредиент уже нарезан.
    std::string describe() const override;
    /// Нарезать ингредиент.
    void chop();
    /// Сколько штук нужно, чтобы набрать заданную массу (с округлением вверх).
    int piecesFor(double grams) const;

private:
    double _pieceWeight;
    bool _isChopped;
};
