#pragma once

#include "recipes/CookingProcess.h"

/// @brief Химическая обработка: маринование, брожение, засолка с помощью вещества-агента.
class ChemicalProcess : public CookingProcess {
public:
    static constexpr double MIN_PH = 0.0;
    static constexpr double MAX_PH = 14.0;
    /// Нейтральная кислотность. Ниже — кислая среда.
    static constexpr double NEUTRAL_PH = 7.0;

    ChemicalProcess(const std::string& name, int durationMinutes, const std::string& agent, double phLevel);

    const std::string& getAgent() const { return _agent; }
    double getPhLevel() const { return _phLevel; }

    /// Например: «Маринование (уксус, pH 3.50), 60 мин».
    std::string execute() const override;
    /// Кислая ли среда (pH < NEUTRAL_PH).
    bool isAcidic() const;

private:
    std::string _agent;
    double _phLevel;
};
