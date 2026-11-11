#include "hardware/CardReader.h"

#include "exceptions/AtmException.h"

CardReader::CardReader(const std::string& serialNumber) : AtmDevice(serialNumber) {}

std::string CardReader::deviceName() const {
    return "Картоприёмник";
}

void CardReader::insertCard(std::shared_ptr<Card> card) {
    ensureWorking();
    if (!card) {
        throw InvalidCardException("Карта не вставлена");
    }
    if (hasCard()) {
        throw DeviceException("В картоприёмнике уже есть карта");
    }
    _insertedCard = std::move(card);
}

std::shared_ptr<Card> CardReader::ejectCard() {
    if (!hasCard()) {
        throw DeviceException("В картоприёмнике нет карты");
    }
    std::shared_ptr<Card> card = _insertedCard;
    _insertedCard.reset();
    return card;
}

void CardReader::captureCard() {
    _capturedCards.push_back(ejectCard());
}

bool CardReader::hasCard() const {
    return _insertedCard != nullptr;
}

int CardReader::capturedCount() const {
    return static_cast<int>(_capturedCards.size());
}

std::vector<std::shared_ptr<Card>> CardReader::releaseCapturedCards() {
    std::vector<std::shared_ptr<Card>> cards = _capturedCards;
    _capturedCards.clear();
    return cards;
}
