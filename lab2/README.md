# Лабораторная работа 2. Крупный ООП-проект

**Предметная область:** кулинария — работа ресторанной кухни.
Язык: C++17. Сборка: CMake. Тесты: doctest (файл `tests/doctest.h` лежит в проекте, скачивать ничего не надо).

Что умеет модель: продукты лежат на складе. Из них по рецептам готовятся блюда (по шагам: жарка, нарезка,
маринование). Блюда собраны в меню. Гостей сажают за столики, принимают заказы, шеф раздаёт блюда поварам,
потом выставляется счёт с чаевыми. Кроме того, просроченное списывается, а то, что заканчивается, попадает в список закупки.

## Выполнение требований

| Требование | Нужно | Сделано |
|---|---|---|
| Классы | ≥ 50 | **50** классов предметной области (+ 16 классов исключений) |
| Поля | ≥ 150 | **162** (161 в классах + 1 в базовом исключении) |
| Уникальные поведения (не геттеры/сеттеры) | ≥ 100 | **216** |
| Ассоциации между классами | ≥ 30 | **74** |
| Собственные исключения | ≥ 12 | **16** (базовое `CulinaryException` + 15 наследников) |
| README с описанием классов, полей, методов, ассоциаций, исключений | есть | этот файл |

Как считалось:

* **Поле** — закрытая переменная класса. Все они начинаются с `_`, например `_name`.
* **Поведение** — открытый метод с логикой: расчёт, проверка, изменение состояния. Геттеры вида
  `getName() { return _name; }`, конструкторы и деструкторы **не считаются**.
* **Ассоциация** — класс использует другой класс как поле (`Recipe` хранит `RecipeCategory`) или как параметр/результат
  метода (`Oven::bake(const Recipe&)`). Наследование (`Oven : ThermalAppliance`) ассоциацией **не считается**.

## Структура проекта

| Папка / файл | Что внутри |
|---|---|
| `include/<группа>/*.h` | объявления классов (по одному классу на файл) |
| `src/<группа>/*.cpp` | реализация методов |
| `include/exceptions/CulinaryException.h` | все собственные исключения |
| `include/Format.h` | функция `format::number` — вывод числа с двумя знаками после запятой |
| `app/` | консольное меню (CLI). Классы предметной области о нём ничего не знают |
| `tests/` | модульные тесты (50 тестов, 502 проверки) |
| `CMakeLists.txt` | сборка: библиотека `lab2_core`, программа `culinary_app`, тесты `lab2_tests` |

Группы классов: `ingredients` (продукты), `recipes` (рецепты), `appliances` (приборы и склад), `dishes` (блюда и меню),
`business` (персонал, столики, заказы, счёт, сам ресторан).

## Сборка, запуск, тесты

Нужны CMake и компилятор: MinGW (g++) или Visual Studio.

```bat
cmake -S . -B build -G Ninja
cmake --build build
build\bin\culinary_app.exe
ctest --test-dir build --output-on-failure
```

Под MinGW программы собираются статически, поэтому `.exe` запускается на любом компьютере без установленного компилятора.

**Покрытие тестами:** 99% строк кода в `src/` (1089 из 1100), измерено gcc + gcov. Нужно было больше 90%.

```bat
cmake -S . -B build -G Ninja -DENABLE_COVERAGE=ON
cmake --build build
build\bin\lab2_tests.exe
gcovr -r . --object-directory build --filter src/ --print-summary
```

Внимание: gcov не умеет работать с путями, где есть русские буквы. Для замера покрытия кладите проект в папку
с латинским путём.

## Консольное меню (CLI)

`culinary_app` запускает готовый ресторан «Бульбаш»: 5 блюд, комбо «Бизнес-ланч», склад, 6 приборов, шеф и 3 повара,
3 столика. Пункты меню:

1. Показать меню (цены, калории, комбо)
2. Показать столики
3. Посадить гостей
4. Принять заказ (можно срочный — он встанет в начало очереди)
5. Приготовить следующий заказ — шеф выбирает повара, со склада списываются продукты, выполняются шаги рецепта
6. Выставить счёт и принять оплату (чаевые, сдача)
7. Освободить столик
8. Склад и список закупки у поставщика
9. Списать просроченные продукты (вводится номер дня)
10. Блюда без аллергена
11. Отчёт за день

Пример: 3 → 2 гостя, 4 → столик 1, блюда 1, 2, 4, затем 0 → 5 → 6 → заказ 1, чаевые 10%, оплата 50 → 11.

## Главные идеи ООП в проекте

* **Инкапсуляция.** Все поля закрытые (`private`). Менять их можно только через методы, а методы проверяют данные.
  Например, `RestaurantTable::seat()` не посадит гостей за занятый столик.
* **Наследование.** Пять иерархий:
  * `Ingredient` → `SolidIngredient`, `LiquidIngredient`, `GaseousIngredient`, `PerishableIngredient`
  * `CookingProcess` → `ThermalProcess`, `MechanicalProcess`, `ChemicalProcess`
  * `KitchenAppliance` → `ThermalAppliance` (→ `Oven`, `StoveTop`), `SafeProcessor` (→ `Blender`, `FoodProcessor`,
    `MeatGrinder`), `SmartScale`
  * `Dish` → `Appetizer`, `MainCourse`, `Dessert`, `Beverage`
  * `KitchenStaff` → `Cook`, `Chef`
* **Полиморфизм.** Виртуальные методы `unit()`, `execute()`, `type()`, `course()`, `serve()`, `role()`. Например,
  `Menu` хранит `std::shared_ptr<Dish>`, а `dish->serve()` выводит текст, своё для каждого вида блюда.
