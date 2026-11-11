/**
 * @file AtmException.h
 * @brief Собственные исключения системы «Банкомат».
 *
 * Все исключения наследуются от AtmException, поэтому их можно поймать одним catch.
 */
#pragma once

#include <exception>
#include <string>

/// @brief Базовое исключение системы «Банкомат».
class AtmException : public std::exception {
public:
    explicit AtmException(const std::string& message);
    const char* what() const noexcept override;

private:
    std::string _message;
};

/// Неверная карта: плохой номер, нет счёта, карта не найдена, карта не того типа.
class InvalidCardException : public AtmException {
public:
    using AtmException::AtmException;
};

/// Карта заблокирована.
class CardBlockedException : public AtmException {
public:
    using AtmException::AtmException;
};

/// Срок действия карты истёк.
class CardExpiredException : public AtmException {
public:
    using AtmException::AtmException;
};

/// Введён неверный PIN-код или PIN неверного формата.
class InvalidPinException : public AtmException {
public:
    using AtmException::AtmException;
};

/// Попытки ввода PIN закончились — PIN заблокирован.
class PinLockedException : public AtmException {
public:
    using AtmException::AtmException;
};

/// На счёте (или на карте) недостаточно денег.
class InsufficientFundsException : public AtmException {
public:
    using AtmException::AtmException;
};

/// Превышен лимит: дневной лимит снятия, число операций, число купюр.
class LimitExceededException : public AtmException {
public:
    using AtmException::AtmException;
};

/// Неверная сумма: ноль, отрицательная, не кратна номиналу купюр.
class InvalidAmountException : public AtmException {
public:
    using AtmException::AtmException;
};

/// В банкомате не хватает купюр, чтобы выдать сумму.
class CashShortageException : public AtmException {
public:
    using AtmException::AtmException;
};

/// Счёт не найден в банке.
class AccountNotFoundException : public AtmException {
public:
    using AtmException::AtmException;
};

/// Счёт заморожен (арестован) — операции по нему запрещены.
class AccountFrozenException : public AtmException {
public:
    using AtmException::AtmException;
};

/// Ошибка сеанса: карта не вставлена, PIN не введён, время сеанса вышло.
class SessionException : public AtmException {
public:
    using AtmException::AtmException;
};

/// Неисправность устройства банкомата: нет бумаги, переполнен лоток, устройство сломано.
class DeviceException : public AtmException {
public:
    using AtmException::AtmException;
};

/// Купюра не принята: фальшивая, повреждённая, неизвестный номинал или валюта.
class InvalidBanknoteException : public AtmException {
public:
    using AtmException::AtmException;
};

/// Ошибка валюты: неизвестный код, неверный курс, разные валюты счетов.
class CurrencyException : public AtmException {
public:
    using AtmException::AtmException;
};

/// Операция похожа на мошенничество.
class FraudDetectedException : public AtmException {
public:
    using AtmException::AtmException;
};

/// Нет доступа: неверный служебный код сотрудника.
class AccessDeniedException : public AtmException {
public:
    using AtmException::AtmException;
};

/// Ошибка операции: неверный переход статуса, операция не найдена.
class TransactionException : public AtmException {
public:
    using AtmException::AtmException;
};
