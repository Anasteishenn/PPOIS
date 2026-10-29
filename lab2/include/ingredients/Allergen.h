#pragma once

#include <string>

/// @brief Аллерген (глютен, лактоза, орехи...) с уровнем опасности от 1 до 5.
class Allergen {
public:
    static constexpr int MIN_SEVERITY = 1;
    static constexpr int MAX_SEVERITY = 5;
    /// Начиная с этого уровня аллерген считается опасным.
    static constexpr int SEVERE_LEVEL = 4;

    Allergen(const std::string& name, int severity, const std::string& description = "");

    const std::string& getName() const { return _name; }
    int getSeverity() const { return _severity; }
    const std::string& getDescription() const { return _description; }

    /// Опасный ли аллерген (уровень >= SEVERE_LEVEL).
    bool isSevere() const;
    /// Совпадает ли название аллергена с заданным.
    bool matches(const std::string& name) const;
    /// Текст предупреждения для меню.
    std::string warning() const;

private:
    std::string _name;
    int _severity;
    std::string _description;
};