* **Абстрактные классы.** `Ingredient`, `CookingProcess`, `KitchenAppliance`, `ThermalAppliance`, `SafeProcessor`, `Dish`,
  `KitchenStaff`. У них есть чисто виртуальный метод (`= 0`), поэтому создать их объект нельзя.
* **Ассоциация, агрегация, композиция.** `Recipe` содержит шаги и ингредиенты, `Menu` содержит блюда, `Kitchen` содержит
  всё остальное.
* **Исключения.** Ошибки предметной области — свои классы, наследники `CulinaryException`. CLI ловит их одним
  `catch (const CulinaryException&)` и печатает понятное сообщение.
* **Никаких магических чисел.** Константы вынесены в `static constexpr`, например `Beverage::ADULT_AGE = 18` или
  `WasteLog::NORM_PERCENT = 5.0`.

## Исключения (16)

Все объявлены в `include/exceptions/CulinaryException.h`. Базовый класс хранит поле `_message` и отдаёт его через `what()`.

| Исключение | Когда бросается (примеры) |
|---|---|
| `CulinaryException` | базовый класс, напрямую не бросается |
| `InvalidIngredientException` | пустое название, отрицательная цена/количество, поставка другого продукта |
| `ExpiredIngredientException` | `PerishableIngredient::checkFresh()` для просроченного продукта |
| `InsufficientStockException` | `StockItem::consume()` больше, чем есть; нет продукта на складе |
| `InvalidTemperatureException` | температура процесса вне 40..300 °C; прибор не держит такую температуру |
| `ApplianceException` | прибор выключен, открыта крышка, перегруз, нет насадки, все конфорки заняты |
| `RecipeException` | у рецепта нет шагов, неверное число порций, таймер не запущен, pH вне 0..14 |
| `InvalidRatingException` | оценка рецепта вне 1..5, рейтинг поставщика вне 0..5 |
| `InvalidPriceException` | отрицательная цена, скидка больше 90% |
| `DishNotFoundException` | блюда нет в меню, пустое блюдо |
| `DishNotAvailableException` | блюдо закончилось (`markSoldOut()`) |
| `OrderException` | неверный переход статуса заказа, очередь пуста или полна, заказ не найден |
| `TableException` | столик занят, не найден, мало мест |
| `PaymentException` | денег не хватает, счёт уже оплачен, закупка дороже бюджета |
| `AgeRestrictionException` | алкогольный напиток гостю младше 18 лет |
| `StaffException` | смена больше 12 часов, нет повара нужной специализации |

## Сводная таблица классов

Формат как в примере задания: класс, число полей, число методов-поведений (без геттеров и конструкторов),
от кого наследуется, с какими классами связан.

| Класс | Поля | Методы-поведения | Наследует | Ассоциации (связанные классы) |
|---|---|---|---|---|
| Allergen | 3 | 3 | — | — |
| Appetizer | 2 | 3 | Dish | Recipe, PlatingStyle, PriceTag |
| Beverage | 3 | 4 | Dish | Recipe, PlatingStyle, PriceTag |
| Bill | 4 | 6 | — | KitchenOrder |
| Blender | 2 | 3 | SafeProcessor | LiquidIngredient |
| Chef | 3 | 6 | KitchenStaff | Recipe, RecipeReview, Dish, Cook |
| ChemicalProcess | 2 | 2 | CookingProcess | — |
| ComboMeal | 3 | 6 | — | Dish |
| Cook | 3 | 4 | KitchenStaff | KitchenAppliance, Dish |
| CookingProcess | 3 | 2 | — | RecipeTimer |
| Dessert | 2 | 3 | Dish | Recipe, PlatingStyle, PriceTag |
| Dish | 5 | 7 | — | Recipe, PlatingStyle, PriceTag |
| FoodProcessor | 2 | 4 | SafeProcessor | SolidIngredient |
| GaseousIngredient | 2 | 3 | Ingredient | — |
| Ingredient | 4 | 5 | — | Allergen, NutritionalValue |
| IngredientPackage | 4 | 4 | — | Ingredient |
| IngredientProportion | 2 | 4 | — | Ingredient, NutritionalValue |
| Kitchen | 10 | 10 | — | Supplier, KitchenInventory, Menu, Bill, Chef, KitchenOrder, OrderQueue, RestaurantTable, ShoppingList, WasteLog |
| KitchenAppliance | 4 | 6 | — | — |
| KitchenInventory | 3 | 9 | — | StockItem, Recipe, KitchenAppliance |
| KitchenOrder | 6 | 6 | — | Dish, RestaurantTable |
| KitchenStaff | 4 | 4 | — | — |
| LiquidIngredient | 2 | 3 | Ingredient | NutritionalValue |
| MainCourse | 2 | 4 | Dish | Recipe, Beverage, PlatingStyle, PriceTag |
| MeatGrinder | 2 | 4 | SafeProcessor | SolidIngredient |
| MechanicalProcess | 2 | 2 | CookingProcess | — |
| Menu | 4 | 8 | — | ComboMeal, Dish |
| NutritionalValue | 4 | 3 | — | — |
| OrderQueue | 3 | 6 | — | KitchenOrder |
| Oven | 2 | 3 | ThermalAppliance | Recipe |
| PerishableIngredient | 3 | 5 | Ingredient | NutritionalValue |
| PlatingStyle | 3 | 2 | — | — |
| PortionScaler | 2 | 4 | — | IngredientProportion, Recipe, PriceTag |
| PriceTag | 3 | 3 | — | — |
| Recipe | 6 | 10 | — | NutritionalValue, IngredientProportion, RecipeCategory, RecipeReview, RecipeStep |
| RecipeCategory | 4 | 3 | — | — |
| RecipeReview | 3 | 2 | — | — |
| RecipeStep | 3 | 2 | — | CookingProcess |
| RecipeTimer | 3 | 5 | — | — |
| RestaurantTable | 4 | 3 | — | — |
| SafeProcessor | 2 | 4 | KitchenAppliance | — |
| ShoppingList | 4 | 4 | — | StockItem, Supplier, KitchenInventory |
| SmartScale | 3 | 4 | KitchenAppliance | IngredientProportion |
| SolidIngredient | 2 | 4 | Ingredient | NutritionalValue |
| StockItem | 4 | 6 | — | Ingredient, IngredientPackage, Supplier |
| StoveTop | 3 | 4 | ThermalAppliance | — |
| Supplier | 5 | 4 | — | — |
| ThermalAppliance | 2 | 3 | KitchenAppliance | ThermalProcess |
| ThermalProcess | 2 | 3 | CookingProcess | — |
| WasteLog | 3 | 4 | — | Ingredient, PerishableIngredient |

