#include "dishes/PlatingStyle.h"

PlatingStyle::PlatingStyle(const std::string& name, const std::string& plateType, const std::string& garnish)
    : _name(name), _plateType(plateType), _garnish(garnish) {}

std::string PlatingStyle::describe() const {
    std::string text = "подача «" + _name + "», " + _plateType;
    return hasGarnish() ? text + ", украшение: " + _garnish : text;
}

bool PlatingStyle::hasGarnish() const {
    return !_garnish.empty();
}
