#pragma once

#include "operations/ServiceProvider.h"
#include "operations/Transaction.h"

/// @brief Оплата услуги: кому платим и за какой лицевой счёт (номер телефона, договор).
class ServicePayment : public Transaction {
public:
    /// Бросает AccountNotFoundException, если лицевой счёт не подходит поставщику.
    ServicePayment(int id, const std::string& cardMask, double amount, double fee, const ServiceProvider& provider,
                   const std::string& payerAccount);

    const ServiceProvider& getProvider() const { return _provider; }
    const std::string& getPayerAccount() const { return _payerAccount; }

    std::string type() const override;
    /// Описание с поставщиком и лицевым счётом.
    std::string describe() const override;
    /// Относится ли платёж к категории услуг (например, «Мобильная связь»).
    bool isCategory(const std::string& category) const;

private:
    ServiceProvider _provider;
    std::string _payerAccount;
};