**Итого:** 50 классов, 161 поле, 216 методов-поведений, 74 ассоциации.

## Подробное описание классов

### Ингредиенты (ingredients)

#### Allergen

Аллерген (глютен, лактоза, орехи...) с уровнем опасности от 1 до 5.

- **Поля (3):** `_name` (std::string), `_severity` (int), `_description` (std::string)
- **Ассоциации:** нет
- **Поведения (3):**
  - `isSevere()` — Опасный ли аллерген (уровень >= SEVERE_LEVEL).
  - `matches()` — Совпадает ли название аллергена с заданным.
  - `warning()` — Текст предупреждения для меню.

#### GaseousIngredient : Ingredient

Газообразный ингредиент (углекислый газ для газировки, закись азота для сливок).

- **Поля (2):** `_pressure` (double), `_isFlammable` (bool)
- **Ассоциации:** нет
- **Поведения (3):**
  - `unit()` — своя реализация виртуального метода базового класса
  - `isDangerous()` — Опасен ли газ: горючий или под большим давлением.
  - `canBeStoredUnder()` — Можно ли хранить газ в баллоне, рассчитанном на заданное давление.

#### Ingredient

Абстрактный ингредиент: название, цена за единицу, пищевая ценность на 100 единиц, аллергены.

- **Поля (4):** `_name` (std::string), `_pricePerUnit` (double), `_nutrition` (NutritionalValue), `_allergens` (std::vector<Allergen>)
- **Ассоциации:** Allergen, NutritionalValue
- **Поведения (5):**
  - `describe()` — Краткое описание для вывода, например «Мука (г)».
  - `addAllergen()` — Добавляет аллерген (повторно тот же не добавляется).
  - `hasAllergen()` — Есть ли в ингредиенте аллерген с таким названием.
  - `costFor()` — Стоимость заданного количества.
  - `nutritionFor()` — Пищевая ценность заданного количества.

#### IngredientPackage

Упаковки ингредиента от поставщика, например «3 пачки муки по 1000 г».

- **Поля (4):** `_ingredient` (std::shared_ptr<Ingredient>), `_amountPerPackage` (double), `_packageCount` (int), `_packageType` (std::string)
- **Ассоциации:** Ingredient
- **Поведения (4):**
  - `totalAmount()` — Общее количество продукта во всех упаковках.
  - `totalPrice()` — Общая стоимость всех упаковок.
  - `takePackages()` — Забрать несколько упаковок (их становится меньше).
  - `label()` — Подпись, например «3 x пачка по 1000.00 г: Мука».

#### LiquidIngredient : Ingredient

Жидкий ингредиент (молоко, масло, бульон). Измеряется в миллилитрах.

- **Поля (2):** `_density` (double), `_boilingPoint` (int)
- **Ассоциации:** NutritionalValue
- **Поведения (3):**
  - `unit()` — своя реализация виртуального метода базового класса
  - `toGrams()` — Переводит миллилитры в граммы с учётом плотности.
  - `boilsAt()` — Закипает ли жидкость при заданной температуре.

#### NutritionalValue

Пищевая ценность: калории, белки, жиры, углеводы.

- **Поля (4):** `_calories` (double), `_proteins` (double), `_fats` (double), `_carbs` (double)
- **Ассоциации:** нет
- **Поведения (3):**
  - `scale()` — Умножает все показатели на коэффициент (например, пересчёт на другую массу).
  - `add()` — Складывает две пищевые ценности (итог по блюду).
  - `isLowCalorie()` — Проверяет, что калорий меньше LOW_CALORIE_LIMIT.

#### PerishableIngredient : Ingredient

Скоропортящийся ингредиент (рыба, сливки). Срок годности хранится как номер дня.

- **Поля (3):** `_expiryDay` (int), `_storageTemperature` (int), `_isFrozen` (bool)
- **Ассоциации:** NutritionalValue
- **Поведения (5):**
  - `unit()` — своя реализация виртуального метода базового класса
  - `isExpired()` — Просрочен ли продукт в заданный день.
  - `daysLeft()` — Сколько дней осталось до конца срока (0, если уже просрочен).
  - `checkFresh()` — Бросает ExpiredIngredientException, если продукт просрочен.
  - `freeze()` — Заморозить продукт: срок годности увеличивается на FREEZE_EXTRA_DAYS (только один раз).

#### SolidIngredient : Ingredient

Твёрдый ингредиент (лук, картофель, мясо). Измеряется в граммах, можно нарезать.

