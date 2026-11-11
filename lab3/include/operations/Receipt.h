#pragma once

#include <string>
#include <vector>

#include "operations/Transaction.h"

/// @brief Чек банкомата: шапка (банкомат, номер операции, карта) и строки с подробностями.
class Receipt {
public:
    Receipt(const std::string& atmId, int transactionId, const std::string& cardMask);

    const std::string& getAtmId() const { return _atmId; }
    int getTransactionId() const { return _transactionId; }
    const std::string& getCardMask() const { return _cardMask; }
    const std::vector<std::string>& getLines() const { return _lines; }

    /// Добавить строку в чек.
    void addLine(const std::string& line);
    /// Добавить в чек описание операции и её статус.
    void addTransaction(const Transaction& transaction);
    /// Сколько строк в теле чека.
    int lineCount() const;
    /// Готовый текст чека для печати.
    std::string text() const;

private:
    std::string _atmId;
    int _transactionId;
    std::string _cardMask;
    std::vector<std::string> _lines;
};
