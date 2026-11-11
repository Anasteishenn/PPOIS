#pragma once

#include <string>

/// @brief Поставщик услуг, которые можно оплатить в банкомате (мобильная связь, ЖКХ, интернет).
class ServiceProvider {
public:
    static constexpr double PERCENT = 100.0;
    /// Максимальная комиссия поставщика, %.
    static constexpr double MAX_COMMISSION_PERCENT = 5.0;

    /// Бросает InvalidAmountException: пустое название, длина лицевого счёта <= 0, комиссия вне 0..5%.
    ServiceProvider(const std::string& name, const std::string& category, int accountLength,
                    double commissionPercent);

    const std::string& getName() const { return _name; }
    const std::string& getCategory() const { return _category; }
    int getAccountLength() const { return _accountLength; }
    double getCommissionPercent() const { return _commissionPercent; }

    /// Правильный ли лицевой счёт (телефон): только цифры нужной длины.
    bool isValidAccount(const std::string& account) const;
    /// Комиссия за платёж.
    double commissionFor(double amount) const;
    /// Подпись: «МТС (Мобильная связь)».
    std::string label() const;

private:
    std::string _name;
    std::string _category;
    int _accountLength;
    double _commissionPercent;
};
