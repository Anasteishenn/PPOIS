#include "appliances/MeatGrinder.h"

#include <cmath>

#include "Format.h"
#include "exceptions/CulinaryException.h"

MeatGrinder::MeatGrinder(const std::string& model, int powerWatts, double maxLoadGrams, double gramsPerMinute,
                         double plateHoleMm)
    : SafeProcessor(model, powerWatts, maxLoadGrams), _gramsPerMinute(gramsPerMinute), _plateHoleMm(plateHoleMm) {
    if (gramsPerMinute <= 0 || plateHoleMm <= 0) {
        throw ApplianceException("Скорость мясорубки и размер решётки должны быть положительными");
    }
}

std::string MeatGrinder::type() const {
    return "Мясорубка";
}

int MeatGrinder::grindingMinutes(double grams) const {
    return static_cast<int>(std::ceil(grams / _gramsPerMinute));
}

std::string MeatGrinder::grind(const SolidIngredient& ingredient, double grams) {
    int minutes = grindingMinutes(grams);
    safeRun(grams, minutes);
    return "Фарш: " + ingredient.getName() + ", " + format::number(grams) + " г за " + std::to_string(minutes) +
           " мин (решётка " + format::number(_plateHoleMm) + " мм)";
}

void MeatGrinder::changePlate(double holeMm) {
    if (holeMm <= 0) {
        throw ApplianceException("Размер отверстий должен быть положительным");
    }
    _plateHoleMm = holeMm;
}