- **Поля (2):** `_pieceWeight` (double), `_isChopped` (bool)
- **Ассоциации:** NutritionalValue
- **Поведения (4):**
  - `unit()` — своя реализация виртуального метода базового класса
  - `describe()` — Описание с пометкой «нарезан», если ингредиент уже нарезан.
  - `chop()` — Нарезать ингредиент.
  - `piecesFor()` — Сколько штук нужно, чтобы набрать заданную массу (с округлением вверх).

#### StockItem

Позиция на складе: какой продукт, сколько его есть, минимальный запас и кто поставляет.

- **Поля (4):** `_ingredient` (std::shared_ptr<Ingredient>), `_quantity` (double), `_minQuantity` (double), `_supplier` (Supplier)
- **Ассоциации:** Ingredient, IngredientPackage, Supplier
- **Поведения (6):**
  - `consume()` — Списать продукт со склада. Бросает InsufficientStockException, если не хватает.
  - `restock()` — Пополнить склад на заданное количество.
  - `receivePackage()` — Принять поставку упаковок того же продукта.
  - `needsRestock()` — Пора ли дозаказывать (запас ниже минимума).
  - `amountToOrder()` — Сколько нужно дозаказать (0, если запаса хватает).
  - `value()` — Стоимость всего запаса.

#### Supplier

Поставщик продуктов: контакты, рейтинг, срок доставки.

- **Поля (5):** `_name` (std::string), `_phone` (std::string), `_city` (std::string), `_deliveryDays` (int), `_rating` (double)
- **Ассоциации:** нет
- **Поведения (4):**
  - `rate()` — Учитывает новую оценку: рейтинг становится средним старого и новой оценки.
  - `isReliable()` — Надёжный ли поставщик (рейтинг >= RELIABLE_RATING).
  - `deliveryDay()` — В какой день придёт поставка, если заказать сегодня.
  - `contactCard()` — Строка с контактами для списка покупок.

### Рецепты (recipes)

#### ChemicalProcess : CookingProcess

Химическая обработка: маринование, брожение, засолка с помощью вещества-агента.

- **Поля (2):** `_agent` (std::string), `_phLevel` (double)
- **Ассоциации:** нет
- **Поведения (2):**
  - `execute()` — Например: «Маринование (уксус, pH 3.50), 60 мин».
  - `isAcidic()` — Кислая ли среда (pH < NEUTRAL_PH).

#### CookingProcess

Абстрактный процесс приготовления (жарка, нарезка, маринование). У каждого процесса свой таймер.

- **Поля (3):** `_name` (std::string), `_durationMinutes` (int), `_timer` (RecipeTimer)
- **Ассоциации:** RecipeTimer
- **Поведения (2):**
  - `requiresHeat()` — Нужен ли нагрев (плита/духовка). По умолчанию — нет.
  - `run()` — Выполнить процесс: запустить таймер, дождаться окончания и вернуть описание.

#### IngredientProportion

Строка рецепта «сколько какого ингредиента»: например, «Мука: 200 г».

- **Поля (2):** `_ingredient` (std::shared_ptr<Ingredient>), `_amount` (double)
- **Ассоциации:** Ingredient, NutritionalValue
- **Поведения (4):**
  - `scaled()` — Та же строка, но количество умножено на коэффициент (для пересчёта порций).
  - `cost()` — Стоимость этого количества ингредиента.
  - `nutrition()` — Пищевая ценность этого количества ингредиента.
  - `describe()` — Текст, например «Мука: 200.00 г».

#### MechanicalProcess : CookingProcess

Механическая обработка: нарезка, взбивание, перемешивание инструментом.

- **Поля (2):** `_tool` (std::string), `_speed` (int)
- **Ассоциации:** нет
- **Поведения (2):**
  - `execute()` — Например: «Взбивание: венчик, 1200 об/мин, 5 мин».
  - `isFast()` — Быстрая ли обработка (скорость >= FAST_SPEED).

#### Recipe

Рецепт: название, категория, ингредиенты, шаги, отзывы и число порций.

- **Поля (6):** `_title` (std::string), `_category` (RecipeCategory), `_proportions` (std::vector<IngredientProportion>), `_steps` (std::vector<RecipeStep>), `_reviews` (std::vector<RecipeReview>), `_servings` (int)
- **Ассоциации:** NutritionalValue, IngredientProportion, RecipeCategory, RecipeReview, RecipeStep
- **Поведения (10):**
  - `addIngredient()` — Добавить ингредиент в рецепт.
  - `addStep()` — Добавить шаг приготовления.
  - `addReview()` — Добавить отзыв.
  - `totalTime()` — Общее время приготовления в минутах (сумма шагов).
  - `totalCost()` — Себестоимость всех ингредиентов.
  - `costPerServing()` — Себестоимость одной порции.
  - `averageRating()` — Средняя оценка по отзывам (0, если отзывов нет).
  - `cook()` — Приготовить: выполнить все шаги по порядку. Бросает RecipeException, если шагов нет.
  - `containsAllergen()` — Есть ли в рецепте ингредиент с таким аллергеном.
  - `totalNutrition()` — Пищевая ценность всего рецепта.

#### RecipeCategory

Категория рецепта: тип блюда, кухня, вегетарианское ли, сложность от 1 до 5.

- **Поля (4):** `_name` (std::string), `_cuisine` (std::string), `_isVegetarian` (bool), `_difficulty` (int)
- **Ассоциации:** нет
- **Поведения (3):**
  - `label()` — Подпись для меню, например «Итальянская кухня / Паста (вегетарианское)».
  - `matches()` — Подходит ли категория под поисковый запрос (совпадает тип или кухня).
  - `isBeginnerFriendly()` — Подходит ли рецепт новичку (сложность <= EASY_LEVEL).

#### RecipeReview

