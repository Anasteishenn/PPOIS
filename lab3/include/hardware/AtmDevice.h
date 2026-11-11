#pragma once

#include <string>

/// @brief Абстрактное устройство банкомата: серийный номер, исправность, счётчик ошибок.
class AtmDevice {
public:
    /// После стольких ошибок устройство пора менять, а не чинить.
    static constexpr int MAX_ERRORS_BEFORE_REPLACEMENT = 5;

    /// Бросает DeviceException, если серийный номер пустой.
    explicit AtmDevice(const std::string& serialNumber);
    virtual ~AtmDevice() = default;

    const std::string& getSerialNumber() const { return _serialNumber; }
    bool isWorking() const { return _isWorking; }
    int getErrorCount() const { return _errorCount; }
    const std::string& getLastError() const { return _lastError; }

    /// Название устройства («Картоприёмник», «Принтер чеков»...).
    virtual std::string deviceName() const = 0;
    /// Самопроверка устройства. По умолчанию — исправно ли оно.
    virtual bool selfTest() const;
    /// Зафиксировать неисправность: устройство перестаёт работать.
    void reportError(const std::string& error);
    /// Починить устройство.
    void repair();
    /// Обслуживание техником. По умолчанию — просто ремонт; принтер ещё и заправляет бумагу.
    virtual void service();
    /// Бросает DeviceException, если устройство неисправно.
    void ensureWorking() const;
    /// Пора ли заменить устройство (слишком много поломок).
    bool needsReplacement() const;
    /// Строка состояния: «Принтер чеков (ATM1-PR): исправно».
    std::string statusLine() const;

private:
    std::string _serialNumber;
    bool _isWorking;
    int _errorCount;
    std::string _lastError;
};
