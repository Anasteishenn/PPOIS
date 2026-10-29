#include "recipes/MechanicalProcess.h"

#include "exceptions/CulinaryException.h"

MechanicalProcess::MechanicalProcess(const std::string& name, int durationMinutes, const std::string& tool,
                                     int speed)
    : CookingProcess(name, durationMinutes), _tool(tool), _speed(speed) {
    if (speed < 0) {
        throw RecipeException("Скорость обработки не может быть отрицательной");
    }
}

std::string MechanicalProcess::execute() const {
    return getName() + ": " + _tool + ", " + std::to_string(_speed) + " об/мин, " + std::to_string(getDuration()) +
           " мин";
}

bool MechanicalProcess::isFast() const {
    return _speed >= FAST_SPEED;
}
