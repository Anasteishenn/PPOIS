#include "cards/CashbackProgram.h"

#include <algorithm>

#include "exceptions/AtmException.h"

CashbackProgram::CashbackProgram(double basePercent, double bonusPercent, double monthlyCap)
    : _basePercent(basePercent), _bonusPercent(bonusPercent), _monthlyCap(monthlyCap) {
    if (basePercent < 0 || bonusPercent < 0 || monthlyCap < 0) {
        throw InvalidAmountException("Параметры кэшбэка не могут быть отрицательными");
    }
}

void CashbackProgram::addBonusCategory(const std::string& category) {
    _bonusCategories.insert(category);
}

bool CashbackProgram::isBonusCategory(const std::string& category) const {
    return _bonusCategories.count(category) > 0;
}

double CashbackProgram::percentFor(const std::string& category) const {
    return isBonusCategory(category) ? _bonusPercent : _basePercent;
}

double CashbackProgram::calculate(double amount, const std::string& category, double earnedThisMonth) const {
    if (amount <= 0) {
        return 0.0;
    }
    double cashback = amount * percentFor(category) / PERCENT;
    double leftUnderCap = std::max(0.0, _monthlyCap - earnedThisMonth);
    return std::min(cashback, leftUnderCap);
}
