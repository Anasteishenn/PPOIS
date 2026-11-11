#pragma once

#include <string>
#include <vector>

#include "hardware/AtmDevice.h"

/// @brief Экран банкомата: показывает последние сообщения на выбранном языке.
class Display : public AtmDevice {
public:
    /// Сколько строк помещается на экране по умолчанию.
    static constexpr int DEFAULT_MAX_LINES = 5;

    explicit Display(const std::string& serialNumber, int maxLines = DEFAULT_MAX_LINES);

    const std::vector<std::string>& getMessages() const { return _messages; }
    const std::string& getLanguage() const { return _language; }

    std::string deviceName() const override;
    /// Показать сообщение. Старые строки уходят за край экрана.
    void show(const std::string& message);
    /// Очистить экран.
    void clear();
    /// Последнее сообщение (пустая строка, если экран чист).
    std::string lastMessage() const;
    /// Сменить язык: «RU» или «EN». Бросает DeviceException для других языков.
    void switchLanguage(const std::string& language);
    /// Приветствие на текущем языке.
    std::string welcomeText() const;

private:
    std::vector<std::string> _messages;
    std::string _language;
    int _maxLines;
};
