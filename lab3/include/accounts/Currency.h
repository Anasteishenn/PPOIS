#pragma once

#include <string>

/// @brief Валюта: код (BYN, USD), символ и курс к белорусскому рублю.
class Currency {
public:
    /// Длина кода валюты по стандарту ISO 4217.
    static constexpr std::size_t CODE_LENGTH = 3;

    /// Бросает CurrencyException, если код не из 3 букв или курс <= 0.
    Currency(const std::string& code, const std::string& symbol, double rateToByn);

    const std::string& getCode() const { return _code; }
    const std::string& getSymbol() const { return _symbol; }
    double getRateToByn() const { return _rateToByn; }

    /// Перевести сумму в этой валюте в белорусские рубли.
    double toByn(double amount) const;
    /// Перевести сумму в рублях в эту валюту.
    double fromByn(double amountByn) const;
    /// Сумма с символом валюты, например «12.50 $».
    std::string format(double amount) const;
    /// Новый курс. Бросает CurrencyException, если курс <= 0.
    void updateRate(double newRate);
    /// Та же ли это валюта (сравнение по коду).
    bool isSame(const Currency& other) const;

private:
    std::string _code;
    std::string _symbol;
    double _rateToByn;
};
