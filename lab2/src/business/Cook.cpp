#include "business/Cook.h"

#include "exceptions/CulinaryException.h"

Cook::Cook(const std::string& name, int experienceYears, double hourlyRate, const std::string& station,
           const std::string& specialty)
    : KitchenStaff(name, experienceYears, hourlyRate), _station(station), _specialty(specialty), _dishesCooked(0) {}

std::string Cook::role() const {
    return "Повар";
}

bool Cook::canCook(const Dish& dish) const {
    return _specialty.empty() || _specialty == dish.course();
}

std::vector<std::string> Cook::cook(const Dish& dish) {
    if (!canCook(dish)) {
        throw StaffException(getName() + " не готовит раздел: " + dish.course());
    }
    std::vector<std::string> log = {getName() + " готовит " + dish.getName()};
    for (const std::string& line : dish.prepare()) {
        log.push_back("  " + line);
    }
    ++_dishesCooked;
    return log;
}

std::string Cook::useAppliance(KitchenAppliance& appliance, int minutes) const {
    appliance.turnOn();
    appliance.use(minutes);
    return getName() + " использует " + appliance.type() + " " + std::to_string(minutes) + " мин";
}