Отзыв о рецепте: автор, оценка от 1 до 5 и комментарий.

- **Поля (3):** `_author` (std::string), `_rating` (int), `_comment` (std::string)
- **Ассоциации:** нет
- **Поведения (2):**
  - `isPositive()` — Положительный ли отзыв (оценка >= POSITIVE_RATING).
  - `mentions()` — Упоминается ли слово в комментарии.

#### RecipeStep

Шаг рецепта: номер, текст инструкции и процесс, который выполняется на этом шаге.

- **Поля (3):** `_number` (int), `_instruction` (std::string), `_process` (std::shared_ptr<CookingProcess>)
- **Ассоциации:** CookingProcess
- **Поведения (2):**
  - `perform()` — Выполнить шаг и вернуть строку для журнала: «Шаг 1: ... — ...».
  - `duration()` — Сколько минут занимает шаг.

#### RecipeTimer

Кухонный таймер: отсчитывает минуты процесса приготовления.

- **Поля (3):** `_durationMinutes` (int), `_elapsedMinutes` (int), `_isRunning` (bool)
- **Ассоциации:** нет
- **Поведения (5):**
  - `start()` — Запустить таймер.
  - `tick()` — Прошло несколько минут. Когда время вышло, таймер сам останавливается.
  - `isFinished()` — Вышло ли время.
  - `remaining()` — Сколько минут осталось.
  - `reset()` — Сбросить таймер в начальное состояние.

#### ThermalProcess : CookingProcess

Тепловая обработка: жарка, варка, запекание при заданной температуре.

- **Поля (2):** `_temperature` (int), `_method` (std::string)
- **Ассоциации:** нет
- **Поведения (3):**
  - `execute()` — Например: «Жарка при 200°C, 10 мин».
  - `requiresHeat()` — своя реализация виртуального метода базового класса
  - `isHighHeat()` — Сильный ли нагрев (температура >= HIGH_HEAT).

### Кухонные приборы (appliances)

#### Blender : SafeProcessor

Блендер: смешивает жидкости в чаше заданного объёма, на высоких скоростях колет лёд.

- **Поля (2):** `_speedLevels` (int), `_jugVolumeMl` (double)
- **Ассоциации:** LiquidIngredient
- **Поведения (3):**
  - `type()` — своя реализация виртуального метода базового класса
  - `blend()` — Взбить жидкость. Объём не должен превышать объём чаши.
  - `crushIce()` — Наколоть лёд (нужно не меньше ICE_SPEED_LEVEL скоростей).

#### FoodProcessor : SafeProcessor

Кухонный комбайн: набор насадок (тёрка, нож, шинковка), режет твёрдые продукты.

- **Поля (2):** `_attachments` (std::vector<std::string>), `_currentAttachment` (std::string)
- **Ассоциации:** SolidIngredient
- **Поведения (4):**
  - `type()` — своя реализация виртуального метода базового класса
  - `addAttachment()` — Добавить насадку в комплект.
  - `selectAttachment()` — Поставить насадку. Бросает ApplianceException, если такой насадки нет.
  - `process()` — Нарезать продукт текущей насадкой (продукт становится нарезанным).

#### KitchenAppliance

Абстрактный кухонный прибор: модель, мощность, включён ли, сколько минут отработал.

- **Поля (4):** `_model` (std::string), `_powerWatts` (int), `_isOn` (bool), `_usageMinutes` (int)
- **Ассоциации:** нет
- **Поведения (6):**
  - `turnOn()` — Включить прибор.
  - `turnOff()` — Выключить прибор.
  - `use()` — Поработать заданное число минут. Бросает ApplianceException, если прибор выключен.
  - `energyUsedKwh()` — Сколько электроэнергии (кВт·ч) прибор израсходовал за всё время.
  - `needsMaintenance()` — Пора ли на техобслуживание.
  - `service()` — Провести техобслуживание: счётчик минут обнуляется.

#### KitchenInventory

Инвентарь кухни: все приборы и склад продуктов.

- **Поля (3):** `_location` (std::string), `_appliances` (std::vector<std::shared_ptr<KitchenAppliance>>), `_stock` (std::vector<StockItem>)
- **Ассоциации:** StockItem, Recipe, KitchenAppliance
- **Поведения (9):**
  - `addAppliance()` — Добавить прибор.
  - `findAppliance()` — Найти прибор по типу. Бросает ApplianceException, если такого нет.
  - `addStock()` — Добавить позицию на склад.
  - `findStock()` — Найти позицию склада по названию продукта. Бросает InsufficientStockException, если нет.
  - `availableAmount()` — Сколько продукта есть на складе (0, если нет совсем).
  - `hasEnoughFor()` — Хватает ли продуктов, чтобы приготовить рецепт.
  - `cookRecipe()` — Списать со склада всё, что нужно для рецепта.
  - `itemsToRestock()` — Позиции, которые пора дозаказать.
  - `totalStockValue()` — Стоимость всего склада.

#### MeatGrinder : SafeProcessor

Мясорубка: перемалывает продукт со скоростью грамм в минуту через решётку с отверстиями.

- **Поля (2):** `_gramsPerMinute` (double), `_plateHoleMm` (double)
- **Ассоциации:** SolidIngredient
- **Поведения (4):**
  - `type()` — своя реализация виртуального метода базового класса
  - `grindingMinutes()` — Сколько минут нужно, чтобы перемолоть заданную массу (с округлением вверх).
  - `grind()` — Перемолоть продукт и вернуть описание результата.
  - `changePlate()` — Поменять решётку (диаметр отверстий в мм).

#### Oven : ThermalAppliance

Духовка: несколько противней, может быть с конвекцией (тогда греется быстрее).

