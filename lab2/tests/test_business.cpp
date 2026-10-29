#include <memory>

#include "TestData.h"
#include "appliances/SmartScale.h"
#include "business/Bill.h"
#include "business/Kitchen.h"
#include "business/OrderQueue.h"
#include "business/ShoppingList.h"
#include "business/WasteLog.h"
#include "doctest.h"
#include "exceptions/CulinaryException.h"
#include "ingredients/PerishableIngredient.h"

using doctest::Approx;

namespace {

KitchenOrder makeReadyOrder() {
    KitchenOrder order(1, RestaurantTable(1, 4));
    order.addDish(testdata::makeAppetizer());
    order.addDish(testdata::makeBeverage());
    order.startCooking("Иван");
    order.markReady();
    return order;
}

Kitchen makeKitchen(double potatoGrams = 1000) {
    Menu menu("Основное", "Осень");
    menu.addDish(testdata::makeAppetizer());
    menu.addDish(testdata::makeMainCourse());
    Kitchen kitchen("Тест", menu, testdata::makeInventory(potatoGrams), testdata::makeChef());
    kitchen.addTable(RestaurantTable(1, 2));
    kitchen.addTable(RestaurantTable(2, 6));
    return kitchen;
}

}  // namespace

TEST_CASE("KitchenStaff: часы, зарплата, стаж") {
    CHECK_THROWS_AS(Cook("", 1, 10, "цех", ""), StaffException);
    CHECK_THROWS_AS(Cook("Иван", -1, 10, "цех", ""), StaffException);
    Cook cook("Иван", 6, 20, "горячий цех", "");
    CHECK_THROWS_AS(cook.logHours(0), StaffException);
    CHECK_THROWS_AS(cook.logHours(13), StaffException);
    cook.logHours(8);
    cook.logHours(4);
    CHECK(cook.getHoursWorked() == Approx(12));
    CHECK(cook.salary() == Approx(240));
    CHECK(cook.isSenior());
    CHECK_FALSE(Cook("Олег", 1, 10, "бар", "").isSenior());
    CHECK(cook.badge() == "Повар Иван, стаж 6 г.");
    CHECK(cook.getExperienceYears() == 6);
}

TEST_CASE("Cook: специализация, готовка, приборы") {
    Cook coldCook("Мария", 3, 15, "холодный цех", "Закуска");
    CHECK(coldCook.role() == "Повар");
    CHECK(coldCook.canCook(*testdata::makeAppetizer()));
    CHECK_FALSE(coldCook.canCook(*testdata::makeMainCourse()));
    CHECK_THROWS_AS(coldCook.cook(*testdata::makeMainCourse()), StaffException);
    std::vector<std::string> log = coldCook.cook(*testdata::makeAppetizer());
    CHECK(log.front() == "Мария готовит Драники");
    CHECK(coldCook.getDishesCooked() == 1);
    SmartScale scale("Xiaomi", 5, 5000);
    CHECK(coldCook.useAppliance(scale, 2) == "Мария использует Весы 2 мин");
    CHECK(scale.isOn());
    CHECK(coldCook.getStation() == "холодный цех");
    CHECK(coldCook.getSpecialty() == "Закуска");
}

TEST_CASE("Chef: команда, выбор повара, отзыв") {
    Chef chef("Анна", 12, 30, "Бифштекс", 1);
    CHECK(chef.role() == "Шеф-повар");
    CHECK_THROWS_AS(chef.hireCook(nullptr), StaffException);
    CHECK_THROWS_AS(chef.chooseCook(*testdata::makeAppetizer()), StaffException);
    auto ivan = std::make_shared<Cook>("Иван", 6, 20, "цех", "");
    auto maria = std::make_shared<Cook>("Мария", 3, 15, "цех", "");
    chef.hireCook(ivan);
    chef.hireCook(maria);
    CHECK(chef.teamSize() == 2);
    ivan->cook(*testdata::makeAppetizer());
    CHECK(chef.chooseCook(*testdata::makeAppetizer())->getName() == "Мария");
    chef.fireCook("Мария");
    CHECK(chef.teamSize() == 1);
    CHECK_THROWS_AS(chef.fireCook("Мария"), StaffException);
    RecipeReview review = chef.reviewRecipe(testdata::makeRecipe(), 5, "идеально");
    CHECK(review.getAuthor() == "Анна");
    CHECK(review.getComment() == "Пюре: идеально");
    CHECK(chef.getSignatureDish() == "Бифштекс");
    CHECK(chef.getMichelinStars() == 1);
    CHECK(chef.getCooks().size() == 1);
}

