#include "cards/CardHolder.h"

#include <algorithm>
#include <cctype>

#include "exceptions/AtmException.h"

CardHolder::CardHolder(const std::string& fullName, const std::string& passportNumber, const std::string& phone)
    : _fullName(fullName), _passportNumber(passportNumber), _phone(phone), _isVip(false) {
    if (fullName.empty() || passportNumber.empty()) {
        throw InvalidCardException("У держателя карты должны быть ФИО и номер паспорта");
    }
}

void CardHolder::addCard(std::shared_ptr<Card> card) {
    if (!card) {
        throw InvalidCardException("Пустая карта");
    }
    for (const std::shared_ptr<Card>& existing : _cards) {
        if (existing->getNumber() == card->getNumber()) {
            throw InvalidCardException("Такая карта у клиента уже есть");
        }
    }
    _cards.push_back(std::move(card));
}

void CardHolder::removeCard(const std::string& cardNumber) {
    std::shared_ptr<Card> card = findCard(cardNumber);
    _cards.erase(std::find(_cards.begin(), _cards.end(), card));
}

std::shared_ptr<Card> CardHolder::findCard(const std::string& cardNumber) const {
    for (const std::shared_ptr<Card>& card : _cards) {
        if (card->getNumber() == cardNumber) {
            return card;
        }
    }
    throw InvalidCardException("У клиента нет карты " + cardNumber);
}

double CardHolder::totalAvailable() const {
    double total = 0.0;
    for (const std::shared_ptr<Card>& card : _cards) {
        if (!card->isBlocked()) {
            total += card->availableFunds();
        }
    }
    return total;
}

int CardHolder::activeCardsCount() const {
    return static_cast<int>(std::count_if(_cards.begin(), _cards.end(),
                                          [](const std::shared_ptr<Card>& card) { return !card->isBlocked(); }));
}

bool CardHolder::hasValidPhone() const {
    if (_phone.size() != PHONE_LENGTH || _phone.rfind(PHONE_PREFIX, 0) != 0) {
        return false;
    }
    std::string digits = _phone.substr(1);
    return std::all_of(digits.begin(), digits.end(),
                       [](char symbol) { return std::isdigit(static_cast<unsigned char>(symbol)) != 0; });
}

bool CardHolder::upgradeToVip(double requiredBalance) {
    if (totalAvailable() >= requiredBalance) {
        _isVip = true;
    }
    return _isVip;
}
