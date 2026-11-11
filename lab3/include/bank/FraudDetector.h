#pragma once

#include <deque>
#include <string>
#include <vector>

/// @brief Антифрод: ловит слишком крупные суммы и серии операций подряд на большую сумму.
class FraudDetector {
public:
    /// Серия операций подозрительна, если в сумме больше порога, умноженного на этот коэффициент.
    static constexpr double SERIES_MULTIPLIER = 2.0;

    /// Бросает InvalidAmountException, если порог или размер окна <= 0.
    FraudDetector(double largeAmountThreshold, int windowSize);

    double getLargeAmountThreshold() const { return _largeAmountThreshold; }
    const std::vector<std::string>& getAlerts() const { return _alerts; }

    /// Подозрительна ли сумма (сама по себе крупная или вместе с последними операциями).
    bool isSuspicious(double amount) const;
    /// Проверить операцию. Бросает FraudDetectedException и записывает тревогу, если она подозрительна.
    void check(const std::string& cardMask, double amount);
    /// Запомнить сумму операции (хранятся только последние windowSize сумм).
    void record(double amount);
    /// Сумма последних операций.
    double recentTotal() const;
    /// Забыть последние операции (например, после проверки клиента службой безопасности).
    void reset();

private:
    double _largeAmountThreshold;
    int _windowSize;
    std::deque<double> _recentAmounts;
    std::vector<std::string> _alerts;
};
