#pragma once

#include <map>

#include "operations/Transaction.h"

/// @brief Снятие наличных: какие купюры выданы, чужая ли карта.
class Withdrawal : public Transaction {
public:
    Withdrawal(int id, const std::string& cardMask, double amount, double fee, bool isForeignCard);

    const std::map<int, int>& getNotes() const { return _notes; }
    bool isForeignCard() const { return _isForeignCard; }

    std::string type() const override;
    /// Описание с перечнем купюр.
    std::string describe() const override;
    /// Записать, какие купюры выданы (номинал -> количество).
    void recordNotes(const std::map<int, int>& notes);
    /// Сколько всего купюр выдано.
    int notesCount() const;
    /// Купюры словами: «2 x 100, 1 x 50».
    std::string notesDescription() const;

private:
    std::map<int, int> _notes;
    bool _isForeignCard;
};
