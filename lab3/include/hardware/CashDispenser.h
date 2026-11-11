#pragma once

#include <map>
#include <vector>

#include "hardware/AtmDevice.h"
#include "hardware/CashCassette.h"

/// Какие купюры выдать: номинал -> количество.
using NotesPlan = std::map<int, int>;

/// @brief Диспенсер: набор кассет и выдача наличных крупными купюрами.
class CashDispenser : public AtmDevice {
public:
    /// Больше стольких купюр за раз банкомат не выдаёт.
    static constexpr int DEFAULT_MAX_NOTES = 40;

    explicit CashDispenser(const std::string& serialNumber, int maxNotesPerWithdrawal = DEFAULT_MAX_NOTES);

    const std::vector<CashCassette>& getCassettes() const { return _cassettes; }
    int getDispensedTotal() const { return _dispensedTotal; }

    std::string deviceName() const override;
    /// Установить кассету. Бросает DeviceException, если кассета такого номинала уже стоит.
    void addCassette(const CashCassette& cassette);
    /// Сколько всего денег в банкомате.
    int totalCash() const;
    /// Разложить сумму на купюры: сначала крупные. Бросает CashShortageException или LimitExceededException.
    NotesPlan planNotes(int amount) const;
    /// Можно ли выдать сумму имеющимися купюрами.
    bool canDispense(int amount) const;
    /// Выдать сумму: купюры забираются из кассет. Возвращает, какие купюры выданы.
    NotesPlan dispense(int amount);
    /// Загрузить купюры в кассету нужного номинала. Бросает DeviceException, если такой кассеты нет.
    void loadNotes(int denomination, int count);
    /// Заполнить все кассеты доверху. Возвращает загруженную сумму.
    int refillAll();
    /// Сколько кассет почти пусты.
    int lowCassettesCount() const;
    /// Самый мелкий номинал, который сейчас есть (0, если денег нет).
    int minDenomination() const;

private:
    std::vector<CashCassette> _cassettes;
    int _maxNotesPerWithdrawal;
    int _dispensedTotal;
};
