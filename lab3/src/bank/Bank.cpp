#include "bank/Bank.h"

#include <algorithm>

#include "exceptions/AtmException.h"

Bank::Bank(const std::string& name, const std::string& bic, const CurrencyExchange& exchange)
    : _name(name), _bic(bic), _exchange(exchange), _nextAccountNumber(1) {
    if (name.empty() || bic.empty()) {
        throw AccountNotFoundException("У банка должны быть название и БИК");
    }
}

std::shared_ptr<BankAccount> Bank::openAccount(const std::string& ownerName, const std::string& currencyCode,
                                               double initialBalance) {
    const Currency& currency = _exchange.findCurrency(currencyCode);
    std::string number = "BY" + _bic + std::to_string(ACCOUNT_NUMBER_BASE + _nextAccountNumber);
    auto account = std::make_shared<BankAccount>(number, ownerName, currency, initialBalance);
    addAccount(account);
    ++_nextAccountNumber;
    return account;
}

void Bank::addAccount(std::shared_ptr<BankAccount> account) {
    if (!account || _accounts.count(account->getNumber()) > 0) {
        throw AccountNotFoundException("Счёт пустой или с таким номером уже есть");
    }
    _accounts[account->getNumber()] = std::move(account);
}

std::shared_ptr<BankAccount> Bank::findAccount(const std::string& number) const {
    auto found = _accounts.find(number);
    if (found == _accounts.end()) {
        throw AccountNotFoundException("Счёт " + number + " не найден");
    }
    return found->second;
}

void Bank::addClient(std::shared_ptr<CardHolder> holder) {
    if (std::find(_clients.begin(), _clients.end(), holder) == _clients.end()) {
        _clients.push_back(std::move(holder));
    }
}

void Bank::issueCard(const std::shared_ptr<Card>& card, const std::shared_ptr<CardHolder>& holder) {
    if (!card || !holder) {
        throw InvalidCardException("Нужны карта и её владелец");
    }
    findAccount(card->getAccount()->getNumber());
    if (isOwnCard(card->getNumber())) {
        throw InvalidCardException("Карта " + card->maskedNumber() + " уже выпущена");
    }
    holder->addCard(card);
    _cards[card->getNumber()] = card;
    addClient(holder);
}

std::shared_ptr<Card> Bank::findCard(const std::string& number) const {
    auto found = _cards.find(number);
    if (found == _cards.end()) {
        throw InvalidCardException("Карта " + number + " не найдена");
    }
    return found->second;
}

bool Bank::isOwnCard(const std::string& number) const {
    return _cards.count(number) > 0;
}

void Bank::blockCard(const std::string& number) {
    findCard(number)->block();
}

std::shared_ptr<CardHolder> Bank::findHolderOf(const std::string& cardNumber) const {
    for (const std::shared_ptr<CardHolder>& holder : _clients) {
        for (const std::shared_ptr<Card>& card : holder->getCards()) {
            if (card->getNumber() == cardNumber) {
                return holder;
            }
        }
    }
    return nullptr;
}

double Bank::receiveTransfer(const std::string& targetNumber, double amount, const std::string& currencyCode,
                             const std::string& senderName) {
    std::shared_ptr<BankAccount> target = findAccount(targetNumber);
    double credited = _exchange.convert(amount, currencyCode, target->getCurrency().getCode());
    target->deposit(credited, "Перевод от " + senderName);
    return credited;
}

double Bank::transfer(const std::string& fromNumber, const std::string& toNumber, double amount) {
    std::shared_ptr<BankAccount> from = findAccount(fromNumber);
    std::shared_ptr<BankAccount> to = findAccount(toNumber);
    if (from->getCurrency().isSame(to->getCurrency())) {
        from->transferTo(*to, amount);
        return amount;
    }
    if (to->isFrozen()) {
        throw AccountFrozenException("Счёт получателя " + toNumber + " заморожен");
    }
    from->withdraw(amount, "Перевод на счёт " + toNumber);
    return receiveTransfer(toNumber, amount, from->getCurrency().getCode(), from->getOwnerName());
}

double Bank::totalDepositsInByn() const {
    double total = 0.0;
    for (const auto& [number, account] : _accounts) {
        total += account->getCurrency().toByn(account->getBalance());
    }
    return total;
}
