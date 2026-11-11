#include "hardware/Display.h"

#include "exceptions/AtmException.h"

namespace {
const char* const RUSSIAN = "RU";
const char* const ENGLISH = "EN";
}  // namespace

Display::Display(const std::string& serialNumber, int maxLines)
    : AtmDevice(serialNumber), _language(RUSSIAN), _maxLines(maxLines) {}

std::string Display::deviceName() const {
    return "Экран";
}

void Display::show(const std::string& message) {
    ensureWorking();
    _messages.push_back(message);
    if (static_cast<int>(_messages.size()) > _maxLines) {
        _messages.erase(_messages.begin());
    }
}

void Display::clear() {
    _messages.clear();
}

std::string Display::lastMessage() const {
    return _messages.empty() ? "" : _messages.back();
}

void Display::switchLanguage(const std::string& language) {
    if (language != RUSSIAN && language != ENGLISH) {
        throw DeviceException("Язык не поддерживается: " + language);
    }
    _language = language;
}

std::string Display::welcomeText() const {
    return _language == ENGLISH ? "Welcome! Please insert your card" : "Добро пожаловать! Вставьте карту";
}
