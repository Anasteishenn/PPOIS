#include "operations/Deposit.h"

Deposit::Deposit(int id, const std::string& cardMask, double amount, int acceptedNotes, int rejectedNotes)
    : Transaction(id, cardMask, amount, 0.0), _acceptedNotes(acceptedNotes), _rejectedNotes(rejectedNotes) {}

std::string Deposit::type() const {
    return "Внесение наличных";
}

std::string Deposit::describe() const {
    std::string text = Transaction::describe() + ", принято купюр: " + std::to_string(_acceptedNotes);
    if (hasRejected()) {
        text += ", возвращено: " + std::to_string(_rejectedNotes);
    }
    return text;
}

bool Deposit::hasRejected() const {
    return _rejectedNotes > 0;
}