- **Поля (2):** `_trays` (int), `_hasConvection` (bool)
- **Ассоциации:** Recipe
- **Поведения (3):**
  - `type()` — своя реализация виртуального метода базового класса
  - `preheatMinutes()` — Сколько минут нужно, чтобы разогреться до заданной температуры.
  - `bake()` — Запечь блюдо по рецепту. Духовка должна быть разогрета, противней — не больше, чем есть.

#### SafeProcessor : KitchenAppliance

Абстрактный прибор с ножами и защитной крышкой (блендер, комбайн, мясорубка).

- **Поля (2):** `_lidClosed` (bool), `_maxLoadGrams` (double)
- **Ассоциации:** нет
- **Поведения (4):**
  - `closeLid()` — Закрыть крышку.
  - `openLid()` — Открыть крышку.
  - `checkReady()` — Проверить, что можно работать: прибор включён, крышка закрыта, загрузка в норме.
  - `safeRun()` — Проверить готовность и поработать заданное число минут.

#### SmartScale : KitchenAppliance

Умные кухонные весы: учитывают тару и проверяют, правильно ли отмерена порция.

- **Поля (3):** `_maxWeight` (double), `_tare` (double), `_lastWeight` (double)
- **Ассоциации:** IngredientProportion
- **Поведения (4):**
  - `type()` — своя реализация виртуального метода базового класса
  - `setTare()` — Запомнить вес тары (миски), он будет вычитаться при взвешивании.
  - `weigh()` — Взвесить: возвращает вес без тары. Бросает ApplianceException при перегрузе.
  - `checkPortion()` — Взвесить продукт и проверить, что вес отличается от рецепта не больше чем на tolerancePercent.

#### StoveTop : ThermalAppliance

Варочная плита с несколькими конфорками (газовая или электрическая).

- **Поля (3):** `_burners` (int), `_busyBurners` (int), `_isGas` (bool)
- **Ассоциации:** нет
- **Поведения (4):**
  - `type()` — своя реализация виртуального метода базового класса
  - `occupyBurner()` — Занять свободную конфорку. Бросает ApplianceException, если плита выключена или все заняты.
  - `releaseBurner()` — Освободить конфорку.
  - `freeBurners()` — Сколько конфорок свободно.

#### ThermalAppliance : KitchenAppliance

Абстрактный нагревательный прибор: умеет нагреваться до температуры не выше максимальной.

- **Поля (2):** `_currentTemperature` (int), `_maxTemperature` (int)
- **Ассоциации:** ThermalProcess
- **Поведения (3):**
  - `heatTo()` — Нагреть до температуры. Прибор должен быть включён, температура — не выше максимальной.
  - `coolDown()` — Остудить до комнатной температуры.
  - `canPerform()` — Можно ли выполнить тепловой процесс на этом приборе (хватит ли температуры).

### Блюда и меню (dishes)

#### Appetizer : Dish

Закуска: холодная или горячая, из нескольких кусочков, которые можно разделить.

- **Поля (2):** `_isCold` (bool), `_pieces` (int)
- **Ассоциации:** Recipe, PlatingStyle, PriceTag
- **Поведения (3):**
  - `course()` — своя реализация виртуального метода базового класса
  - `serve()` — своя реализация виртуального метода базового класса
  - `piecesPerPerson()` — Сколько кусочков достанется каждому, если делить на компанию.

#### Beverage : Dish

Напиток: объём, алкогольный или нет, температура подачи.

- **Поля (3):** `_volumeMl` (int), `_isAlcoholic` (bool), `_servingTemperature` (int)
- **Ассоциации:** Recipe, PlatingStyle, PriceTag
- **Поведения (4):**
  - `course()` — своя реализация виртуального метода базового класса
  - `serve()` — своя реализация виртуального метода базового класса
  - `checkAge()` — Проверить возраст гостя. Бросает AgeRestrictionException, если алкоголь и гость младше ADULT_AGE.
  - `isHot()` — Горячий ли напиток.

#### ComboMeal

Комбо-набор: несколько блюд со скидкой (например, «Бизнес-ланч»).

- **Поля (3):** `_title` (std::string), `_dishes` (std::vector<std::shared_ptr<Dish>>), `_discountPercent` (double)
- **Ассоциации:** Dish
- **Поведения (6):**
  - `addDish()` — Добавить блюдо в набор.
  - `fullPrice()` — Сумма цен блюд без скидки набора.
  - `totalPrice()` — Цена набора со скидкой.
  - `savings()` — Сколько гость экономит, беря набор.
  - `totalCalories()` — Калорийность всего набора (по одной порции каждого блюда).
  - `hasBeverage()` — Есть ли в наборе напиток.

#### Dessert : Dish

Десерт: количество сахара, замороженный или нет (мороженое).

- **Поля (2):** `_sugarGrams` (double), `_isFrozen` (bool)
- **Ассоциации:** Recipe, PlatingStyle, PriceTag
- **Поведения (3):**
  - `course()` — своя реализация виртуального метода базового класса
  - `serve()` — своя реализация виртуального метода базового класса
  - `isSugarFree()` — Можно ли назвать десерт «без сахара».

#### Dish

Абстрактное блюдо меню: название, рецепт, цена, подача и есть ли оно в наличии.

