#pragma once

#include <string>
#include <vector>

#include "appliances/SafeProcessor.h"
#include "ingredients/SolidIngredient.h"

/// @brief Кухонный комбайн: набор насадок (тёрка, нож, шинковка), режет твёрдые продукты.
class FoodProcessor : public SafeProcessor {
public:
    FoodProcessor(const std::string& model, int powerWatts, double maxLoadGrams);

    const std::vector<std::string>& getAttachments() const { return _attachments; }
    const std::string& getCurrentAttachment() const { return _currentAttachment; }

    std::string type() const override;
    /// Добавить насадку в комплект.
    void addAttachment(const std::string& attachment);
    /// Поставить насадку. Бросает ApplianceException, если такой насадки нет.
    void selectAttachment(const std::string& attachment);
    /// Нарезать продукт текущей насадкой (продукт становится нарезанным).
    std::string process(SolidIngredient& ingredient, double grams, int minutes);

private:
    std::vector<std::string> _attachments;
    std::string _currentAttachment;
};
