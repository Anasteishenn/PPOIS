#pragma once

#include <memory>
#include <vector>

#include "cards/Card.h"
#include "hardware/AtmDevice.h"

/// @brief Картоприёмник: принимает карту, возвращает её или изымает (задерживает).
class CardReader : public AtmDevice {
public:
    explicit CardReader(const std::string& serialNumber);

    std::shared_ptr<Card> getInsertedCard() const { return _insertedCard; }
    const std::vector<std::shared_ptr<Card>>& getCapturedCards() const { return _capturedCards; }

    std::string deviceName() const override;
    /// Принять карту. Бросает DeviceException, если карта уже внутри или устройство неисправно.
    void insertCard(std::shared_ptr<Card> card);
    /// Вернуть карту клиенту. Бросает DeviceException, если карты нет.
    std::shared_ptr<Card> ejectCard();
    /// Изъять карту (заблокированную или забытую). Бросает DeviceException, если карты нет.
    void captureCard();
    /// Есть ли карта в картоприёмнике.
    bool hasCard() const;
    /// Сколько карт изъято.
    int capturedCount() const;
    /// Отдать изъятые карты сотруднику банка (список очищается).
    std::vector<std::shared_ptr<Card>> releaseCapturedCards();

private:
    std::shared_ptr<Card> _insertedCard;
    std::vector<std::shared_ptr<Card>> _capturedCards;
};
