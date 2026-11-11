#pragma once

#include <memory>
#include <string>
#include <vector>

#include "cards/Card.h"

/// @brief Держатель карт — клиент банка: ФИО, паспорт, телефон и его карты.
class CardHolder {
public:
    /// Белорусские номера: «+375» и ещё 9 цифр.
    static constexpr const char* PHONE_PREFIX = "+375";
    static constexpr std::size_t PHONE_LENGTH = 13;

    /// Бросает InvalidCardException, если ФИО или номер паспорта пустые.
    CardHolder(const std::string& fullName, const std::string& passportNumber, const std::string& phone);

    const std::string& getFullName() const { return _fullName; }
    const std::string& getPassportNumber() const { return _passportNumber; }
    const std::string& getPhone() const { return _phone; }
    const std::vector<std::shared_ptr<Card>>& getCards() const { return _cards; }
    bool isVip() const { return _isVip; }

    /// Выдать клиенту карту. Бросает InvalidCardException, если карта пустая или уже есть.
    void addCard(std::shared_ptr<Card> card);
    /// Закрыть карту по номеру. Бросает InvalidCardException, если такой карты нет.
    void removeCard(const std::string& cardNumber);
    /// Найти карту по номеру. Бросает InvalidCardException, если такой карты нет.
    std::shared_ptr<Card> findCard(const std::string& cardNumber) const;
    /// Сколько всего денег доступно по всем незаблокированным картам.
    double totalAvailable() const;
    /// Сколько карт не заблокировано.
    int activeCardsCount() const;
    /// Правильный ли номер телефона (для SMS).
    bool hasValidPhone() const;
    /// Сделать клиента VIP, если у него достаточно денег на картах.
    bool upgradeToVip(double requiredBalance);

private:
    std::string _fullName;
    std::string _passportNumber;
    std::string _phone;
    std::vector<std::shared_ptr<Card>> _cards;
    bool _isVip;
};
