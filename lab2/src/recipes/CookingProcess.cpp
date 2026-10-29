#include "recipes/CookingProcess.h"

CookingProcess::CookingProcess(const std::string& name, int durationMinutes)
    : _name(name), _durationMinutes(durationMinutes), _timer(durationMinutes) {}

bool CookingProcess::requiresHeat() const {
    return false;
}

std::string CookingProcess::run() {
    _timer.reset();
    _timer.start();
    _timer.tick(_durationMinutes);
    return execute();
}
