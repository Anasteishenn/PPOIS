#include "cards/PinCode.h"

#include <cctype>
#include <functional>

#include "exceptions/AtmException.h"

PinCode::PinCode(const std::string& pin, int maxAttempts)
    : _hash(hashOf(pin)), _failedAttempts(0), _maxAttempts(maxAttempts) {
    if (!isValidFormat(pin)) {
        throw InvalidPinException("PIN-код должен состоять из 4 цифр");
    }
    if (maxAttempts <= 0) {
        throw InvalidPinException("Число попыток должно быть больше нуля");
    }
}

bool PinCode::isValidFormat(const std::string& pin) {
    if (pin.size() != PIN_LENGTH) {
        return false;
    }
    for (char symbol : pin) {
        if (!std::isdigit(static_cast<unsigned char>(symbol))) {
            return false;
        }
    }
    return true;
}

bool PinCode::matches(const std::string& pin) const {
    return hashOf(pin) == _hash;
}

void PinCode::verify(const std::string& pin) {
    if (isLocked()) {
        throw PinLockedException("PIN-код заблокирован: попытки закончились");
    }
    if (matches(pin)) {
        _failedAttempts = 0;
        return;
    }
    ++_failedAttempts;
    if (isLocked()) {
        throw PinLockedException("PIN-код заблокирован: попытки закончились");
    }
    throw InvalidPinException("Неверный PIN-код. Осталось попыток: " + std::to_string(attemptsLeft()));
}

bool PinCode::isLocked() const {
    return _failedAttempts >= _maxAttempts;
}

int PinCode::attemptsLeft() const {
    return isLocked() ? 0 : _maxAttempts - _failedAttempts;
}

void PinCode::change(const std::string& oldPin, const std::string& newPin) {
    verify(oldPin);
    if (!isValidFormat(newPin)) {
        throw InvalidPinException("Новый PIN-код должен состоять из 4 цифр");
    }
    if (newPin == oldPin) {
        throw InvalidPinException("Новый PIN-код должен отличаться от старого");
    }
    _hash = hashOf(newPin);
}

void PinCode::reset() {
    _failedAttempts = 0;
}

std::size_t PinCode::hashOf(const std::string& pin) {
    return std::hash<std::string>{}(pin);
}
