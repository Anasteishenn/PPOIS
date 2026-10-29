#include "business/Chef.h"

#include <utility>

#include "exceptions/CulinaryException.h"

Chef::Chef(const std::string& name, int experienceYears, double hourlyRate, const std::string& signatureDish,
           int michelinStars)
    : KitchenStaff(name, experienceYears, hourlyRate), _signatureDish(signatureDish), _michelinStars(michelinStars) {}

std::string Chef::role() const {
    return "Шеф-повар";
}

void Chef::hireCook(std::shared_ptr<Cook> cook) {
    if (!cook) {
        throw StaffException("Нельзя нанять пустого повара");
    }
    _cooks.push_back(std::move(cook));
}

void Chef::fireCook(const std::string& cookName) {
    for (auto it = _cooks.begin(); it != _cooks.end(); ++it) {
        if ((*it)->getName() == cookName) {
            _cooks.erase(it);
            return;
        }
    }
    throw StaffException("В команде нет повара: " + cookName);
}

std::shared_ptr<Cook> Chef::chooseCook(const Dish& dish) const {
    std::shared_ptr<Cook> best;
    for (const std::shared_ptr<Cook>& cook : _cooks) {
        bool lessBusy = !best || cook->getDishesCooked() < best->getDishesCooked();
        if (cook->canCook(dish) && lessBusy) {
            best = cook;
        }
    }
    if (!best) {
        throw StaffException("Некому приготовить: " + dish.getName());
    }
    return best;
}

RecipeReview Chef::reviewRecipe(const Recipe& recipe, int rating, const std::string& comment) const {
    return RecipeReview(getName(), rating, recipe.getTitle() + ": " + comment);
}

int Chef::teamSize() const {
    return static_cast<int>(_cooks.size());
}
