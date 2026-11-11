#pragma once

#include "hardware/AtmDevice.h"

/// @brief Цифровая клавиатура банкомата (PIN-пад). Набранное можно скрывать звёздочками.
class Keypad : public AtmDevice {
public:
    /// Максимальная длина набора по умолчанию.
    static constexpr int DEFAULT_MAX_LENGTH = 12;

    Keypad(const std::string& serialNumber, bool isMasked, int maxLength = DEFAULT_MAX_LENGTH);

    const std::string& getBuffer() const { return _buffer; }
    bool isMasked() const { return _isMasked; }

    std::string deviceName() const override;
    /// Нажать клавишу. Принимаются только цифры, пока не набрана максимальная длина.
    bool pressKey(char key);
    /// Набрать строку целиком. Возвращает, сколько клавиш принято.
    int typeText(const std::string& text);
    /// Стереть последнюю цифру.
    void backspace();
    /// Стереть всё набранное.
    void clear();
    /// Забрать набранное (буфер очищается).
    std::string takeInput();
    /// Что показать на экране: цифры или звёздочки.
    std::string displayText() const;

private:
    std::string _buffer;
    int _maxLength;
    bool _isMasked;
};
