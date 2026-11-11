#include "operations/ServicePayment.h"

#include "exceptions/AtmException.h"

ServicePayment::ServicePayment(int id, const std::string& cardMask, double amount, double fee,
                               const ServiceProvider& provider, const std::string& payerAccount)
    : Transaction(id, cardMask, amount, fee), _provider(provider), _payerAccount(payerAccount) {
    if (!provider.isValidAccount(payerAccount)) {
        throw AccountNotFoundException("Неверный лицевой счёт для «" + provider.getName() + "»: " + payerAccount);
    }
}

std::string ServicePayment::type() const {
    return "Оплата услуг";
}

std::string ServicePayment::describe() const {
    return Transaction::describe() + ", " + _provider.label() + ", лицевой счёт " + _payerAccount;
}

bool ServicePayment::isCategory(const std::string& category) const {
    return _provider.getCategory() == category;
}
