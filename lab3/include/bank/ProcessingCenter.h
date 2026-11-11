#pragma once

#include <memory>

#include "bank/Bank.h"
#include "bank/FraudDetector.h"
#include "cards/Card.h"

/// @brief Процессинговый центр: одобряет или отклоняет операции по картам.
class ProcessingCenter {
public:
    static constexpr double PERCENT = 100.0;

    /// Бросает AccountNotFoundException, если банк не задан.
    ProcessingCenter(std::shared_ptr<Bank> bank, const FraudDetector& fraudDetector);

    const FraudDetector& getFraudDetector() const { return _fraudDetector; }
    int getApprovedCount() const { return _approvedCount; }
    int getDeclinedCount() const { return _declinedCount; }

    /// Одобрить списание: карта действует, операция не подозрительна, денег хватает. Иначе — исключение.
    void authorize(const Card& card, double amount, int month, int year);
    /// Чужая ли карта (выпущена другим банком).
    bool isForeignCard(const Card& card) const;
    /// Процент одобренных операций.
    double approvalRate() const;
    /// Обнулить статистику одобрений.
    void resetStatistics();

private:
    std::shared_ptr<Bank> _bank;
    FraudDetector _fraudDetector;
    int _approvedCount;
    int _declinedCount;
};
