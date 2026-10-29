#include <memory>

#include "TestData.h"
#include "appliances/Blender.h"
#include "appliances/FoodProcessor.h"
#include "appliances/KitchenInventory.h"
#include "appliances/MeatGrinder.h"
#include "appliances/Oven.h"
#include "appliances/SmartScale.h"
#include "appliances/StoveTop.h"
#include "doctest.h"
#include "exceptions/CulinaryException.h"
#include "ingredients/LiquidIngredient.h"

using doctest::Approx;

namespace {

/// Простейший нагреватель, чтобы проверить ThermalAppliance с любой максимальной температурой.
class TestHeater : public ThermalAppliance {
public:
    using ThermalAppliance::ThermalAppliance;
    std::string type() const override { return "Нагреватель"; }
};

}  // namespace

TEST_CASE("KitchenAppliance: включение, работа, энергия, обслуживание") {
    CHECK_THROWS_AS(SmartScale("", 5, 1000), ApplianceException);
    CHECK_THROWS_AS(SmartScale("Весы", 0, 1000), ApplianceException);
    TestHeater heater("Тэн", 1000, 100);
    CHECK_THROWS_AS(heater.use(10), ApplianceException);
    heater.turnOn();
    CHECK(heater.isOn());
    CHECK_THROWS_AS(heater.use(0), ApplianceException);
    heater.use(60);
    CHECK(heater.getUsageMinutes() == 60);
    CHECK(heater.energyUsedKwh() == Approx(1));
    CHECK_FALSE(heater.needsMaintenance());
    heater.use(KitchenAppliance::MAINTENANCE_MINUTES);
    CHECK(heater.needsMaintenance());
    heater.service();
    CHECK(heater.getUsageMinutes() == 0);
    heater.turnOff();
    CHECK_FALSE(heater.isOn());
    CHECK(heater.getPowerWatts() == 1000);
}

TEST_CASE("ThermalAppliance: нагрев и проверка процесса") {
    CHECK_THROWS_AS(TestHeater("Тэн", 1000, 20), InvalidTemperatureException);
    TestHeater heater("Тэн", 1000, 150);
    CHECK_THROWS_AS(heater.heatTo(100), ApplianceException);
    heater.turnOn();
    CHECK_THROWS_AS(heater.heatTo(200), InvalidTemperatureException);
    CHECK_THROWS_AS(heater.heatTo(0), InvalidTemperatureException);
    heater.heatTo(120);
    CHECK(heater.getCurrentTemperature() == 120);
    heater.coolDown();
    CHECK(heater.getCurrentTemperature() == ThermalAppliance::ROOM_TEMPERATURE);
    CHECK(heater.canPerform(ThermalProcess("Варка", 10, 100)));
    CHECK_FALSE(heater.canPerform(ThermalProcess("Жарка", 10, 200)));
    CHECK(heater.getMaxTemperature() == 150);
    CHECK(heater.type() == "Нагреватель");
}

TEST_CASE("Oven: разогрев и запекание") {
    CHECK_THROWS_AS(Oven("Bosch", 3000, 0, false), ApplianceException);
    Oven oven("Bosch", 3000, 2, false);
    CHECK(oven.type() == "Духовка");
    CHECK(oven.preheatMinutes(200) == 18);
    CHECK(Oven("Bosch", 3000, 2, true).preheatMinutes(200) == 9);
    CHECK(oven.preheatMinutes(10) == 0);
    Recipe recipe = testdata::makeRecipe();
    oven.turnOn();
    CHECK_THROWS_AS(oven.bake(recipe, 1), ApplianceException);
    oven.heatTo(180);
    CHECK_THROWS_AS(oven.bake(recipe, 3), ApplianceException);
    CHECK(oven.bake(recipe, 1).find("Пюре") != std::string::npos);
    CHECK(oven.getUsageMinutes() == recipe.totalTime());
    CHECK(oven.getTrays() == 2);
    CHECK_FALSE(oven.hasConvection());
}

TEST_CASE("StoveTop: конфорки") {
    CHECK_THROWS_AS(StoveTop("Gefest", 6000, 0, true), ApplianceException);
    StoveTop stove("Gefest", 6000, 2, true);
    CHECK(stove.type() == "Плита");
    CHECK_THROWS_AS(stove.occupyBurner(), ApplianceException);
    stove.turnOn();
    stove.occupyBurner();
    stove.occupyBurner();
    CHECK(stove.freeBurners() == 0);
    CHECK_THROWS_AS(stove.occupyBurner(), ApplianceException);
    stove.releaseBurner();
    stove.releaseBurner();
    CHECK_THROWS_AS(stove.releaseBurner(), ApplianceException);
    CHECK(stove.freeBurners() == 2);
    CHECK(stove.getBurners() == 2);
    CHECK(stove.isGas());
}

