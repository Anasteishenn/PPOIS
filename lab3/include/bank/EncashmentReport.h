#pragma once

#include <string>

/// @brief Акт инкассации: сколько денег загружено в банкомат и сколько забрано из купюроприёмника.
class EncashmentReport {
public:
    EncashmentReport(const std::string& atmId, const std::string& collectorName, int day);

    const std::string& getAtmId() const { return _atmId; }
    const std::string& getCollectorName() const { return _collectorName; }
    int getDay() const { return _day; }
    double getLoadedAmount() const { return _loadedAmount; }
    double getCollectedAmount() const { return _collectedAmount; }

    /// Учесть загруженные деньги. Бросает InvalidAmountException, если сумма отрицательная.
    void addLoaded(double amount);
    /// Учесть забранные деньги. Бросает InvalidAmountException, если сумма отрицательная.
    void addCollected(double amount);
    /// Насколько изменилось количество денег у банка в банкомате (загружено минус забрано).
    double netChange() const;
    /// Акт одной строкой.
    std::string summary() const;

private:
    std::string _atmId;
    std::string _collectorName;
    int _day;
    double _loadedAmount;
    double _collectedAmount;
};
