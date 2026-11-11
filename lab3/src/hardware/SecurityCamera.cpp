#include "hardware/SecurityCamera.h"

#include "exceptions/AtmException.h"

SecurityCamera::SecurityCamera(const std::string& serialNumber, int storageLimit)
    : AtmDevice(serialNumber), _isRecording(false), _storageLimit(storageLimit), _totalSnapshots(0) {}

std::string SecurityCamera::deviceName() const {
    return "Камера наблюдения";
}

void SecurityCamera::startRecording() {
    ensureWorking();
    _isRecording = true;
}

void SecurityCamera::stopRecording() {
    _isRecording = false;
}

std::string SecurityCamera::takeSnapshot(const std::string& event) {
    if (!_isRecording) {
        throw DeviceException("Камера не ведёт запись");
    }
    ++_totalSnapshots;
    std::string snapshot = "Снимок #" + std::to_string(_totalSnapshots) + ": " + event;
    _snapshots.push_back(snapshot);
    if (static_cast<int>(_snapshots.size()) > _storageLimit) {
        _snapshots.erase(_snapshots.begin());
    }
    return snapshot;
}

int SecurityCamera::clearSnapshots() {
    int removed = static_cast<int>(_snapshots.size());
    _snapshots.clear();
    return removed;
}
