#include "operations/Session.h"

#include "exceptions/AtmException.h"

Session::Session(std::shared_ptr<Card> card, int timeoutSeconds)
    : _card(std::move(card)),
      _isAuthenticated(false),
      _secondsIdle(0),
      _timeoutSeconds(timeoutSeconds),
      _operationsCount(0),
      _isClosed(false) {
    if (!_card) {
        throw SessionException("Сеанс нельзя начать без карты");
    }
    if (timeoutSeconds <= 0) {
        throw SessionException("Время сеанса должно быть больше нуля");
    }
}

void Session::authenticate(const std::string& pin) {
    requireActive();
    _card->authenticate(pin);
    _isAuthenticated = true;
    _secondsIdle = 0;
}

void Session::tick(int seconds) {
    if (seconds > 0) {
        _secondsIdle += seconds;
    }
}

bool Session::isExpired() const {
    return _secondsIdle >= _timeoutSeconds;
}

bool Session::isActive() const {
    return !_isClosed && !isExpired();
}

void Session::requireAuthenticated() const {
    requireActive();
    if (!_isAuthenticated) {
        throw SessionException("Сначала введите PIN-код");
    }
}

void Session::registerOperation() {
    requireAuthenticated();
    ++_operationsCount;
    _secondsIdle = 0;
}

void Session::close() {
    _isClosed = true;
}

void Session::requireActive() const {
    if (_isClosed) {
        throw SessionException("Сеанс завершён");
    }
    if (isExpired()) {
        throw SessionException("Время сеанса истекло");
    }
}
