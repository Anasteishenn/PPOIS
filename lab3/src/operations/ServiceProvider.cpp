#include "operations/ServiceProvider.h"

#include <algorithm>
#include <cctype>

#include "exceptions/AtmException.h"

ServiceProvider::ServiceProvider(const std::string& name, const std::string& category, int accountLength,
                                 double commissionPercent)
    : _name(name), _category(category), _accountLength(accountLength), _commissionPercent(commissionPercent) {
    if (name.empty() || accountLength <= 0) {
        throw InvalidAmountException("У поставщика должны быть название и длина лицевого счёта");
    }
    if (commissionPercent < 0 || commissionPercent > MAX_COMMISSION_PERCENT) {
        throw InvalidAmountException("Комиссия поставщика должна быть от 0 до 5%");
    }
}

bool ServiceProvider::isValidAccount(const std::string& account) const {
    if (static_cast<int>(account.size()) != _accountLength) {
        return false;
    }
    return std::all_of(account.begin(), account.end(),
                       [](char symbol) { return std::isdigit(static_cast<unsigned char>(symbol)) != 0; });
}

double ServiceProvider::commissionFor(double amount) const {
    return amount * _commissionPercent / PERCENT;
}

std::string ServiceProvider::label() const {
    return _name + " (" + _category + ")";
}
