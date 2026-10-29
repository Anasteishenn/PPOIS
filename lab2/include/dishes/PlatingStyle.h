#pragma once

#include <string>

/// @brief Способ подачи блюда: название стиля, тип тарелки и украшение.
class PlatingStyle {
public:
    PlatingStyle(const std::string& name, const std::string& plateType, const std::string& garnish = "");

    const std::string& getName() const { return _name; }
    const std::string& getPlateType() const { return _plateType; }
    const std::string& getGarnish() const { return _garnish; }

    /// Текст для официанта: стиль, тарелка и украшение (если есть).
    std::string describe() const;
    /// Есть ли украшение.
    bool hasGarnish() const;

private:
    std::string _name;
    std::string _plateType;
    std::string _garnish;
};
