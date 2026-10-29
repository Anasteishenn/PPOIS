#include "ingredients/Allergen.h"

#include "exceptions/CulinaryException.h"

Allergen::Allergen(const std::string& name, int severity, const std::string& description)
    : _name(name), _severity(severity), _description(description) {
    if (name.empty()) {
        throw InvalidIngredientException("Название аллергена не может быть пустым");
    }
    if (severity < MIN_SEVERITY || severity > MAX_SEVERITY) {
        throw InvalidIngredientException("Уровень опасности аллергена должен быть от 1 до 5");
    }
}

bool Allergen::isSevere() const {
    return _severity >= SEVERE_LEVEL;
}

bool Allergen::matches(const std::string& name) const {
    return _name == name;
}

std::string Allergen::warning() const {
    return "Внимание: содержит " + _name + " (опасность " + std::to_string(_severity) + "/" +
           std::to_string(MAX_SEVERITY) + ")";
}
