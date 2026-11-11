#pragma once

#include "hardware/AtmDevice.h"
#include "operations/Receipt.h"

/// @brief Принтер чеков: печатает чек, следит за запасом бумаги.
class ReceiptPrinter : public AtmDevice {
public:
    static constexpr double PERCENT = 100.0;
    /// Бумага считается заканчивающейся, если её осталось меньше 10%.
    static constexpr double LOW_PAPER_PERCENT = 10.0;

    /// Бросает DeviceException, если запас бумаги <= 0.
    ReceiptPrinter(const std::string& serialNumber, int paperCapacity);

    int getPaperLeft() const { return _paperLeft; }
    int getPaperCapacity() const { return _paperCapacity; }
    int getPrintedCount() const { return _printedCount; }

    std::string deviceName() const override;
    /// Напечатать чек. Если бумага кончилась — принтер ломается и бросает DeviceException.
    std::string print(const Receipt& receipt);
    /// Заправить новый рулон бумаги (принтер снова работает).
    void refillPaper();
    /// Обслуживание принтера — заправка бумаги.
    void service() override;
    /// Заканчивается ли бумага.
    bool isPaperLow() const;

private:
    int _paperLeft;
    int _paperCapacity;
    int _printedCount;
};
