#pragma once

#include <cstddef>
#include <string>

/// @brief PIN-код карты. Хранится не сам PIN, а его хеш. Считает неудачные попытки ввода.
class PinCode {
public:
    /// Длина PIN-кода.
    static constexpr std::size_t PIN_LENGTH = 4;
    /// Сколько раз можно ошибиться, прежде чем PIN заблокируется.
    static constexpr int DEFAULT_MAX_ATTEMPTS = 3;

    /// Бросает InvalidPinException, если PIN не из 4 цифр.
    explicit PinCode(const std::string& pin, int maxAttempts = DEFAULT_MAX_ATTEMPTS);

    int getFailedAttempts() const { return _failedAttempts; }
    int getMaxAttempts() const { return _maxAttempts; }

    /// Правильный ли формат PIN: ровно 4 цифры.
    static bool isValidFormat(const std::string& pin);
    /// Совпадает ли PIN (без подсчёта попыток).
    bool matches(const std::string& pin) const;
    /// Проверить PIN. При ошибке бросает InvalidPinException, после последней попытки — PinLockedException.
    void verify(const std::string& pin);
    /// Заблокирован ли PIN (попытки закончились).
    bool isLocked() const;
    /// Сколько попыток осталось.
    int attemptsLeft() const;
    /// Сменить PIN. Старый PIN должен быть верным, новый — правильного формата и отличаться от старого.
    void change(const std::string& oldPin, const std::string& newPin);
    /// Сбросить счётчик ошибок (делает банк после проверки личности клиента).
    void reset();

private:
    static std::size_t hashOf(const std::string& pin);

    std::size_t _hash;
    int _failedAttempts;
    int _maxAttempts;
};
