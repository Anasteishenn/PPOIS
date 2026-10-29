/**
 * @file CulinaryException.h
 * @brief Собственные исключения кулинарной системы.
 *
 * Все исключения наследуются от CulinaryException, поэтому их можно поймать одним catch.
 */
#pragma once

#include <exception>
#include <string>

/// @brief Базовое исключение кулинарной системы.
class CulinaryException : public std::exception {
public:
    explicit CulinaryException(const std::string& message);
    const char* what() const noexcept override;

private:
    std::string _message;
};

/// Некорректные данные ингредиента: пустое имя, отрицательная цена или количество.
class InvalidIngredientException : public CulinaryException {
public:
    using CulinaryException::CulinaryException;
};

/// Ингредиент просрочен.
class ExpiredIngredientException : public CulinaryException {
public:
    using CulinaryException::CulinaryException;
};

/// На складе не хватает продукта.
class InsufficientStockException : public CulinaryException {
public:
    using CulinaryException::CulinaryException;
};

/// Недопустимая температура (вне диапазона процесса или прибора).
class InvalidTemperatureException : public CulinaryException {
public:
    using CulinaryException::CulinaryException;
};

/// Ошибка кухонного прибора: выключен, перегружен, открыта крышка, нет насадки.
class ApplianceException : public CulinaryException {
public:
    using CulinaryException::CulinaryException;
};

/// Ошибка рецепта: нет шагов, неверное число порций, неверное время.
class RecipeException : public CulinaryException {
public:
    using CulinaryException::CulinaryException;
};

/// Оценка вне допустимого диапазона.
class InvalidRatingException : public CulinaryException {
public:
    using CulinaryException::CulinaryException;
};

/// Некорректная цена или скидка.
class InvalidPriceException : public CulinaryException {
public:
    using CulinaryException::CulinaryException;
};

/// Блюдо не найдено в меню.
class DishNotFoundException : public CulinaryException {
public:
    using CulinaryException::CulinaryException;
};

/// Блюдо временно недоступно (закончилось).
class DishNotAvailableException : public CulinaryException {
public:
    using CulinaryException::CulinaryException;
};

/// Ошибка заказа: неверный статус, очередь пуста или переполнена, заказ не найден.
class OrderException : public CulinaryException {
public:
    using CulinaryException::CulinaryException;
};

/// Ошибка столика: занят, не найден, не хватает мест.
class TableException : public CulinaryException {
public:
    using CulinaryException::CulinaryException;
};

/// Ошибка оплаты: денег недостаточно, счёт уже оплачен, превышен бюджет.
class PaymentException : public CulinaryException {
public:
    using CulinaryException::CulinaryException;
};

/// Гость слишком молод для алкогольного напитка.
class AgeRestrictionException : public CulinaryException {
public:
    using CulinaryException::CulinaryException;
};

/// Ошибка персонала: неверные часы работы, нет подходящего повара.
class StaffException : public CulinaryException {
public:
    using CulinaryException::CulinaryException;
};
