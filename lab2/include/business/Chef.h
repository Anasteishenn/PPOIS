#pragma once

#include <memory>
#include <string>
#include <vector>

#include "business/Cook.h"
#include "business/KitchenStaff.h"
#include "dishes/Dish.h"
#include "recipes/Recipe.h"
#include "recipes/RecipeReview.h"

/// @brief Шеф-повар: руководит командой поваров и распределяет между ними блюда.
class Chef : public KitchenStaff {
public:
    Chef(const std::string& name, int experienceYears, double hourlyRate, const std::string& signatureDish,
         int michelinStars);

    const std::vector<std::shared_ptr<Cook>>& getCooks() const { return _cooks; }
    const std::string& getSignatureDish() const { return _signatureDish; }
    int getMichelinStars() const { return _michelinStars; }

    std::string role() const override;
    /// Нанять повара в команду.
    void hireCook(std::shared_ptr<Cook> cook);
    /// Уволить повара по имени. Бросает StaffException, если такого нет.
    void fireCook(const std::string& cookName);
    /// Выбрать для блюда наименее загруженного подходящего повара. Бросает StaffException, если таких нет.
    std::shared_ptr<Cook> chooseCook(const Dish& dish) const;
    /// Оценить рецепт: шеф пишет отзыв от своего имени.
    RecipeReview reviewRecipe(const Recipe& recipe, int rating, const std::string& comment) const;
    /// Размер команды (сколько поваров).
    int teamSize() const;

private:
    std::vector<std::shared_ptr<Cook>> _cooks;
    std::string _signatureDish;
    int _michelinStars;
};
