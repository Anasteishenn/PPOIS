#include "ingredients/IngredientPackage.h"

#include <utility>

#include "Format.h"
#include "exceptions/CulinaryException.h"

IngredientPackage::IngredientPackage(std::shared_ptr<Ingredient> ingredient, double amountPerPackage,
                                     int packageCount, const std::string& packageType)
    : _ingredient(std::move(ingredient)),
      _amountPerPackage(amountPerPackage),
      _packageCount(packageCount),
      _packageType(packageType) {
    if (!_ingredient || amountPerPackage <= 0 || packageCount < 0) {
        throw InvalidIngredientException("Некорректная упаковка ингредиента");
    }
}

double IngredientPackage::totalAmount() const {
    return _amountPerPackage * _packageCount;
}

double IngredientPackage::totalPrice() const {
    return _ingredient->costFor(totalAmount());
}

void IngredientPackage::takePackages(int count) {
    if (count <= 0 || count > _packageCount) {
        throw InsufficientStockException("Нельзя взять столько упаковок: " + _ingredient->getName());
    }
    _packageCount -= count;
}

std::string IngredientPackage::label() const {
    return std::to_string(_packageCount) + " x " + _packageType + " по " + format::number(_amountPerPackage) + " " +
           _ingredient->unit() + ": " + _ingredient->getName();
}