TEST_CASE("SafeProcessor и Blender: крышка, перегруз, смешивание, лёд") {
    CHECK_THROWS_AS(Blender("Philips", 800, 3, 0), ApplianceException);
    CHECK_THROWS_AS(Blender("Philips", 800, 0, 1000), ApplianceException);
    Blender blender("Philips", 800, 5, 1000);
    LiquidIngredient milk("Молоко", 0.002, NutritionalValue(52), 1.0, 100);
    CHECK(blender.type() == "Блендер");
    CHECK_THROWS_AS(blender.checkReady(100), ApplianceException);
    blender.turnOn();
    CHECK_THROWS_AS(blender.checkReady(100), ApplianceException);
    blender.closeLid();
    CHECK(blender.isLidClosed());
    CHECK_THROWS_AS(blender.checkReady(5000), ApplianceException);
    CHECK_THROWS_AS(blender.blend(milk, 2000, 1), ApplianceException);
    CHECK(blender.blend(milk, 500, 2) == "Взбито 500.00 мл: Молоко");
    CHECK(blender.crushIce(200) == "Наколото льда: 200.00 г");
    Blender weak("Vitek", 300, 1, 500);
    CHECK_THROWS_AS(weak.crushIce(100), ApplianceException);
    blender.openLid();
    CHECK_FALSE(blender.isLidClosed());
    CHECK(blender.getMaxLoadGrams() == Approx(1000));
    CHECK(blender.getSpeedLevels() == 5);
    CHECK(blender.getJugVolumeMl() == Approx(1000));
}

TEST_CASE("FoodProcessor: насадки и нарезка") {
    FoodProcessor processor("Kenwood", 1000, 2000);
    SolidIngredient onion("Лук", 0.003, NutritionalValue(40), 80);
    CHECK(processor.type() == "Кухонный комбайн");
    processor.turnOn();
    processor.closeLid();
    CHECK_THROWS_AS(processor.process(onion, 100, 1), ApplianceException);
    processor.addAttachment("нож");
    CHECK_THROWS_AS(processor.selectAttachment("тёрка"), ApplianceException);
    processor.selectAttachment("нож");
    CHECK(processor.getCurrentAttachment() == "нож");
    CHECK(processor.process(onion, 100, 1).find("Лук") != std::string::npos);
    CHECK(onion.isChopped());
    CHECK(processor.getAttachments().size() == 1);
}

TEST_CASE("MeatGrinder: время и решётка") {
    CHECK_THROWS_AS(MeatGrinder("Moulinex", 1600, 3000, 0, 6), ApplianceException);
    MeatGrinder grinder("Moulinex", 1600, 3000, 1000, 6);
    SolidIngredient beef("Говядина", 0.03, NutritionalValue(250), 500);
    CHECK(grinder.type() == "Мясорубка");
    CHECK(grinder.grindingMinutes(1500) == 2);
    grinder.turnOn();
    grinder.closeLid();
    CHECK(grinder.grind(beef, 1500).find("Говядина") != std::string::npos);
    CHECK(grinder.getUsageMinutes() == 2);
    grinder.changePlate(4.5);
    CHECK(grinder.getPlateHoleMm() == Approx(4.5));
    CHECK_THROWS_AS(grinder.changePlate(0), ApplianceException);
    CHECK(grinder.getGramsPerMinute() == Approx(1000));
}

TEST_CASE("SmartScale: тара, взвешивание, проверка порции") {
    CHECK_THROWS_AS(SmartScale("Xiaomi", 5, 0), ApplianceException);
    SmartScale scale("Xiaomi", 5, 5000);
    CHECK(scale.type() == "Весы");
    CHECK_THROWS_AS(scale.weigh(100), ApplianceException);
    scale.turnOn();
    CHECK_THROWS_AS(scale.setTare(-1), ApplianceException);
    scale.setTare(100);
    CHECK(scale.getTare() == Approx(100));
    CHECK(scale.weigh(350) == Approx(250));
    CHECK(scale.getLastWeight() == Approx(250));
    CHECK_THROWS_AS(scale.weigh(6000), ApplianceException);
    IngredientProportion portion(testdata::makePotato(), 200);
    CHECK(scale.checkPortion(portion, 305, 5));
    CHECK_FALSE(scale.checkPortion(portion, 400, 5));
}

TEST_CASE("KitchenInventory: приборы и склад") {
    KitchenInventory inventory = testdata::makeInventory();
    CHECK(inventory.getLocation() == "Кухня");
    CHECK_THROWS_AS(inventory.addAppliance(nullptr), ApplianceException);
    inventory.addAppliance(std::make_shared<StoveTop>("Gefest", 6000, 4, true));
    CHECK(inventory.findAppliance("Плита")->getModel() == "Gefest");
    CHECK_THROWS_AS(inventory.findAppliance("Духовка"), ApplianceException);
    CHECK(inventory.getAppliances().size() == 1);
    CHECK(inventory.availableAmount("Картофель") == Approx(1000));
    CHECK(inventory.availableAmount("Соль") == Approx(0));
    CHECK_THROWS_AS(inventory.findStock("Соль"), InsufficientStockException);
    Recipe recipe = testdata::makeRecipe();
    CHECK(inventory.hasEnoughFor(recipe));
    inventory.cookRecipe(recipe);
    CHECK(inventory.findStock("Мука").getQuantity() == Approx(400));
    CHECK(inventory.totalStockValue() == Approx(8 + 0.8));
    KitchenInventory poor = testdata::makeInventory(100, 500);
    CHECK_FALSE(poor.hasEnoughFor(recipe));
    CHECK_THROWS_AS(poor.cookRecipe(recipe), InsufficientStockException);
    REQUIRE(poor.itemsToRestock().size() == 1);
    CHECK(poor.itemsToRestock()[0].getIngredient().getName() == "Картофель");
    CHECK(poor.getStock().size() == 2);
}
