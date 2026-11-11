#pragma once

#include <cstddef>
#include <string>

/// @brief Купюра: номинал, валюта, серийный номер, повреждена ли.
class Banknote {
public:
    /// Серийный номер: 2 заглавные латинские буквы и 7 цифр, например «AB1234567».
    static constexpr std::size_t SERIAL_LETTERS = 2;
    static constexpr std::size_t SERIAL_LENGTH = 9;

    Banknote(int denomination, const std::string& currencyCode, const std::string& serialNumber,
             bool isDamaged = false);

    int getDenomination() const { return _denomination; }
    const std::string& getCurrencyCode() const { return _currencyCode; }
    const std::string& getSerialNumber() const { return _serialNumber; }
    bool isDamaged() const { return _isDamaged; }

    /// Существует ли такой номинал (5, 10, 20, 50, 100, 200, 500).
    static bool isValidDenomination(int denomination);
    /// Правильный ли серийный номер (защита от подделок).
    bool hasValidSerial() const;
    /// Можно ли принять купюру в банкомат, работающий с заданной валютой.
    bool isAcceptable(const std::string& currencyCode) const;

private:
    int _denomination;
    std::string _currencyCode;
    std::string _serialNumber;
    bool _isDamaged;
};
