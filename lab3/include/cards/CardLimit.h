#pragma once

/// @brief Дневные лимиты карты: сколько денег можно снять и сколько операций сделать за день.
class CardLimit {
public:
    /// Число операций снятия в день по умолчанию.
    static constexpr int DEFAULT_MAX_OPERATIONS = 10;

    /// Бросает InvalidAmountException, если лимит или число операций <= 0.
    explicit CardLimit(double dailyLimit, int maxOperationsPerDay = DEFAULT_MAX_OPERATIONS);

    double getDailyLimit() const { return _dailyLimit; }
    double getWithdrawnToday() const { return _withdrawnToday; }
    int getMaxOperationsPerDay() const { return _maxOperationsPerDay; }
    int getOperationsToday() const { return _operationsToday; }

    /// Сколько ещё можно снять сегодня.
    double remaining() const;
    /// Можно ли снять сумму, не нарушив лимиты.
    bool canWithdraw(double amount) const;
    /// Учесть снятие. Бросает LimitExceededException, если лимит превышен.
    void registerWithdrawal(double amount);
    /// Новый день — счётчики обнуляются.
    void resetDay();
    /// Изменить дневной лимит. Бросает InvalidAmountException, если лимит <= 0.
    void changeLimit(double newLimit);

private:
    double _dailyLimit;
    double _withdrawnToday;
    int _maxOperationsPerDay;
    int _operationsToday;
};