TEST_CASE("RestaurantTable: посадка и освобождение") {
    CHECK_THROWS_AS(RestaurantTable(0, 2), TableException);
    RestaurantTable table(1, 4);
    CHECK(table.canSeat(4));
    CHECK_FALSE(table.canSeat(5));
    CHECK_FALSE(table.canSeat(0));
    CHECK_THROWS_AS(table.seat(5), TableException);
    table.seat(3);
    CHECK(table.isOccupied());
    CHECK(table.getGuests() == 3);
    CHECK_FALSE(table.canSeat(1));
    CHECK_THROWS_AS(table.seat(1), TableException);
    table.release();
    CHECK_FALSE(table.isOccupied());
    CHECK(table.getSeats() == 4);
}

TEST_CASE("KitchenOrder: статусы и сумма") {
    KitchenOrder order(7, RestaurantTable(3, 4), true);
    CHECK(order.statusName() == "Новый");
    CHECK_THROWS_AS(order.startCooking("Иван"), OrderException);
    CHECK_THROWS_AS(order.addDish(nullptr), DishNotFoundException);
    order.addDish(testdata::makeAppetizer());
    order.addDish(testdata::makeBeverage());
    CHECK(order.total() == Approx(15));
    CHECK_THROWS_AS(order.markReady(), OrderException);
    order.startCooking("Иван");
    CHECK(order.statusName() == "Готовится");
    CHECK(order.getAssignedCook() == "Иван");
    CHECK_THROWS_AS(order.addDish(testdata::makeAppetizer()), OrderException);
    order.markReady();
    CHECK(order.statusName() == "Готов");
    CHECK_THROWS_AS(order.cancel(), OrderException);
    KitchenOrder other(8, RestaurantTable(1, 2));
    other.cancel();
    CHECK(other.statusName() == "Отменён");
    CHECK(order.getId() == 7);
    CHECK(order.getTableNumber() == 3);
    CHECK(order.isUrgent());
}

TEST_CASE("OrderQueue: очередь со срочными заказами") {
    CHECK_THROWS_AS(OrderQueue(0), OrderException);
    OrderQueue queue(2);
    CHECK(queue.isEmpty());
    CHECK_THROWS_AS(queue.next(), OrderException);
    queue.enqueue(KitchenOrder(1, RestaurantTable(1, 2)));
    queue.enqueue(KitchenOrder(2, RestaurantTable(2, 2), true));
    CHECK(queue.isFull());
    CHECK_THROWS_AS(queue.enqueue(KitchenOrder(3, RestaurantTable(3, 2))), OrderException);
    CHECK(queue.find(1).getId() == 1);
    CHECK_THROWS_AS(queue.find(99), OrderException);
    CHECK(queue.next().getId() == 2);
    CHECK(queue.pendingCount() == 1);
    CHECK(queue.getProcessedCount() == 1);
    CHECK(queue.getOrders().size() == 1);
}

TEST_CASE("ShoppingList: закупка в пределах бюджета") {
    CHECK_THROWS_AS(ShoppingList(testdata::makeSupplier(), -1), PaymentException);
    ShoppingList list(testdata::makeSupplier(), 10);
    CHECK_THROWS_AS(list.checkout(1), PaymentException);
    list.addItem(StockItem(testdata::makePotato(), 1000, 300, testdata::makeSupplier()));
    CHECK(list.getItems().empty());
    list.fillFrom(testdata::makeInventory(100, 500));
    CHECK(list.getItems().at("Картофель") == Approx(500));
    CHECK(list.getEstimatedCost() == Approx(5));
    CHECK(list.isWithinBudget());
    CHECK(list.checkout(3) == 5);
    CHECK(list.getItems().empty());
    ShoppingList expensive(testdata::makeSupplier(), 1);
    expensive.fillFrom(testdata::makeInventory(100, 500));
    CHECK_FALSE(expensive.isWithinBudget());
    CHECK_THROWS_AS(expensive.checkout(1), PaymentException);
    CHECK(expensive.getBudget() == Approx(1));
    CHECK(expensive.getSupplier().getName() == "Ферма");
}

TEST_CASE("WasteLog: списания и норма отходов") {
    WasteLog log;
    PerishableIngredient fish("Рыба", 0.02, NutritionalValue(120), 5, 2);
    CHECK_THROWS_AS(log.logWaste(fish, 0, "упала"), InvalidIngredientException);
    log.logWaste(*testdata::makePotato(), 100, "подгнил");
    CHECK_FALSE(log.logExpired(fish, 200, 5));
    CHECK(log.logExpired(fish, 200, 6));
    CHECK(log.getEntries().size() == 2);
    CHECK(log.getTotalAmount() == Approx(300));
    CHECK(log.getTotalCost() == Approx(1 + 4));
    CHECK(log.wastePercent(3000) == Approx(10));
    CHECK(log.isAboveNorm(3000));
    CHECK_FALSE(log.isAboveNorm(100000));
    CHECK_THROWS_AS(log.wastePercent(0), InvalidIngredientException);
}

