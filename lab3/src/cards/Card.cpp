#include "cards/Card.h"

#include <cctype>

#include "exceptions/AtmException.h"

namespace {
const int LUHN_BASE = 10;
const int LUHN_MAX_DIGIT = 9;
const int LUHN_MULTIPLIER = 2;
}  // namespace

Card::Card(const std::string& number, const std::string& holderName, int expiryMonth, int expiryYear,
           const std::string& pin, std::shared_ptr<BankAccount> account)
    : _number(number),
      _holderName(holderName),
      _expiryMonth(expiryMonth),
      _expiryYear(expiryYear),
      _isBlocked(false),
      _pin(pin),
      _limit(DEFAULT_DAILY_LIMIT),
      _account(std::move(account)) {
    if (!isValidNumber(number)) {
        throw InvalidCardException("Неверный номер карты: " + number);
    }
    if (holderName.empty() || expiryMonth < 1 || expiryMonth > MONTHS_IN_YEAR) {
        throw InvalidCardException("Неверные данные владельца или срока действия карты");
    }
    if (!_account) {
        throw InvalidCardException("Карта должна быть привязана к счёту");
    }
}

double Card::availableFunds() const {
    return _account->availableFunds();
}

void Card::charge(double amount, const std::string& description) {
    _account->withdraw(amount, description);
}

bool Card::isValidNumber(const std::string& number) {
    if (number.size() != NUMBER_LENGTH) {
        return false;
    }
    int sum = 0;
    for (std::size_t index = 0; index < number.size(); ++index) {
        char symbol = number[number.size() - 1 - index];
        if (!std::isdigit(static_cast<unsigned char>(symbol))) {
            return false;
        }
        int digit = symbol - '0';
        // Каждую вторую цифру справа удваиваем; если вышло больше 9 — вычитаем 9.
        if (index % LUHN_MULTIPLIER == 1) {
            digit *= LUHN_MULTIPLIER;
            digit = digit > LUHN_MAX_DIGIT ? digit - LUHN_MAX_DIGIT : digit;
        }
        sum += digit;
    }
    return sum % LUHN_BASE == 0;
}

bool Card::isExpired(int month, int year) const {
    return year > _expiryYear || (year == _expiryYear && month > _expiryMonth);
}

void Card::validate(int month, int year) const {
    if (_isBlocked) {
        throw CardBlockedException("Карта " + maskedNumber() + " заблокирована");
    }
    if (isExpired(month, year)) {
        throw CardExpiredException("Срок действия карты " + maskedNumber() + " истёк");
    }
}

void Card::authenticate(const std::string& pin) {
    if (_isBlocked) {
        throw CardBlockedException("Карта " + maskedNumber() + " заблокирована");
    }
    try {
        _pin.verify(pin);
    } catch (const PinLockedException&) {
        _isBlocked = true;
        throw;
    }
}

void Card::changePin(const std::string& oldPin, const std::string& newPin) {
    try {
        _pin.change(oldPin, newPin);
    } catch (const PinLockedException&) {
        _isBlocked = true;
        throw;
    }
}

void Card::registerCashWithdrawal(double amount) {
    _limit.registerWithdrawal(amount);
}

void Card::topUp(double amount, const std::string& description) {
    _account->deposit(amount, description);
}

std::string Card::maskedNumber() const {
    return "**** **** **** " + _number.substr(NUMBER_LENGTH - VISIBLE_DIGITS);
}

void Card::block() {
    _isBlocked = true;
}

void Card::unblock() {
    _isBlocked = false;
    _pin.reset();
}

void Card::startNewDay() {
    _limit.resetDay();
}
