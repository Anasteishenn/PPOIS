/**
 * @file DemoKitchen.h
 * @brief Готовый ресторан с примером меню, склада и персонала для консольной программы.
 */
#pragma once

#include "business/Kitchen.h"

namespace demo {

/// Создаёт ресторан «Бульбаш»: 5 блюд, комбо-набор, склад, приборы, шеф с тремя поварами, 3 столика.
Kitchen createKitchen();

}  // namespace demo
