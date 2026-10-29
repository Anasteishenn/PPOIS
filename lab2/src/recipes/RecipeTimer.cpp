#include "recipes/RecipeTimer.h"

#include "exceptions/CulinaryException.h"

RecipeTimer::RecipeTimer(int durationMinutes)
    : _durationMinutes(durationMinutes), _elapsedMinutes(0), _isRunning(false) {
    if (durationMinutes <= 0) {
        throw RecipeException("Длительность таймера должна быть положительной");
    }
}

void RecipeTimer::start() {
    _isRunning = true;
}

void RecipeTimer::tick(int minutes) {
    if (!_isRunning) {
        throw RecipeException("Таймер не запущен");
    }
    _elapsedMinutes += minutes;
    if (isFinished()) {
        _elapsedMinutes = _durationMinutes;
        _isRunning = false;
    }
}

bool RecipeTimer::isFinished() const {
    return _elapsedMinutes >= _durationMinutes;
}

int RecipeTimer::remaining() const {
    return _durationMinutes - _elapsedMinutes;
}

void RecipeTimer::reset() {
    _elapsedMinutes = 0;
    _isRunning = false;
}
