#include "hardware/ReceiptPrinter.h"

#include "exceptions/AtmException.h"

ReceiptPrinter::ReceiptPrinter(const std::string& serialNumber, int paperCapacity)
    : AtmDevice(serialNumber), _paperLeft(paperCapacity), _paperCapacity(paperCapacity), _printedCount(0) {
    if (paperCapacity <= 0) {
        throw DeviceException("Запас бумаги должен быть больше нуля");
    }
}

std::string ReceiptPrinter::deviceName() const {
    return "Принтер чеков";
}

std::string ReceiptPrinter::print(const Receipt& receipt) {
    ensureWorking();
    if (_paperLeft == 0) {
        reportError("закончилась бумага");
        throw DeviceException("В принтере закончилась бумага");
    }
    --_paperLeft;
    ++_printedCount;
    return receipt.text();
}

void ReceiptPrinter::refillPaper() {
    _paperLeft = _paperCapacity;
    repair();
}

void ReceiptPrinter::service() {
    refillPaper();
}

bool ReceiptPrinter::isPaperLow() const {
    return _paperLeft * PERCENT / _paperCapacity < LOW_PAPER_PERCENT;
}
