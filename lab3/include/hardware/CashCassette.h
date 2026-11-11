#pragma once

#include <string>

/// @brief Кассета с купюрами одного номинала внутри диспенсера.
class CashCassette {
public:
    static constexpr double PERCENT = 100.0;
    /// Кассета считается почти пустой, если заполнена меньше чем на 20%.
    static constexpr double LOW_LEVEL_PERCENT = 20.0;

    /// Бросает InvalidBanknoteException для неизвестного номинала, DeviceException — для неверной ёмкости.
    CashCassette(int denomination, int count, int capacity, const std::string& currencyCode);

    int getDenomination() const { return _denomination; }
    int getCount() const { return _count; }
    int getCapacity() const { return _capacity; }
    const std::string& getCurrencyCode() const { return _currencyCode; }

    /// Достать купюры. Бросает CashShortageException, если купюр не хватает.
    void take(int count);
    /// Загрузить купюры. Бросает DeviceException, если не поместятся.
    void load(int count);
    /// Заполнить кассету доверху. Возвращает, сколько купюр добавлено.
    int fillToCapacity();
    /// Сумма денег в кассете.
    int totalValue() const;
    /// Пуста ли кассета.
    bool isEmpty() const;
    /// Почти пуста ли кассета (пора инкассировать).
    bool isLow() const;
    /// Заполненность в процентах.
    double fillPercent() const;

private:
    int _denomination;
    int _count;
    int _capacity;
    std::string _currencyCode;
};
