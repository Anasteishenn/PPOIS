#include "hardware/Banknote.h"

#include <algorithm>
#include <array>
#include <cctype>

namespace {
const std::array<int, 7> DENOMINATIONS = {5, 10, 20, 50, 100, 200, 500};
}

Banknote::Banknote(int denomination, const std::string& currencyCode, const std::string& serialNumber,
                   bool isDamaged)
    : _denomination(denomination), _currencyCode(currencyCode), _serialNumber(serialNumber), _isDamaged(isDamaged) {}

bool Banknote::isValidDenomination(int denomination) {
    return std::find(DENOMINATIONS.begin(), DENOMINATIONS.end(), denomination) != DENOMINATIONS.end();
}

bool Banknote::hasValidSerial() const {
    if (_serialNumber.size() != SERIAL_LENGTH) {
        return false;
    }
    for (std::size_t index = 0; index < _serialNumber.size(); ++index) {
        unsigned char symbol = static_cast<unsigned char>(_serialNumber[index]);
        bool isCorrect = index < SERIAL_LETTERS ? std::isupper(symbol) != 0 : std::isdigit(symbol) != 0;
        if (!isCorrect) {
            return false;
        }
    }
    return true;
}

bool Banknote::isAcceptable(const std::string& currencyCode) const {
    return isValidDenomination(_denomination) && !_isDamaged && hasValidSerial() && _currencyCode == currencyCode;
}
