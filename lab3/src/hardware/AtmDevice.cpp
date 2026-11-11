#include "hardware/AtmDevice.h"

#include "exceptions/AtmException.h"

AtmDevice::AtmDevice(const std::string& serialNumber)
    : _serialNumber(serialNumber), _isWorking(true), _errorCount(0) {
    if (serialNumber.empty()) {
        throw DeviceException("У устройства должен быть серийный номер");
    }
}

bool AtmDevice::selfTest() const {
    return _isWorking;
}

void AtmDevice::reportError(const std::string& error) {
    _isWorking = false;
    _lastError = error;
    ++_errorCount;
}

void AtmDevice::repair() {
    _isWorking = true;
    _lastError.clear();
}

void AtmDevice::service() {
    repair();
}

void AtmDevice::ensureWorking() const {
    if (!_isWorking) {
        throw DeviceException(deviceName() + " неисправно: " + _lastError);
    }
}

bool AtmDevice::needsReplacement() const {
    return _errorCount >= MAX_ERRORS_BEFORE_REPLACEMENT;
}

std::string AtmDevice::statusLine() const {
    std::string state = selfTest() ? "исправно" : "неисправно (" + _lastError + ")";
    return deviceName() + " (" + _serialNumber + "): " + state;
}
