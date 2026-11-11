#pragma once

#include <string>

/// Состояние операции.
enum class TransactionStatus { Pending, Completed, Failed };

/// @brief Абстрактная операция в банкомате: номер, карта, сумма, комиссия, статус.
class Transaction {
public:
    /// Бросает InvalidAmountException, если сумма или комиссия отрицательные.
    Transaction(int id, const std::string& cardMask, double amount, double fee);
    virtual ~Transaction() = default;

    int getId() const { return _id; }
    const std::string& getCardMask() const { return _cardMask; }
    double getAmount() const { return _amount; }
    double getFee() const { return _fee; }
    TransactionStatus getStatus() const { return _status; }
    const std::string& getFailReason() const { return _failReason; }

    /// Вид операции словами («Снятие наличных», «Перевод»...).
    virtual std::string type() const = 0;
    /// Описание для экрана и чека.
    virtual std::string describe() const;
    /// Сумма вместе с комиссией.
    double total() const;
    /// Операция прошла успешно. Бросает TransactionException, если она уже завершена.
    void complete();
    /// Операция не удалась. Бросает TransactionException, если она уже завершена.
    void fail(const std::string& reason);
    /// Успешна ли операция.
    bool isSuccessful() const;
    /// Статус словами: «В обработке», «Успешно», «Отклонено».
    std::string statusName() const;

private:
    void ensurePending() const;

    int _id;
    std::string _cardMask;
    double _amount;
    double _fee;
    TransactionStatus _status;
    std::string _failReason;
};