TEST_CASE("Bill: чаевые, оплата, сдача, деление") {
    CHECK_THROWS_AS(Bill(KitchenOrder(1, RestaurantTable(1, 2)), 10), OrderException);
    CHECK_THROWS_AS(Bill(makeReadyOrder(), 60), PaymentException);
    Bill bill(makeReadyOrder(), 10);
    CHECK(bill.subtotal() == Approx(15));
    CHECK(bill.tip() == Approx(1.5));
    CHECK(bill.total() == Approx(16.5));
    CHECK(bill.change() == Approx(0));
    CHECK(bill.splitBetween(3) == Approx(5.5));
    CHECK_THROWS_AS(bill.splitBetween(0), PaymentException);
    CHECK_THROWS_AS(bill.pay(10), PaymentException);
    bill.pay(20);
    CHECK(bill.isPaid());
    CHECK(bill.change() == Approx(3.5));
    CHECK_THROWS_AS(bill.pay(20), PaymentException);
    CHECK(bill.getPaidAmount() == Approx(20));
    CHECK(bill.getOrder().getId() == 1);
}

TEST_CASE("Kitchen: посадка, заказ, готовка, оплата, отчёт") {
    Kitchen kitchen = makeKitchen();
    CHECK_THROWS_AS(kitchen.seatGuests(10), TableException);
    CHECK(kitchen.seatGuests(2) == 1);
    CHECK_THROWS_AS(kitchen.placeOrder(5, {"Драники"}, false), TableException);
    CHECK_THROWS_AS(kitchen.placeOrder(2, {"Драники"}, false), TableException);
    CHECK_THROWS_AS(kitchen.placeOrder(1, {"Суп"}, false), DishNotFoundException);
    CHECK(kitchen.placeOrder(1, {"Драники", "Бифштекс"}, false) == 1);
    std::vector<std::string> log = kitchen.processNextOrder();
    CHECK(log.front() == "Заказ №1 (столик №1)");
    CHECK(log.back() == "Заказ №1 готов");
    CHECK(kitchen.getReadyOrders().size() == 1);
    CHECK_THROWS_AS(kitchen.createBill(99, 0), OrderException);
    Bill bill = kitchen.createBill(1, 0);
    CHECK(kitchen.acceptPayment(bill, 50) == Approx(20));
    CHECK(kitchen.getRevenue() == Approx(30));
    kitchen.releaseTable(1);
    CHECK_FALSE(kitchen.getTables()[0].isOccupied());
    CHECK(kitchen.dailyReport().find("Выручка: 30.00 BYN") != std::string::npos);
    CHECK(kitchen.getName() == "Тест");
    CHECK(kitchen.getChef().teamSize() == 1);
    CHECK(kitchen.getQueue().getProcessedCount() == 1);
}

TEST_CASE("Kitchen: нет продуктов, нет блюда в наличии, закупка, списание") {
    Kitchen kitchen = makeKitchen(250);
    kitchen.seatGuests(4);
    kitchen.placeOrder(2, {"Драники", "Бифштекс"}, true);
    CHECK(kitchen.processNextOrder().front() == "Заказ №1 отменён: не хватает продуктов");
    ShoppingList list = kitchen.makeShoppingList(testdata::makeSupplier(), 100);
    CHECK(list.getItems().count("Картофель") == 1);
    kitchen.getMenu().findDish("Драники")->markSoldOut();
    CHECK_THROWS_AS(kitchen.placeOrder(2, {"Драники"}, false), DishNotAvailableException);
    CHECK(kitchen.writeOffExpired(1) == 0);
    CHECK(kitchen.getWasteLog().getEntries().empty());
    CHECK(kitchen.getInventory().getStock().size() == 2);
}

TEST_CASE("Kitchen: списание просроченного со склада") {
    auto fish = std::make_shared<PerishableIngredient>("Рыба", 0.02, NutritionalValue(120), 3, 2);
    KitchenInventory inventory("Кухня");
    inventory.addStock(StockItem(fish, 500, 100, testdata::makeSupplier()));
    inventory.addStock(StockItem(testdata::makePotato(), 500, 100, testdata::makeSupplier()));
    Kitchen kitchen("Тест", Menu("Меню", "Зима"), inventory, testdata::makeChef());
    CHECK(kitchen.writeOffExpired(2) == 0);
    CHECK(kitchen.writeOffExpired(5) == 1);
    CHECK(kitchen.getInventory().availableAmount("Рыба") == Approx(0));
    CHECK(kitchen.getWasteLog().getTotalCost() == Approx(10));
    CHECK(kitchen.writeOffExpired(6) == 0);
}