- **Поля (5):** `_name` (std::string), `_recipe` (Recipe), `_price` (PriceTag), `_plating` (PlatingStyle), `_isAvailable` (bool)
- **Ассоциации:** Recipe, PlatingStyle, PriceTag
- **Поведения (7):**
  - `serve()` — Как блюдо подаётся гостю. Наследники дополняют текст.
  - `prepare()` — Приготовить: выполнить рецепт и подать. Бросает DishNotAvailableException, если блюдо закончилось.
  - `caloriesPerServing()` — Калорийность одной порции.
  - `hasAllergen()` — Есть ли в блюде аллерген.
  - `isVegetarian()` — Вегетарианское ли блюдо (по категории рецепта).
  - `markSoldOut()` — Отметить, что блюдо закончилось.
  - `markAvailable()` — Снова сделать блюдо доступным.

#### MainCourse : Dish

Основное блюдо: с гарниром, может быть острым.

- **Поля (2):** `_sideDish` (std::string), `_isSpicy` (bool)
- **Ассоциации:** Recipe, Beverage, PlatingStyle, PriceTag
- **Поведения (4):**
  - `course()` — своя реализация виртуального метода базового класса
  - `serve()` — своя реализация виртуального метода базового класса
  - `pairWith()` — Рекомендация официанта: подходит ли напиток к блюду (острое не советуют с горячим напитком).
  - `spiceWarning()` — Предупреждение для меню, если блюдо острое (иначе пустая строка).

#### Menu

Меню ресторана: блюда и комбо-наборы.

- **Поля (4):** `_title` (std::string), `_season` (std::string), `_dishes` (std::vector<std::shared_ptr<Dish>>), `_combos` (std::vector<ComboMeal>)
- **Ассоциации:** ComboMeal, Dish
- **Поведения (8):**
  - `addDish()` — Добавить блюдо в меню.
  - `removeDish()` — Убрать блюдо из меню. Бросает DishNotFoundException, если его нет.
  - `findDish()` — Найти блюдо по названию. Бросает DishNotFoundException, если его нет.
  - `addCombo()` — Добавить комбо-набор.
  - `availableDishes()` — Блюда, которые сейчас есть в наличии.
  - `cheapestDish()` — Самое дешёвое блюдо. Бросает DishNotFoundException, если меню пустое.
  - `withoutAllergen()` — Блюда без указанного аллергена (для гостя с аллергией).
  - `vegetarianDishes()` — Вегетарианские блюда.

#### PlatingStyle

Способ подачи блюда: название стиля, тип тарелки и украшение.

- **Поля (3):** `_name` (std::string), `_plateType` (std::string), `_garnish` (std::string)
- **Ассоциации:** нет
- **Поведения (2):**
  - `describe()` — Текст для официанта: стиль, тарелка и украшение (если есть).
  - `hasGarnish()` — Есть ли украшение.

#### PortionScaler

Пересчёт порций: например, рецепт на 2 порции пересчитать на 6.

- **Поля (2):** `_baseServings` (int), `_targetServings` (int)
- **Ассоциации:** IngredientProportion, Recipe, PriceTag
- **Поведения (4):**
  - `factor()` — Коэффициент пересчёта: target / base.
  - `scale()` — Ингредиенты рецепта, пересчитанные на новое число порций.
  - `scaleCost()` — Себестоимость рецепта на новое число порций.
  - `scalePrice()` — Цена блюда на новое число порций.

#### PriceTag

Ценник блюда: базовая цена, скидка в процентах и валюта.

- **Поля (3):** `_basePrice` (double), `_discountPercent` (double), `_currency` (std::string)
- **Ассоциации:** нет
- **Поведения (3):**
  - `finalPrice()` — Цена с учётом скидки.
  - `applyDiscount()` — Установить скидку (от 0 до MAX_DISCOUNT процентов).
  - `format()` — Текст ценника, например «12.50 BYN» или «10.00 BYN (скидка 20.00%)».

### Работа ресторана (business)

#### Bill

Счёт за готовый заказ: сумма, чаевые, оплата и сдача.

- **Поля (4):** `_order` (KitchenOrder), `_tipPercent` (double), `_paidAmount` (double), `_isPaid` (bool)
- **Ассоциации:** KitchenOrder
- **Поведения (6):**
  - `subtotal()` — Сумма блюд.
  - `tip()` — Чаевые.
  - `total()` — Итого к оплате.
  - `pay()` — Оплатить. Бросает PaymentException, если денег мало или счёт уже оплачен.
  - `change()` — Сдача (0, если ещё не оплачено).
  - `splitBetween()` — Сколько платит каждый, если делить счёт поровну.

#### Chef : KitchenStaff

Шеф-повар: руководит командой поваров и распределяет между ними блюда.

- **Поля (3):** `_cooks` (std::vector<std::shared_ptr<Cook>>), `_signatureDish` (std::string), `_michelinStars` (int)
- **Ассоциации:** Recipe, RecipeReview, Dish, Cook
- **Поведения (6):**
  - `role()` — своя реализация виртуального метода базового класса
  - `hireCook()` — Нанять повара в команду.
  - `fireCook()` — Уволить повара по имени. Бросает StaffException, если такого нет.
  - `chooseCook()` — Выбрать для блюда наименее загруженного подходящего повара. Бросает StaffException, если таких нет.
  - `reviewRecipe()` — Оценить рецепт: шеф пишет отзыв от своего имени.
  - `teamSize()` — Размер команды (сколько поваров).

#### Cook : KitchenStaff

Повар: работает на своём участке (станции) и специализируется на разделе меню.

- **Поля (3):** `_station` (std::string), `_specialty` (std::string), `_dishesCooked` (int)
- **Ассоциации:** KitchenAppliance, Dish
- **Поведения (4):**
  - `role()` — своя реализация виртуального метода базового класса
  - `canCook()` — Умеет ли повар готовить это блюдо (совпадает ли раздел меню со специализацией).
  - `cook()` — Приготовить блюдо. Бросает StaffException, если блюдо не по специализации.
  - `useAppliance()` — Включить прибор и поработать на нём заданное число минут.

