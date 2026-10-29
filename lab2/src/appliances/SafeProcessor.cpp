#include "appliances/SafeProcessor.h"

#include "Format.h"
#include "exceptions/CulinaryException.h"

SafeProcessor::SafeProcessor(const std::string& model, int powerWatts, double maxLoadGrams)
    : KitchenAppliance(model, powerWatts), _lidClosed(false), _maxLoadGrams(maxLoadGrams) {
    if (maxLoadGrams <= 0) {
        throw ApplianceException("Максимальная загрузка должна быть положительной");
    }
}

void SafeProcessor::closeLid() {
    _lidClosed = true;
}

void SafeProcessor::openLid() {
    _lidClosed = false;
}

void SafeProcessor::checkReady(double loadGrams) const {
    if (!isOn()) {
        throw ApplianceException("Прибор выключен: " + getModel());
    }
    if (!_lidClosed) {
        throw ApplianceException("Крышка открыта — работать опасно");
    }
    if (loadGrams > _maxLoadGrams) {
        throw ApplianceException("Перегрузка: максимум " + format::number(_maxLoadGrams) + " г");
    }
}

void SafeProcessor::safeRun(double loadGrams, int minutes) {
    checkReady(loadGrams);
    use(minutes);
}
