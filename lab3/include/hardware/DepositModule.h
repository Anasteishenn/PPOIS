#pragma once

#include <vector>

#include "hardware/AtmDevice.h"
#include "hardware/Banknote.h"
#include "hardware/BanknoteValidator.h"

/// @brief Купюроприёмник: проверяет внесённые купюры детектором и складывает их в лоток.
class DepositModule : public AtmDevice {
public:
    /// Ёмкость лотка по умолчанию (купюр).
    static constexpr int DEFAULT_CAPACITY = 200;

    DepositModule(const std::string& serialNumber, const BanknoteValidator& validator,
                  int capacity = DEFAULT_CAPACITY);

    const BanknoteValidator& getValidator() const { return _validator; }
    const std::vector<Banknote>& getStoredNotes() const { return _storedNotes; }
    int getCapacity() const { return _capacity; }
    int getLastRejectedCount() const { return _lastRejectedCount; }

    std::string deviceName() const override;
    /// Принять пачку купюр. Плохие купюры возвращаются клиенту. Возвращает принятую сумму.
    int acceptNotes(const std::vector<Banknote>& notes);
    /// Сколько ещё купюр поместится в лоток.
    int freeSpace() const;
    /// Полон ли лоток.
    bool isFull() const;
    /// Сумма денег в лотке.
    int storedValue() const;
    /// Забрать все купюры из лотка (делает инкассатор).
    std::vector<Banknote> collectAll();

private:
    BanknoteValidator _validator;
    std::vector<Banknote> _storedNotes;
    int _capacity;
    int _lastRejectedCount;
};