#### Kitchen

Ресторан целиком: меню, склад, шеф с поварами, столики, очередь заказов, выручка.

- **Поля (10):** `_name` (std::string), `_menu` (Menu), `_inventory` (KitchenInventory), `_chef` (Chef), `_queue` (OrderQueue), `_tables` (std::vector<RestaurantTable>), `_readyOrders` (std::vector<KitchenOrder>), `_wasteLog` (WasteLog), `_nextOrderId` (int), `_revenue` (double)
- **Ассоциации:** Supplier, KitchenInventory, Menu, Bill, Chef, KitchenOrder, OrderQueue, RestaurantTable, ShoppingList, WasteLog
- **Поведения (10):**
  - `addTable()` — Добавить столик в зал.
  - `seatGuests()` — Посадить гостей за первый подходящий свободный столик. Возвращает номер столика.
  - `placeOrder()` — Принять заказ от занятого столика. Возвращает номер заказа.
  - `processNextOrder()` — Приготовить следующий заказ из очереди. Возвращает журнал готовки.
  - `createBill()` — Выставить счёт за готовый заказ.
  - `acceptPayment()` — Принять оплату по счёту. Возвращает сдачу.
  - `releaseTable()` — Освободить столик.
  - `writeOffExpired()` — Списать со склада всё просроченное. Возвращает число списанных позиций.
  - `makeShoppingList()` — Составить список закупки у поставщика по тому, что заканчивается.
  - `dailyReport()` — Отчёт за день: столики, заказы, выручка, склад, отходы.

#### KitchenOrder

Заказ на кухню: номер, столик, блюда, статус, ответственный и срочность.

- **Поля (6):** `_id` (int), `_tableNumber` (int), `_dishes` (std::vector<std::shared_ptr<Dish>>), `_status` (OrderStatus), `_assignedCook` (std::string), `_isUrgent` (bool)
- **Ассоциации:** Dish, RestaurantTable
- **Поведения (6):**
  - `addDish()` — Добавить блюдо (только в новый заказ).
  - `startCooking()` — Начать готовить. Заказ должен быть новым и непустым.
  - `markReady()` — Заказ готов. Можно только из статуса Cooking.
  - `cancel()` — Отменить заказ. Готовый заказ отменить нельзя.
  - `total()` — Сумма заказа.
  - `statusName()` — Статус словами: «Новый», «Готовится», «Готов», «Отменён».

#### KitchenStaff

Абстрактный сотрудник кухни: имя, стаж, почасовая ставка, отработанные часы.

- **Поля (4):** `_name` (std::string), `_experienceYears` (int), `_hourlyRate` (double), `_hoursWorked` (double)
- **Ассоциации:** нет
- **Поведения (4):**
  - `logHours()` — Записать отработанную смену. Бросает StaffException, если часов <= 0 или больше MAX_SHIFT_HOURS.
  - `salary()` — Зарплата за отработанные часы.
  - `isSenior()` — Опытный ли сотрудник.
  - `badge()` — Бейдж: «Повар Иван, стаж 3 г.».

#### OrderQueue

Очередь заказов на кухне. Срочные заказы встают в начало очереди.

- **Поля (3):** `_orders` (std::deque<KitchenOrder>), `_maxSize` (int), `_processedCount` (int)
- **Ассоциации:** KitchenOrder
- **Поведения (6):**
  - `enqueue()` — Поставить заказ в очередь. Бросает OrderException, если очередь переполнена.
  - `next()` — Забрать следующий заказ. Бросает OrderException, если очередь пуста.
  - `find()` — Найти заказ в очереди по номеру. Бросает OrderException, если не найден.
  - `pendingCount()` — Сколько заказов ждёт.
  - `isFull()` — Заполнена ли очередь.
  - `isEmpty()` — Пуста ли очередь.

#### RestaurantTable

Столик в зале: номер, число мест, сколько гостей сидит, занят ли.

- **Поля (4):** `_number` (int), `_seats` (int), `_guests` (int), `_isOccupied` (bool)
- **Ассоциации:** нет
- **Поведения (3):**
  - `canSeat()` — Можно ли посадить столько гостей (столик свободен и мест хватает).
  - `seat()` — Посадить гостей. Бросает TableException, если столик занят или мест мало.
  - `release()` — Гости ушли — столик свободен.

#### ShoppingList

Список закупки у поставщика: что и сколько купить, укладываемся ли в бюджет.

- **Поля (4):** `_supplier` (Supplier), `_budget` (double), `_items` (std::map<std::string, double>), `_estimatedCost` (double)
- **Ассоциации:** StockItem, Supplier, KitchenInventory
- **Поведения (4):**
  - `addItem()` — Добавить в список то, что нужно дозаказать по позиции склада.
  - `fillFrom()` — Заполнить список всем, что заканчивается на складе.
  - `isWithinBudget()` — Укладывается ли закупка в бюджет.
  - `checkout()` — Оформить закупку: список очищается, возвращается день доставки. Бросает PaymentException, если список пуст или превышен бюджет.

#### WasteLog

Журнал списаний: что выбросили, сколько и почему.

- **Поля (3):** `_entries` (std::vector<std::string>), `_totalCost` (double), `_totalAmount` (double)
- **Ассоциации:** Ingredient, PerishableIngredient
- **Поведения (4):**
  - `logWaste()` — Записать списание продукта с причиной.
  - `logExpired()` — Списать продукт, если он просрочен. Возвращает true, если списали.
  - `wastePercent()` — Какой процент от закупленного ушёл в отходы.
  - `isAboveNorm()` — Превышена ли норма отходов.
