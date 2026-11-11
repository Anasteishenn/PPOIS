/**
 * @file DemoAtm.h
 * @brief Готовые данные для консольной программы: банк, клиенты, карты, банкомат, сотрудники.
 */
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "bank/Atm.h"
#include "bank/Bank.h"
#include "bank/CashCollector.h"
#include "bank/MaintenanceLog.h"
#include "bank/Technician.h"
#include "operations/ServiceProvider.h"
#include "operations/UtilityBill.h"

namespace demo {

/// Служебный код банкомата (для инкассатора и техника).
const std::string SERVICE_CODE = "7777";
/// Код доступа сотрудников для начала смены.
const std::string EMPLOYEE_CODE = "2468";

/// Карта из демо-набора: подпись для меню и PIN-код (показываем, чтобы можно было попробовать).
struct DemoCard {
    std::string label;
    std::string pin;
    std::shared_ptr<Card> card;
};

/// Всё, с чем работает консольная программа.
struct World {
    std::shared_ptr<Bank> bank;
    std::shared_ptr<Atm> atm;
    std::vector<DemoCard> cards;
    std::vector<ServiceProvider> providers;
    std::vector<UtilityBill> bills;
    CashCollector collector;
    Technician technician;
    MaintenanceLog maintenance;
    int day;
};

/// Создаёт банк «АТМ-Банк», двух клиентов, пять карт (одна — чужого банка) и банкомат в Минске с наличными.
World createWorld();

}  // namespace demo
