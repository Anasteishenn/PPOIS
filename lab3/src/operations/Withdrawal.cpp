#include "operations/Withdrawal.h"

Withdrawal::Withdrawal(int id, const std::string& cardMask, double amount, double fee, bool isForeignCard)
    : Transaction(id, cardMask, amount, fee), _isForeignCard(isForeignCard) {}

std::string Withdrawal::type() const {
    return "Снятие наличных";
}

std::string Withdrawal::describe() const {
    std::string text = Transaction::describe();
    if (!_notes.empty()) {
        text += ", купюры: " + notesDescription();
    }
    return text;
}

void Withdrawal::recordNotes(const std::map<int, int>& notes) {
    _notes = notes;
}

int Withdrawal::notesCount() const {
    int count = 0;
    for (const auto& [denomination, number] : _notes) {
        count += number;
    }
    return count;
}

std::string Withdrawal::notesDescription() const {
    std::string text;
    // Перебираем от крупных купюр к мелким.
    for (auto note = _notes.rbegin(); note != _notes.rend(); ++note) {
        if (!text.empty()) {
            text += ", ";
        }
        text += std::to_string(note->second) + " x " + std::to_string(note->first);
    }
    return text;
}
