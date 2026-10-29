#include "appliances/FoodProcessor.h"

#include <algorithm>

#include "Format.h"
#include "exceptions/CulinaryException.h"

FoodProcessor::FoodProcessor(const std::string& model, int powerWatts, double maxLoadGrams)
    : SafeProcessor(model, powerWatts, maxLoadGrams) {}

std::string FoodProcessor::type() const {
    return "Кухонный комбайн";
}

void FoodProcessor::addAttachment(const std::string& attachment) {
    _attachments.push_back(attachment);
}

void FoodProcessor::selectAttachment(const std::string& attachment) {
    if (std::find(_attachments.begin(), _attachments.end(), attachment) == _attachments.end()) {
        throw ApplianceException("Нет такой насадки: " + attachment);
    }
    _currentAttachment = attachment;
}

std::string FoodProcessor::process(SolidIngredient& ingredient, double grams, int minutes) {
    if (_currentAttachment.empty()) {
        throw ApplianceException("Насадка не выбрана");
    }
    safeRun(grams, minutes);
    ingredient.chop();
    return ingredient.getName() + " (" + format::number(grams) + " г) обработан насадкой «" + _currentAttachment +
           "»";
}
