#pragma once

#include <string>
#include <vector>

#include "hardware/AtmDevice.h"

/// @brief Камера наблюдения: снимает подозрительные события (изъятие карты, мошенничество).
class SecurityCamera : public AtmDevice {
public:
    /// Сколько снимков помещается в память по умолчанию.
    static constexpr int DEFAULT_STORAGE_LIMIT = 100;

    explicit SecurityCamera(const std::string& serialNumber, int storageLimit = DEFAULT_STORAGE_LIMIT);

    bool isRecording() const { return _isRecording; }
    const std::vector<std::string>& getSnapshots() const { return _snapshots; }

    std::string deviceName() const override;
    /// Включить запись. Бросает DeviceException, если камера неисправна.
    void startRecording();
    /// Выключить запись.
    void stopRecording();
    /// Сделать снимок события. Бросает DeviceException, если запись выключена. Старые снимки удаляются.
    std::string takeSnapshot(const std::string& event);
    /// Удалить все снимки. Возвращает, сколько удалено.
    int clearSnapshots();

private:
    bool _isRecording;
    std::vector<std::string> _snapshots;
    int _storageLimit;
    int _totalSnapshots;
};
