#pragma once

#include <memory>

#include "cards/Card.h"

/// @brief Сеанс работы клиента с банкоматом: от вставки карты до её возврата.
class Session {
public:
    /// Сколько секунд бездействия допускается по умолчанию.
    static constexpr int DEFAULT_TIMEOUT_SECONDS = 60;

    /// Бросает SessionException, если карта пустая или таймаут <= 0.
    explicit Session(std::shared_ptr<Card> card, int timeoutSeconds = DEFAULT_TIMEOUT_SECONDS);

    std::shared_ptr<Card> getCard() const { return _card; }
    bool isAuthenticated() const { return _isAuthenticated; }
    int getSecondsIdle() const { return _secondsIdle; }
    int getOperationsCount() const { return _operationsCount; }
    bool isClosed() const { return _isClosed; }

    /// Ввести PIN. При успехе сеанс становится авторизованным.
    void authenticate(const std::string& pin);
    /// Прошло время бездействия.
    void tick(int seconds);
    /// Истекло ли время сеанса.
    bool isExpired() const;
    /// Можно ли ещё работать (сеанс не закрыт и не истёк).
    bool isActive() const;
    /// Бросает SessionException, если сеанс не активен или PIN не введён.
    void requireAuthenticated() const;
    /// Учесть выполненную операцию (таймер бездействия обнуляется).
    void registerOperation();
    /// Закрыть сеанс.
    void close();

private:
    void requireActive() const;

    std::shared_ptr<Card> _card;
    bool _isAuthenticated;
    int _secondsIdle;
    int _timeoutSeconds;
    int _operationsCount;
    bool _isClosed;
};
