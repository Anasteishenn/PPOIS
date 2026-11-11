#include "hardware/Keypad.h"

#include <cctype>

Keypad::Keypad(const std::string& serialNumber, bool isMasked, int maxLength)
    : AtmDevice(serialNumber), _maxLength(maxLength), _isMasked(isMasked) {}

std::string Keypad::deviceName() const {
    return "Клавиатура";
}

bool Keypad::pressKey(char key) {
    ensureWorking();
    bool isDigit = std::isdigit(static_cast<unsigned char>(key)) != 0;
    if (!isDigit || static_cast<int>(_buffer.size()) >= _maxLength) {
        return false;
    }
    _buffer += key;
    return true;
}

int Keypad::typeText(const std::string& text) {
    int accepted = 0;
    for (char key : text) {
        if (pressKey(key)) {
            ++accepted;
        }
    }
    return accepted;
}

void Keypad::backspace() {
    if (!_buffer.empty()) {
        _buffer.pop_back();
    }
}

void Keypad::clear() {
    _buffer.clear();
}

std::string Keypad::takeInput() {
    std::string input = _buffer;
    _buffer.clear();
    return input;
}

std::string Keypad::displayText() const {
    return _isMasked ? std::string(_buffer.size(), '*') : _buffer;
}
