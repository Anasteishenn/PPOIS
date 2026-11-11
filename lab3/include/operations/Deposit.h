#pragma once

#include "operations/Transaction.h"

/// @brief Внесение наличных: сколько купюр принято и сколько возвращено.
class Deposit : public Transaction {
public:
    Deposit(int id, const std::string& cardMask, double amount, int acceptedNotes, int rejectedNotes);

    int getAcceptedNotes() const { return _acceptedNotes; }
    int getRejectedNotes() const { return _rejectedNotes; }

    std::string type() const override;
    /// Описание с числом принятых и возвращённых купюр.
    std::string describe() const override;
    /// Были ли возвращённые (непринятые) купюры.
    bool hasRejected() const;

private:
    int _acceptedNotes;
    int _rejectedNotes;
};
