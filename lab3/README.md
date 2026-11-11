# Лабораторная работа 3. Ещё один крупный ООП-проект

**Предметная область:** система «Банкомат».
Язык: C++17. Сборка: CMake. Тесты: doctest (файл `tests/doctest.h` лежит в проекте, скачивать ничего не надо).

Что умеет модель: у банка есть клиенты, счета (текущие и сберегательные, в разных валютах) и карты
(дебетовая с кэшбэком, кредитная, предоплаченная). Клиент вставляет карту в банкомат, вводит PIN-код и может
узнать баланс, снять наличные (банкомат сам раскладывает сумму на купюры), внести наличные (детектор отбраковывает
фальшивые купюры), перевести деньги на счёт (с конвертацией валюты), оплатить мобильную связь или коммунальный счёт,
сменить PIN, получить мини-выписку и чек. Процессинговый центр одобряет операции, антифрод ловит подозрительные суммы,
клиенту приходят SMS. Заблокированные и забытые карты банкомат изымает. Инкассатор загружает наличные и забирает
внесённые деньги, техник чинит сломанные устройства и ведёт журнал обслуживания.

## Выполнение требований

| Требование | Нужно | Сделано |
|---|---|---|
| Классы | ≥ 50 | **50** классов предметной области (+ 18 классов исключений) |
| Поля | ≥ 150 | **199** (198 в классах + 1 в базовом исключении) |
| Уникальные поведения (не геттеры/сеттеры) | ≥ 100 | **280** |
| Ассоциации между классами | ≥ 30 | **65** |
| Собственные исключения | ≥ 12 | **18** (базовое `AtmException` + 17 наследников) |
| README с описанием классов, полей, методов, ассоциаций, исключений | есть | этот файл |

Как считалось:

* **Поле** — закрытая переменная класса. Все они начинаются с `_`, например `_balance`.
* **Поведение** — открытый метод с логикой: расчёт, проверка, изменение состояния. Геттеры вида
  `getBalance() { return _balance; }`, конструкторы и деструкторы **не считаются**.
* **Ассоциация** — класс использует другой класс как поле (`Card` хранит `PinCode`) или как параметр/результат
  метода (`ReceiptPrinter::print(const Receipt&)`). Наследование (`CreditCard : Card`) ассоциацией **не считается**.

## Структура проекта

| Папка / файл | Что внутри |
|---|---|
| `include/<группа>/*.h` | объявления классов (по одному классу на файл) |
| `src/<группа>/*.cpp` | реализация методов |
| `include/exceptions/AtmException.h` | все собственные исключения |
| `include/Format.h` | функция `format::number` — вывод числа с двумя знаками после запятой |
| `app/` | консольное меню (CLI) и демо-данные. Классы предметной области о нём ничего не знают |
| `tests/` | модульные тесты (59 тестов, 718 проверок) |
| `CMakeLists.txt` | сборка: библиотека `lab3_core`, программа `atm_app`, тесты `lab3_tests` |

Группы классов: `accounts` (счета, валюты, выписка), `cards` (карты, PIN, лимиты, клиенты), `hardware` (устройства
банкомата и купюры), `operations` (операции, чек, журнал, тарифы, сеанс), `bank` (банк, банкомат, сотрудники, сеть).

## Сборка, запуск, тесты

Нужны CMake и компилятор: MinGW (g++) или Visual Studio.

```bat
cmake -S . -B build -G Ninja
cmake --build build
build\bin\atm_app.exe
ctest --test-dir build --output-on-failure
```

Под MinGW программы собираются статически, поэтому `.exe` запускается на любом компьютере без установленного компилятора.

**Покрытие тестами:** 99% строк кода в `src/` (1589 из 1604), измерено gcc + gcov. Нужно было больше 90%.

```bat
cmake -S . -B build -G Ninja -DENABLE_COVERAGE=ON
cmake --build build
build\bin\lab3_tests.exe
gcovr -r . --object-directory build --filter src/ --print-summary
```

Внимание: gcov не умеет работать с путями, где есть русские буквы. Для замера покрытия кладите проект в папку
с латинским путём.

## Консольное меню (CLI)

`atm_app` запускает банкомат ATM-001 в Минске (пр. Независимости, 65). В нём есть наличные (купюры 200, 100, 50, 20, 10).
В банке «АТМ-Банк» два клиента с четырьмя картами, плюс есть карта чужого банка. PIN-коды показаны прямо в меню выбора карты, чтобы их можно было попробовать:

| Карта | Владелец | PIN | Что показать |
|---|---|---|---|
| Дебетовая …0010 | Иван Петров | 1234 | обычная работа, 1500 BYN |
| Дебетовая …0036 | Иван Петров | 0000 | срок действия истёк — карта возвращается |
| Кредитная …0040 | Анна Смирнова | 4321 | 200 BYN своих + 1000 BYN кредита |
| Предоплаченная …0028 | Анна Смирнова | 1111 | 100 BYN |
| Чужого банка …0056 | John Smith | 9999 | комиссия 1,5% (не меньше 2 BYN) |

Пункты меню: вставить карту, ввести PIN, баланс, снять наличные, внести наличные, перевод на счёт, оплата услуг,
оплата коммунального счёта, сменить PIN, мини-выписка, чек, забрать карту, подождать (тайм-аут — карта изымается),
состояние банкомата, инкассация, сломать устройство (демонстрация), вызвать техника, следующий день (обнуляет лимиты).
Служебный код банкомата для инкассатора и техника — `7777`.

Пример: 1 → 1 (карта Ивана) → 2 → `1234` → 4 → `380` → 5 → `100 50 30` (купюра 30 не существует и будет возвращена)
→ 11 (чек) → 12 (забрать карту) → 14 (состояние).

## Главные идеи ООП в проекте

* **Инкапсуляция.** Все поля закрытые (`private`). Менять их можно только через методы, а методы проверяют данные.
  Например, PIN-код хранится не открытым текстом, а как хеш (`PinCode::_hash`), и узнать его снаружи нельзя.
* **Наследование.** Пять иерархий:
  * `Card` → `DebitCard`, `CreditCard`, `PrepaidCard`
  * `BankAccount` → `SavingsAccount`
  * `AtmDevice` → `CardReader`, `Keypad`, `Display`, `CashDispenser`, `DepositModule`, `ReceiptPrinter`, `SecurityCamera`
  * `Transaction` → `Withdrawal`, `Deposit`, `Transfer`, `BalanceInquiry`, `ServicePayment`
  * `Employee` → `CashCollector`, `Technician`
* **Полиморфизм.** Виртуальные методы `type()`, `availableFunds()`, `charge()`, `deviceName()`, `service()`, `describe()`,
  `role()`. Например, `CreditCard::charge()` сначала тратит свои деньги, а потом кредитные, а `DebitCard` — только свои;
  `Technician` вызывает `device.service()`, и для принтера это ещё и заправка бумаги.
* **Абстрактные классы.** `Card`, `AtmDevice`, `Transaction`, `Employee` — у них есть чисто виртуальный метод (`= 0`),
  поэтому создать их объект нельзя.
* **Ассоциация, агрегация, композиция.** `Atm` состоит из устройств (композиция), хранит общий `Bank` через
  `shared_ptr` (агрегация), `Card` хранит `PinCode` и `CardLimit`.
* **Исключения.** Ошибки предметной области — свои классы, наследники `AtmException`. CLI ловит их одним
  `catch (const AtmException&)` и печатает понятное сообщение.
* **Никаких магических чисел.** Константы вынесены в `static constexpr`, например `PinCode::PIN_LENGTH = 4`,
  `CashDispenser::DEFAULT_MAX_NOTES = 40` или `Atm::FRAUD_THRESHOLD = 3000.0`.

## Интересные алгоритмы

* **Проверка номера карты по алгоритму Луна** (`Card::isValidNumber`). Идём по цифрам справа налево, каждую вторую
  удваиваем (если вышло больше 9 — вычитаем 9), всё складываем. Номер правильный, если сумма делится на 10.
* **Выдача суммы купюрами** (`CashDispenser::planNotes`). Жадный алгоритм: кассеты отсортированы от крупных купюр к мелким,
  из каждой берём столько купюр, сколько влезает в остаток суммы. Если остаток не обнулился — `CashShortageException`.
* **Антифрод** (`FraudDetector`). Операция подозрительна, если сумма больше порога или если последние операции вместе
  с этой больше двух порогов.
* **Пеня по коммунальному счёту** (`UtilityBill::penalty`) — 0,1% от суммы за каждый день просрочки.

## Исключения (18)

Все объявлены в `include/exceptions/AtmException.h`. Базовый класс хранит поле `_message` и отдаёт его через `what()`.

| Исключение | Когда бросается (примеры) |
|---|---|
| `AtmException` | базовый класс, напрямую не бросается |
| `InvalidCardException` | номер карты не прошёл проверку Луна, нет владельца или счёта, карта уже выпущена, не найдена |
| `CardBlockedException` | вставлена заблокированная карта (банкомат её изымает) |
| `CardExpiredException` | срок действия карты истёк |
| `InvalidPinException` | неверный PIN (сообщает, сколько попыток осталось), PIN не из 4 цифр |
| `PinLockedException` | третья неверная попытка — PIN и карта блокируются, карта изымается |
| `InsufficientFundsException` | на счёте или карте не хватает денег, превышен кредитный лимит |
| `LimitExceededException` | превышен дневной лимит снятия, число операций за день, число купюр за раз |
| `InvalidAmountException` | сумма ≤ 0, отрицательные тарифы, ставки, лимиты |
| `CashShortageException` | в банкомате нет купюр, чтобы выдать сумму (например, 15 BYN) |
| `AccountNotFoundException` | счёт не найден, неверный лицевой счёт у поставщика услуг |
| `AccountFrozenException` | счёт заморожен — нельзя пополнять, снимать, переводить |
| `SessionException` | карта не вставлена, PIN не введён, время сеанса вышло, банкомат уже занят |
| `DeviceException` | устройство неисправно, нет бумаги, лоток переполнен, кассета уже стоит |
| `InvalidBanknoteException` | купюра не принята: фальшивая, повреждённая, неизвестный номинал |
| `CurrencyException` | неизвестная валюта, курс ≤ 0, перевод между счетами в разных валютах без банка |
| `FraudDetectedException` | антифрод заблокировал подозрительную операцию |
| `AccessDeniedException` | неверный служебный код банкомата или код доступа сотрудника, сотрудник не на смене |
| `TransactionException` | операция уже завершена, не найдена, счёт уже оплачен |

## Сводная таблица классов

Формат как в примере задания: класс, число полей, число методов-поведений (без геттеров и конструкторов), от кого наследуется, с какими классами связан.

| Класс | Группа | Поля | Методы-поведения | Наследует | Ассоциации (связанные классы) |
|---|---|---|---|---|---|
| AccountStatement | accounts | 4 | 4 | — | StatementEntry |
| Address | bank | 4 | 3 | — | — |
| Atm | bank | 18 | 19 | — | Address, AtmDevice, Bank, Banknote, Card, CardReader, CashCassette, CashDispenser, DepositModule, Display, FeePolicy, Keypad, ProcessingCenter, ReceiptPrinter, SecurityCamera, ServiceProvider, Session, SmsNotifier, StatementEntry, Transaction, TransactionLog, UtilityBill |
| AtmDevice | hardware | 4 | 8 | — | — |
| AtmNetwork | bank | 3 | 6 | — | Atm |
| BalanceInquiry | operations | 2 | 3 | Transaction | — |
| Bank | bank | 7 | 12 | — | BankAccount, Card, CardHolder, CurrencyExchange |
| BankAccount | accounts | 6 | 8 | — | AccountStatement, Currency |
| Banknote | hardware | 4 | 3 | — | — |
| BanknoteValidator | hardware | 3 | 4 | — | Banknote |
| Card | cards | 8 | 14 | — | BankAccount, CardLimit, PinCode |
| CardHolder | cards | 5 | 7 | — | Card |
| CardLimit | cards | 4 | 5 | — | — |
| CardReader | hardware | 2 | 7 | AtmDevice | Card |
| CashCassette | hardware | 4 | 7 | — | — |
| CashCollector | bank | 3 | 4 | Employee | Atm, CashDispenser, DepositModule, EncashmentReport |
| CashDispenser | hardware | 3 | 10 | AtmDevice | CashCassette |
| CashbackProgram | cards | 4 | 4 | — | — |
| CreditCard | cards | 3 | 7 | Card | BankAccount |
| Currency | accounts | 3 | 5 | — | — |
| CurrencyExchange | accounts | 3 | 5 | — | Currency |
| DebitCard | cards | 2 | 3 | Card | BankAccount, CashbackProgram |
| Deposit | operations | 2 | 3 | Transaction | — |
| DepositModule | hardware | 4 | 6 | AtmDevice | Banknote, BanknoteValidator |
| Display | hardware | 3 | 6 | AtmDevice | — |
| Employee | bank | 4 | 6 | — | — |
| EncashmentReport | bank | 5 | 4 | — | — |
| FeePolicy | operations | 4 | 4 | — | ServiceProvider |
| FraudDetector | bank | 4 | 5 | — | — |
| Keypad | hardware | 3 | 7 | AtmDevice | — |
| MaintenanceLog | bank | 4 | 4 | — | MaintenanceRecord |
| MaintenanceRecord | bank | 5 | 3 | — | — |
| PinCode | cards | 3 | 7 | — | — |
| PrepaidCard | cards | 2 | 3 | Card | BankAccount |
| ProcessingCenter | bank | 4 | 4 | — | Bank, Card, FraudDetector |
| Receipt | operations | 4 | 4 | — | Transaction |
| ReceiptPrinter | hardware | 3 | 5 | AtmDevice | Receipt |
| SavingsAccount | accounts | 2 | 4 | BankAccount | Currency |
| SecurityCamera | hardware | 4 | 5 | AtmDevice | — |
| ServicePayment | operations | 2 | 3 | Transaction | ServiceProvider |
| ServiceProvider | operations | 4 | 3 | — | — |
| Session | operations | 6 | 7 | — | Card |
| SmsNotifier | bank | 3 | 5 | — | CardHolder, Transaction |
| StatementEntry | accounts | 4 | 3 | — | — |
| Technician | bank | 3 | 4 | Employee | Atm, AtmDevice, MaintenanceLog |
| Transaction | operations | 6 | 7 | — | — |
| TransactionLog | operations | 2 | 8 | — | Transaction |
| Transfer | operations | 2 | 3 | Transaction | — |
| UtilityBill | operations | 5 | 4 | — | ServiceProvider |
| Withdrawal | operations | 2 | 5 | Transaction | — |

**Итого:** 50 классов, 198 полей, 280 методов-поведений, 65 ассоциаций.

## Подробное описание классов

### Счета и валюты (accounts)

#### AccountStatement

Выписка по счёту: хранит последние операции (старые удаляются при переполнении).

- **Поля (4):** `_accountNumber` (std::string), `_entries` (std::vector<StatementEntry>), `_maxEntries` (int), `_totalAdded` (int)
- **Ассоциации:** StatementEntry
- **Поведения (4):**
  - `addEntry()` — Добавить строку. Если места нет — самая старая строка удаляется.
  - `lastEntries()` — Последние count строк (мини-выписка в банкомате).
  - `totalIncome()` — Сумма всех поступлений в выписке.
  - `totalExpense()` — Сумма всех списаний в выписке (положительное число).

#### BankAccount

Банковский (текущий) счёт клиента: баланс, валюта, выписка, заморозка.

- **Поля (6):** `_number` (std::string), `_ownerName` (std::string), `_currency` (Currency), `_balance` (double), `_isFrozen` (bool), `_statement` (AccountStatement)
- **Ассоциации:** AccountStatement, Currency
- **Поведения (8):**
  - `type()` — Тип счёта словами.
  - `availableFunds()` — Сколько денег можно снять (у сберегательного счёта есть неснижаемый остаток).
  - `canWithdraw()` — Хватит ли денег, чтобы снять сумму.
  - `deposit()` — Зачислить деньги. Бросает InvalidAmountException или AccountFrozenException.
  - `withdraw()` — Списать деньги. Бросает InvalidAmountException, AccountFrozenException, InsufficientFundsException.
  - `transferTo()` — Перевести деньги на другой счёт той же валюты. Бросает CurrencyException, если валюты разные.
  - `freeze()` — Заморозить счёт (операции запрещены).
  - `unfreeze()` — Разморозить счёт.

#### Currency

Валюта: код (BYN, USD), символ и курс к белорусскому рублю.

- **Поля (3):** `_code` (std::string), `_symbol` (std::string), `_rateToByn` (double)
- **Ассоциации:** нет
- **Поведения (5):**
  - `toByn()` — Перевести сумму в этой валюте в белорусские рубли.
  - `fromByn()` — Перевести сумму в рублях в эту валюту.
  - `format()` — Сумма с символом валюты, например «12.50 $».
  - `updateRate()` — Новый курс. Бросает CurrencyException, если курс <= 0.
  - `isSame()` — Та же ли это валюта (сравнение по коду).

#### CurrencyExchange

Обмен валют: таблица курсов и комиссия банка за конвертацию.

- **Поля (3):** `_currencies` (std::map<std::string, Currency>), `_commissionPercent` (double), `_operationsCount` (int)
- **Ассоциации:** Currency
- **Поведения (5):**
  - `addCurrency()` — Добавить валюту в таблицу (или обновить её курс).
  - `supports()` — Есть ли такая валюта в таблице.
  - `findCurrency()` — Найти валюту по коду. Бросает CurrencyException, если её нет.
  - `commissionFor()` — Комиссия за обмен суммы (в исходной валюте).
  - `convert()` — Обменять сумму из одной валюты в другую с учётом комиссии.

#### SavingsAccount : BankAccount

Сберегательный счёт (вклад): проценты и неснижаемый остаток, который нельзя снять.

- **Поля (2):** `_yearRatePercent` (double), `_minBalance` (double)
- **Ассоциации:** Currency
- **Поведения (4):**
  - `type()` — своя реализация виртуального метода базового класса
  - `availableFunds()` — Снять можно всё, кроме неснижаемого остатка.
  - `interestForMonths()` — Проценты, которые набегут за заданное число месяцев (без капитализации).
  - `accrueMonthlyInterest()` — Начислить проценты за месяц. Возвращает начисленную сумму.

#### StatementEntry

Одна строка выписки по счёту: номер операции, описание, сумма, остаток после неё.

- **Поля (4):** `_number` (int), `_description` (std::string), `_amount` (double), `_balanceAfter` (double)
- **Ассоциации:** нет
- **Поведения (3):**
  - `isIncome()` — Поступление ли это (сумма > 0).
  - `isExpense()` — Списание ли это (сумма < 0).
  - `format()` — Строка для печати: «#3 Снятие наличных: -100.00 (остаток 400.00)».

### Карты и клиенты (cards)

#### Card

Абстрактная банковская карта: номер, владелец, срок действия, PIN, лимиты и привязанный счёт.

- **Поля (8):** `_number` (std::string), `_holderName` (std::string), `_expiryMonth` (int), `_expiryYear` (int), `_isBlocked` (bool), `_pin` (PinCode), `_limit` (CardLimit), `_account` (std::shared_ptr<BankAccount>)
- **Ассоциации:** BankAccount, CardLimit, PinCode
- **Поведения (14):**
  - `type()` — Тип карты словами (дебетовая, кредитная, предоплаченная).
  - `availableFunds()` — Сколько денег доступно по карте.
  - `charge()` — Списать деньги с карты.
  - `isValidNumber()` — Проверка номера карты по алгоритму Луна (16 цифр, контрольная сумма делится на 10).
  - `isExpired()` — Истёк ли срок действия на заданную дату (месяц, год).
  - `validate()` — Проверить, что картой можно пользоваться. Бросает CardBlockedException или CardExpiredException.
  - `authenticate()` — Проверить PIN. После последней неверной попытки карта блокируется.
  - `changePin()` — Сменить PIN-код.
  - `registerCashWithdrawal()` — Учесть снятие наличных в дневном лимите. Бросает LimitExceededException.
  - `topUp()` — Зачислить деньги на счёт карты.
  - `maskedNumber()` — Номер со звёздочками: «**** **** **** 1234».
  - `block()` — Заблокировать карту.
  - `unblock()` — Разблокировать карту (и сбросить ошибки PIN).
  - `startNewDay()` — Новый день: обнулить дневные лимиты.

#### CardHolder

Держатель карт — клиент банка: ФИО, паспорт, телефон и его карты.

- **Поля (5):** `_fullName` (std::string), `_passportNumber` (std::string), `_phone` (std::string), `_cards` (std::vector<std::shared_ptr<Card>>), `_isVip` (bool)
- **Ассоциации:** Card
- **Поведения (7):**
  - `addCard()` — Выдать клиенту карту. Бросает InvalidCardException, если карта пустая или уже есть.
  - `removeCard()` — Закрыть карту по номеру. Бросает InvalidCardException, если такой карты нет.
  - `findCard()` — Найти карту по номеру. Бросает InvalidCardException, если такой карты нет.
  - `totalAvailable()` — Сколько всего денег доступно по всем незаблокированным картам.
  - `activeCardsCount()` — Сколько карт не заблокировано.
  - `hasValidPhone()` — Правильный ли номер телефона (для SMS).
  - `upgradeToVip()` — Сделать клиента VIP, если у него достаточно денег на картах.

#### CardLimit

Дневные лимиты карты: сколько денег можно снять и сколько операций сделать за день.

- **Поля (4):** `_dailyLimit` (double), `_withdrawnToday` (double), `_maxOperationsPerDay` (int), `_operationsToday` (int)
- **Ассоциации:** нет
- **Поведения (5):**
  - `remaining()` — Сколько ещё можно снять сегодня.
  - `canWithdraw()` — Можно ли снять сумму, не нарушив лимиты.
  - `registerWithdrawal()` — Учесть снятие. Бросает LimitExceededException, если лимит превышен.
  - `resetDay()` — Новый день — счётчики обнуляются.
  - `changeLimit()` — Изменить дневной лимит. Бросает InvalidAmountException, если лимит <= 0.

#### CashbackProgram

Программа кэшбэка: базовый процент, повышенный процент в избранных категориях, потолок за месяц.

- **Поля (4):** `_basePercent` (double), `_bonusPercent` (double), `_monthlyCap` (double), `_bonusCategories` (std::set<std::string>)
- **Ассоциации:** нет
- **Поведения (4):**
  - `addBonusCategory()` — Добавить категорию с повышенным кэшбэком (например, «Кафе»).
  - `isBonusCategory()` — Входит ли категория в повышенный кэшбэк.
  - `percentFor()` — Процент кэшбэка для категории.
  - `calculate()` — Кэшбэк за покупку с учётом того, сколько уже начислено в этом месяце (не больше потолка).

#### CreditCard : Card

Кредитная карта: сначала тратятся свои деньги, потом кредитные (появляется долг).

- **Поля (3):** `_creditLimit` (double), `_debt` (double), `_monthlyRatePercent` (double)
- **Ассоциации:** BankAccount
- **Поведения (7):**
  - `type()` — своя реализация виртуального метода базового класса
  - `availableFunds()` — Свои деньги плюс неиспользованный кредит.
  - `charge()` — Списать сначала свои деньги, остаток — в долг. Бросает InsufficientFundsException.
  - `repayDebt()` — Погасить часть долга деньгами со счёта.
  - `monthlyInterest()` — Проценты за месяц на текущий долг.
  - `accrueInterest()` — Начислить проценты за месяц (долг растёт).
  - `minimumPayment()` — Минимальный обязательный платёж за месяц.

#### DebitCard : Card

Дебетовая карта: тратятся только свои деньги, за покупки начисляется кэшбэк.

- **Поля (2):** `_cashbackProgram` (CashbackProgram), `_cashbackEarned` (double)
- **Ассоциации:** BankAccount, CashbackProgram
- **Поведения (3):**
  - `type()` — своя реализация виртуального метода базового класса
  - `earnCashback()` — Начислить кэшбэк за покупку. Возвращает начисленную сумму.
  - `transferCashbackToAccount()` — Перевести накопленный кэшбэк на счёт карты. Возвращает переведённую сумму.

#### PinCode

PIN-код карты. Хранится не сам PIN, а его хеш. Считает неудачные попытки ввода.

- **Поля (3):** `_hash` (std::size_t), `_failedAttempts` (int), `_maxAttempts` (int)
- **Ассоциации:** нет
- **Поведения (7):**
  - `isValidFormat()` — Правильный ли формат PIN: ровно 4 цифры.
  - `matches()` — Совпадает ли PIN (без подсчёта попыток).
  - `verify()` — Проверить PIN. При ошибке бросает InvalidPinException, после последней попытки — PinLockedException.
  - `isLocked()` — Заблокирован ли PIN (попытки закончились).
  - `attemptsLeft()` — Сколько попыток осталось.
  - `change()` — Сменить PIN. Старый PIN должен быть верным, новый — правильного формата и отличаться от старого.
  - `reset()` — Сбросить счётчик ошибок (делает банк после проверки личности клиента).

#### PrepaidCard : Card

Предоплаченная (подарочная) карта: тратится только внесённая сумма, пополнять можно не всегда.

- **Поля (2):** `_isRechargeable` (bool), `_rechargeCount` (int)
- **Ассоциации:** BankAccount
- **Поведения (3):**
  - `type()` — своя реализация виртуального метода базового класса
  - `recharge()` — Пополнить карту. Бросает InvalidCardException, если карта не пополняемая.
  - `isEmpty()` — Закончились ли деньги на карте.

### Устройства банкомата (hardware)

#### AtmDevice

Абстрактное устройство банкомата: серийный номер, исправность, счётчик ошибок.

- **Поля (4):** `_serialNumber` (std::string), `_isWorking` (bool), `_errorCount` (int), `_lastError` (std::string)
- **Ассоциации:** нет
- **Поведения (8):**
  - `deviceName()` — Название устройства («Картоприёмник», «Принтер чеков»...).
  - `selfTest()` — Самопроверка устройства. По умолчанию — исправно ли оно.
  - `reportError()` — Зафиксировать неисправность: устройство перестаёт работать.
  - `repair()` — Починить устройство.
  - `service()` — Обслуживание техником. По умолчанию — просто ремонт; принтер ещё и заправляет бумагу.
  - `ensureWorking()` — Бросает DeviceException, если устройство неисправно.
  - `needsReplacement()` — Пора ли заменить устройство (слишком много поломок).
  - `statusLine()` — Строка состояния: «Принтер чеков (ATM1-PR): исправно».

#### Banknote

Купюра: номинал, валюта, серийный номер, повреждена ли.

- **Поля (4):** `_denomination` (int), `_currencyCode` (std::string), `_serialNumber` (std::string), `_isDamaged` (bool)
- **Ассоциации:** нет
- **Поведения (3):**
  - `isValidDenomination()` — Существует ли такой номинал (5, 10, 20, 50, 100, 200, 500).
  - `hasValidSerial()` — Правильный ли серийный номер (защита от подделок).
  - `isAcceptable()` — Можно ли принять купюру в банкомат, работающий с заданной валютой.

#### BanknoteValidator

Детектор купюр: проверяет подлинность, номинал, валюту и состояние купюры.

- **Поля (3):** `_currencyCode` (std::string), `_acceptedCount` (int), `_rejectedCount` (int)
- **Ассоциации:** Banknote
- **Поведения (4):**
  - `rejectionReason()` — Почему купюра не принимается (пустая строка — купюра в порядке).
  - `inspect()` — Проверить купюру и учесть результат. Возвращает true, если купюра принята.
  - `check()` — Проверить купюру. Бросает InvalidBanknoteException с причиной отказа.
  - `rejectionRate()` — Какой процент купюр отклонён.

#### CardReader : AtmDevice

Картоприёмник: принимает карту, возвращает её или изымает (задерживает).

- **Поля (2):** `_insertedCard` (std::shared_ptr<Card>), `_capturedCards` (std::vector<std::shared_ptr<Card>>)
- **Ассоциации:** Card
- **Поведения (7):**
  - `deviceName()` — своя реализация виртуального метода базового класса
  - `insertCard()` — Принять карту. Бросает DeviceException, если карта уже внутри или устройство неисправно.
  - `ejectCard()` — Вернуть карту клиенту. Бросает DeviceException, если карты нет.
  - `captureCard()` — Изъять карту (заблокированную или забытую). Бросает DeviceException, если карты нет.
  - `hasCard()` — Есть ли карта в картоприёмнике.
  - `capturedCount()` — Сколько карт изъято.
  - `releaseCapturedCards()` — Отдать изъятые карты сотруднику банка (список очищается).

#### CashCassette

Кассета с купюрами одного номинала внутри диспенсера.

- **Поля (4):** `_denomination` (int), `_count` (int), `_capacity` (int), `_currencyCode` (std::string)
- **Ассоциации:** нет
- **Поведения (7):**
  - `take()` — Достать купюры. Бросает CashShortageException, если купюр не хватает.
  - `load()` — Загрузить купюры. Бросает DeviceException, если не поместятся.
  - `fillToCapacity()` — Заполнить кассету доверху. Возвращает, сколько купюр добавлено.
  - `totalValue()` — Сумма денег в кассете.
  - `isEmpty()` — Пуста ли кассета.
  - `isLow()` — Почти пуста ли кассета (пора инкассировать).
  - `fillPercent()` — Заполненность в процентах.

#### CashDispenser : AtmDevice

Диспенсер: набор кассет и выдача наличных крупными купюрами.

- **Поля (3):** `_cassettes` (std::vector<CashCassette>), `_maxNotesPerWithdrawal` (int), `_dispensedTotal` (int)
- **Ассоциации:** CashCassette
- **Поведения (10):**
  - `deviceName()` — своя реализация виртуального метода базового класса
  - `addCassette()` — Установить кассету. Бросает DeviceException, если кассета такого номинала уже стоит.
  - `totalCash()` — Сколько всего денег в банкомате.
  - `planNotes()` — Разложить сумму на купюры: сначала крупные. Бросает CashShortageException или LimitExceededException.
  - `canDispense()` — Можно ли выдать сумму имеющимися купюрами.
  - `dispense()` — Выдать сумму: купюры забираются из кассет. Возвращает, какие купюры выданы.
  - `loadNotes()` — Загрузить купюры в кассету нужного номинала. Бросает DeviceException, если такой кассеты нет.
  - `refillAll()` — Заполнить все кассеты доверху. Возвращает загруженную сумму.
  - `lowCassettesCount()` — Сколько кассет почти пусты.
  - `minDenomination()` — Самый мелкий номинал, который сейчас есть (0, если денег нет).

#### DepositModule : AtmDevice

Купюроприёмник: проверяет внесённые купюры детектором и складывает их в лоток.

- **Поля (4):** `_validator` (BanknoteValidator), `_storedNotes` (std::vector<Banknote>), `_capacity` (int), `_lastRejectedCount` (int)
- **Ассоциации:** Banknote, BanknoteValidator
- **Поведения (6):**
  - `deviceName()` — своя реализация виртуального метода базового класса
  - `acceptNotes()` — Принять пачку купюр. Плохие купюры возвращаются клиенту. Возвращает принятую сумму.
  - `freeSpace()` — Сколько ещё купюр поместится в лоток.
  - `isFull()` — Полон ли лоток.
  - `storedValue()` — Сумма денег в лотке.
  - `collectAll()` — Забрать все купюры из лотка (делает инкассатор).

#### Display : AtmDevice

Экран банкомата: показывает последние сообщения на выбранном языке.

- **Поля (3):** `_messages` (std::vector<std::string>), `_language` (std::string), `_maxLines` (int)
- **Ассоциации:** нет
- **Поведения (6):**
  - `deviceName()` — своя реализация виртуального метода базового класса
  - `show()` — Показать сообщение. Старые строки уходят за край экрана.
  - `clear()` — Очистить экран.
  - `lastMessage()` — Последнее сообщение (пустая строка, если экран чист).
  - `switchLanguage()` — Сменить язык: «RU» или «EN». Бросает DeviceException для других языков.
  - `welcomeText()` — Приветствие на текущем языке.

#### Keypad : AtmDevice

Цифровая клавиатура банкомата (PIN-пад). Набранное можно скрывать звёздочками.

- **Поля (3):** `_buffer` (std::string), `_maxLength` (int), `_isMasked` (bool)
- **Ассоциации:** нет
- **Поведения (7):**
  - `deviceName()` — своя реализация виртуального метода базового класса
  - `pressKey()` — Нажать клавишу. Принимаются только цифры, пока не набрана максимальная длина.
  - `typeText()` — Набрать строку целиком. Возвращает, сколько клавиш принято.
  - `backspace()` — Стереть последнюю цифру.
  - `clear()` — Стереть всё набранное.
  - `takeInput()` — Забрать набранное (буфер очищается).
  - `displayText()` — Что показать на экране: цифры или звёздочки.

#### ReceiptPrinter : AtmDevice

Принтер чеков: печатает чек, следит за запасом бумаги.

- **Поля (3):** `_paperLeft` (int), `_paperCapacity` (int), `_printedCount` (int)
- **Ассоциации:** Receipt
- **Поведения (5):**
  - `deviceName()` — своя реализация виртуального метода базового класса
  - `print()` — Напечатать чек. Если бумага кончилась — принтер ломается и бросает DeviceException.
  - `refillPaper()` — Заправить новый рулон бумаги (принтер снова работает).
  - `service()` — Обслуживание принтера — заправка бумаги.
  - `isPaperLow()` — Заканчивается ли бумага.

#### SecurityCamera : AtmDevice

Камера наблюдения: снимает подозрительные события (изъятие карты, мошенничество).

- **Поля (4):** `_isRecording` (bool), `_snapshots` (std::vector<std::string>), `_storageLimit` (int), `_totalSnapshots` (int)
- **Ассоциации:** нет
- **Поведения (5):**
  - `deviceName()` — своя реализация виртуального метода базового класса
  - `startRecording()` — Включить запись. Бросает DeviceException, если камера неисправна.
  - `stopRecording()` — Выключить запись.
  - `takeSnapshot()` — Сделать снимок события. Бросает DeviceException, если запись выключена. Старые снимки удаляются.
  - `clearSnapshots()` — Удалить все снимки. Возвращает, сколько удалено.

### Операции (operations)

#### BalanceInquiry : Transaction

Запрос баланса: сумма операции 0, запоминается показанный остаток.

- **Поля (2):** `_balance` (double), `_currencyCode` (std::string)
- **Ассоциации:** нет
- **Поведения (3):**
  - `type()` — своя реализация виртуального метода базового класса
  - `describe()` — «Запрос баланса: доступно 500.00 BYN».
  - `isEnoughFor()` — Хватает ли денег на сумму (подсказка клиенту перед снятием).

#### Deposit : Transaction

Внесение наличных: сколько купюр принято и сколько возвращено.

- **Поля (2):** `_acceptedNotes` (int), `_rejectedNotes` (int)
- **Ассоциации:** нет
- **Поведения (3):**
  - `type()` — своя реализация виртуального метода базового класса
  - `describe()` — Описание с числом принятых и возвращённых купюр.
  - `hasRejected()` — Были ли возвращённые (непринятые) купюры.

#### FeePolicy

Тарифы банкомата: комиссия за снятие (своя/чужая карта), за перевод и за оплату услуг.

- **Поля (4):** `_ownBankFee` (double), `_foreignPercent` (double), `_minForeignFee` (double), `_transferPercent` (double)
- **Ассоциации:** ServiceProvider
- **Поведения (4):**
  - `withdrawalFee()` — Комиссия за снятие: фиксированная для своих карт, процент (не меньше минимума) — для чужих.
  - `transferFee()` — Комиссия за перевод.
  - `paymentFee()` — Комиссия за оплату услуги — берёт поставщик.
  - `describe()` — Тарифы текстом для экрана.

#### Receipt

Чек банкомата: шапка (банкомат, номер операции, карта) и строки с подробностями.

- **Поля (4):** `_atmId` (std::string), `_transactionId` (int), `_cardMask` (std::string), `_lines` (std::vector<std::string>)
- **Ассоциации:** Transaction
- **Поведения (4):**
  - `addLine()` — Добавить строку в чек.
  - `addTransaction()` — Добавить в чек описание операции и её статус.
  - `lineCount()` — Сколько строк в теле чека.
  - `text()` — Готовый текст чека для печати.

#### ServicePayment : Transaction

Оплата услуги: кому платим и за какой лицевой счёт (номер телефона, договор).

- **Поля (2):** `_provider` (ServiceProvider), `_payerAccount` (std::string)
- **Ассоциации:** ServiceProvider
- **Поведения (3):**
  - `type()` — своя реализация виртуального метода базового класса
  - `describe()` — Описание с поставщиком и лицевым счётом.
  - `isCategory()` — Относится ли платёж к категории услуг (например, «Мобильная связь»).

#### ServiceProvider

Поставщик услуг, которые можно оплатить в банкомате (мобильная связь, ЖКХ, интернет).

- **Поля (4):** `_name` (std::string), `_category` (std::string), `_accountLength` (int), `_commissionPercent` (double)
- **Ассоциации:** нет
- **Поведения (3):**
  - `isValidAccount()` — Правильный ли лицевой счёт (телефон): только цифры нужной длины.
  - `commissionFor()` — Комиссия за платёж.
  - `label()` — Подпись: «МТС (Мобильная связь)».

#### Session

Сеанс работы клиента с банкоматом: от вставки карты до её возврата.

- **Поля (6):** `_card` (std::shared_ptr<Card>), `_isAuthenticated` (bool), `_secondsIdle` (int), `_timeoutSeconds` (int), `_operationsCount` (int), `_isClosed` (bool)
- **Ассоциации:** Card
- **Поведения (7):**
  - `authenticate()` — Ввести PIN. При успехе сеанс становится авторизованным.
  - `tick()` — Прошло время бездействия.
  - `isExpired()` — Истекло ли время сеанса.
  - `isActive()` — Можно ли ещё работать (сеанс не закрыт и не истёк).
  - `requireAuthenticated()` — Бросает SessionException, если сеанс не активен или PIN не введён.
  - `registerOperation()` — Учесть выполненную операцию (таймер бездействия обнуляется).
  - `close()` — Закрыть сеанс.

#### Transaction

Абстрактная операция в банкомате: номер, карта, сумма, комиссия, статус.

- **Поля (6):** `_id` (int), `_cardMask` (std::string), `_amount` (double), `_fee` (double), `_status` (TransactionStatus), `_failReason` (std::string)
- **Ассоциации:** нет
- **Поведения (7):**
  - `type()` — Вид операции словами («Снятие наличных», «Перевод»...).
  - `describe()` — Описание для экрана и чека.
  - `total()` — Сумма вместе с комиссией.
  - `complete()` — Операция прошла успешно. Бросает TransactionException, если она уже завершена.
  - `fail()` — Операция не удалась. Бросает TransactionException, если она уже завершена.
  - `isSuccessful()` — Успешна ли операция.
  - `statusName()` — Статус словами: «В обработке», «Успешно», «Отклонено».

#### TransactionLog

Журнал операций банкомата: выдаёт номера операций и хранит все операции.

- **Поля (2):** `_transactions` (std::vector<std::shared_ptr<Transaction>>), `_nextId` (int)
- **Ассоциации:** Transaction
- **Поведения (8):**
  - `nextId()` — Новый уникальный номер операции.
  - `record()` — Записать операцию в журнал. Бросает TransactionException, если операция пустая.
  - `findById()` — Найти операцию по номеру. Бросает TransactionException, если её нет.
  - `lastTransaction()` — Последняя операция. Бросает TransactionException, если журнал пуст.
  - `totalWithdrawn()` — Сколько наличных выдано успешными снятиями.
  - `countByType()` — Сколько операций такого вида.
  - `failedCount()` — Сколько операций отклонено.
  - `forCard()` — Все операции по карте.

#### Transfer : Transaction

Перевод с карты на счёт: номер счёта и имя получателя.

- **Поля (2):** `_targetAccount` (std::string), `_targetOwner` (std::string)
- **Ассоциации:** нет
- **Поведения (3):**
  - `type()` — своя реализация виртуального метода базового класса
  - `describe()` — Описание с получателем.
  - `isToOwner()` — Переводит ли клиент деньги сам себе.

#### UtilityBill

Счёт за коммунальные услуги: сколько платить, до какого дня, пеня за просрочку.

- **Поля (5):** `_provider` (ServiceProvider), `_accountNumber` (std::string), `_amountDue` (double), `_dueDay` (int), `_isPaid` (bool)
- **Ассоциации:** ServiceProvider
- **Поведения (4):**
  - `isOverdue()` — Просрочен ли счёт на заданный день.
  - `penalty()` — Пеня за просрочку на заданный день.
  - `totalToPay()` — Сколько всего платить на заданный день (сумма + пеня).
  - `markPaid()` — Отметить оплату. Бросает TransactionException (уже оплачен) или InvalidAmountException (мало денег).

#### Withdrawal : Transaction

Снятие наличных: какие купюры выданы, чужая ли карта.

- **Поля (2):** `_notes` (std::map<int, int>), `_isForeignCard` (bool)
- **Ассоциации:** нет
- **Поведения (5):**
  - `type()` — своя реализация виртуального метода базового класса
  - `describe()` — Описание с перечнем купюр.
  - `recordNotes()` — Записать, какие купюры выданы (номинал -> количество).
  - `notesCount()` — Сколько всего купюр выдано.
  - `notesDescription()` — Купюры словами: «2 x 100, 1 x 50».

### Банк, банкомат и сотрудники (bank)

#### Address

Адрес банкомата: город, улица, дом и место установки (например, «ТЦ Galleria, 1 этаж»).

- **Поля (4):** `_city` (std::string), `_street` (std::string), `_building` (std::string), `_place` (std::string)
- **Ассоциации:** нет
- **Поведения (3):**
  - `format()` — Адрес одной строкой: «г. Минск, ул. Ленина, 5 (ТЦ Galleria)».
  - `isSameCity()` — В одном ли городе два адреса.
  - `isIndoor()` — Стоит ли банкомат внутри помещения (указано место установки).

#### Atm

Банкомат: собирает устройства, сеанс клиента, журнал операций и связь с банком.

- **Поля (18):** `_id` (std::string), `_address` (Address), `_bank` (std::shared_ptr<Bank>), `_processing` (ProcessingCenter), `_feePolicy` (FeePolicy), `_cardReader` (CardReader), `_keypad` (Keypad), `_display` (Display), `_dispenser` (CashDispenser), `_depositModule` (DepositModule), `_printer` (ReceiptPrinter), `_camera` (SecurityCamera), `_log` (TransactionLog), `_notifier` (SmsNotifier), `_session` (std::unique_ptr<Session>), `_serviceCode` (std::string), `_month` (int), `_year` (int)
- **Ассоциации:** Address, AtmDevice, Bank, Banknote, Card, CardReader, CashCassette, CashDispenser, DepositModule, Display, FeePolicy, Keypad, ProcessingCenter, ReceiptPrinter, SecurityCamera, ServiceProvider, Session, SmsNotifier, StatementEntry, Transaction, TransactionLog, UtilityBill
- **Поведения (19):**
  - `setDate()` — Установить текущую дату (нужна для проверки срока карты). Бросает DeviceException.
  - `installCassette()` — Установить кассету с купюрами.
  - `insertCard()` — Вставить карту. Заблокированная карта изымается, просроченная — возвращается.
  - `enterPin()` — Ввести PIN через клавиатуру. После последней неверной попытки карта изымается.
  - `checkBalance()` — Узнать доступный остаток.
  - `withdraw()` — Снять наличные. Возвращает выданные купюры.
  - `deposit()` — Внести наличные. Возвращает принятую сумму (плохие купюры возвращаются).
  - `transfer()` — Перевести деньги с карты на счёт.
  - `payService()` — Оплатить услугу (мобильная связь, интернет...).
  - `payUtilityBill()` — Оплатить коммунальный счёт вместе с пеней на заданный день.
  - `changePin()` — Сменить PIN-код.
  - `miniStatement()` — Мини-выписка: последние операции по счёту карты.
  - `printReceipt()` — Напечатать чек по последней операции.
  - `ejectCard()` — Вернуть карту и закончить сеанс.
  - `tick()` — Прошло время. Если клиент бездействует слишком долго — карта изымается. Возвращает true, если изъята.
  - `enterServiceMode()` — Войти в служебный режим. Бросает AccessDeniedException или SessionException (идёт сеанс клиента).
  - `devices()` — Все устройства банкомата.
  - `isOperational()` — Может ли банкомат работать: все устройства исправны и есть наличные.
  - `statusReport()` — Отчёт о состоянии банкомата.

#### AtmNetwork

Сеть банкоматов банка: поиск по городу, контроль наличных, сколько банкоматов работает.

- **Поля (3):** `_bankName` (std::string), `_atms` (std::vector<std::shared_ptr<Atm>>), `_minCashLevel` (int)
- **Ассоциации:** Atm
- **Поведения (6):**
  - `addAtm()` — Добавить банкомат. Бросает DeviceException, если номер уже занят.
  - `findById()` — Найти банкомат по номеру. Бросает DeviceException.
  - `atmsInCity()` — Банкоматы в городе.
  - `needingEncashment()` — Банкоматы, где наличных меньше минимума (нужна инкассация).
  - `totalCash()` — Сколько наличных во всей сети.
  - `operationalCount()` — Сколько банкоматов исправны и могут выдавать деньги.

#### Bank

Банк: счета, карты, клиенты и обмен валют. Банкоматы обращаются к нему за данными.

- **Поля (7):** `_name` (std::string), `_bic` (std::string), `_exchange` (CurrencyExchange), `_accounts` (std::map<std::string, std::shared_ptr<BankAccount>>), `_cards` (std::map<std::string, std::shared_ptr<Card>>), `_clients` (std::vector<std::shared_ptr<CardHolder>>), `_nextAccountNumber` (int)
- **Ассоциации:** BankAccount, Card, CardHolder, CurrencyExchange
- **Поведения (12):**
  - `openAccount()` — Открыть текущий счёт в валюте из таблицы обмена. Номер счёта присваивает банк.
  - `addAccount()` — Зарегистрировать готовый счёт (например, вклад). Бросает AccountNotFoundException, если номер занят.
  - `findAccount()` — Найти счёт. Бросает AccountNotFoundException.
  - `addClient()` — Добавить клиента (повторно тот же клиент не добавляется).
  - `issueCard()` — Выпустить карту клиенту. Счёт карты должен быть открыт в этом банке.
  - `findCard()` — Найти карту. Бросает InvalidCardException.
  - `isOwnCard()` — Выпущена ли карта этим банком (иначе — чужая карта, комиссия выше).
  - `blockCard()` — Заблокировать карту по номеру.
  - `findHolderOf()` — Найти владельца карты (nullptr, если не нашли).
  - `receiveTransfer()` — Зачислить перевод на счёт с конвертацией валюты. Возвращает зачисленную сумму.
  - `transfer()` — Перевод между двумя счетами банка. Возвращает зачисленную сумму.
  - `totalDepositsInByn()` — Все деньги клиентов в пересчёте на рубли.

#### CashCollector : Employee

Инкассатор: загружает наличные в банкомат и забирает деньги из купюроприёмника.

- **Поля (3):** `_loadedTotal` (double), `_collectedTotal` (double), `_visits` (int)
- **Ассоциации:** Atm, CashDispenser, DepositModule, EncashmentReport
- **Поведения (4):**
  - `role()` — своя реализация виртуального метода базового класса
  - `loadCassette()` — Загрузить купюры в кассету диспенсера. Возвращает загруженную сумму.
  - `collectDeposits()` — Забрать все купюры из купюроприёмника. Возвращает забранную сумму.
  - `encash()` — Полная инкассация банкомата: кассеты доверху, купюроприёмник пустой. Возвращает акт.

#### Employee

Абстрактный сотрудник банка, обслуживающий банкоматы: имя, табельный номер, код доступа, смена.

- **Поля (4):** `_name` (std::string), `_employeeId` (std::string), `_accessCode` (std::string), `_isOnDuty` (bool)
- **Ассоциации:** нет
- **Поведения (6):**
  - `role()` — Должность словами.
  - `verifyAccess()` — Проверить код доступа. Бросает AccessDeniedException.
  - `startShift()` — Начать смену (нужен код доступа).
  - `endShift()` — Закончить смену.
  - `requireOnDuty()` — Бросает AccessDeniedException, если сотрудник не на смене.
  - `badge()` — Бейдж: «Инкассатор Иван Петров (таб. № C-01)».

#### EncashmentReport

Акт инкассации: сколько денег загружено в банкомат и сколько забрано из купюроприёмника.

- **Поля (5):** `_atmId` (std::string), `_collectorName` (std::string), `_day` (int), `_loadedAmount` (double), `_collectedAmount` (double)
- **Ассоциации:** нет
- **Поведения (4):**
  - `addLoaded()` — Учесть загруженные деньги. Бросает InvalidAmountException, если сумма отрицательная.
  - `addCollected()` — Учесть забранные деньги. Бросает InvalidAmountException, если сумма отрицательная.
  - `netChange()` — Насколько изменилось количество денег у банка в банкомате (загружено минус забрано).
  - `summary()` — Акт одной строкой.

#### FraudDetector

Антифрод: ловит слишком крупные суммы и серии операций подряд на большую сумму.

- **Поля (4):** `_largeAmountThreshold` (double), `_windowSize` (int), `_recentAmounts` (std::deque<double>), `_alerts` (std::vector<std::string>)
- **Ассоциации:** нет
- **Поведения (5):**
  - `isSuspicious()` — Подозрительна ли сумма (сама по себе крупная или вместе с последними операциями).
  - `check()` — Проверить операцию. Бросает FraudDetectedException и записывает тревогу, если она подозрительна.
  - `record()` — Запомнить сумму операции (хранятся только последние windowSize сумм).
  - `recentTotal()` — Сумма последних операций.
  - `reset()` — Забыть последние операции (например, после проверки клиента службой безопасности).

#### MaintenanceLog

Журнал обслуживания банкомата: все ремонты и когда пора на плановый осмотр.

- **Поля (4):** `_atmId` (std::string), `_records` (std::vector<MaintenanceRecord>), `_serviceIntervalDays` (int), `_lastServiceDay` (int)
- **Ассоциации:** MaintenanceRecord
- **Поведения (4):**
  - `addRecord()` — Добавить запись (день последнего обслуживания обновляется).
  - `isServiceDue()` — Пора ли на плановое обслуживание.
  - `totalCost()` — Сколько всего потрачено на ремонты.
  - `recordsFor()` — Все записи по одному устройству.

#### MaintenanceRecord

Запись о ремонте или осмотре: день, устройство, что сделано, кто сделал, стоимость.

- **Поля (5):** `_day` (int), `_deviceName` (std::string), `_description` (std::string), `_technicianName` (std::string), `_cost` (double)
- **Ассоциации:** нет
- **Поведения (3):**
  - `summary()` — Запись одной строкой.
  - `isExpensive()` — Дорогой ли ремонт.
  - `daysSince()` — Сколько дней прошло с ремонта.

#### ProcessingCenter

Процессинговый центр: одобряет или отклоняет операции по картам.

- **Поля (4):** `_bank` (std::shared_ptr<Bank>), `_fraudDetector` (FraudDetector), `_approvedCount` (int), `_declinedCount` (int)
- **Ассоциации:** Bank, Card, FraudDetector
- **Поведения (4):**
  - `authorize()` — Одобрить списание: карта действует, операция не подозрительна, денег хватает. Иначе — исключение.
  - `isForeignCard()` — Чужая ли карта (выпущена другим банком).
  - `approvalRate()` — Процент одобренных операций.
  - `resetStatistics()` — Обнулить статистику одобрений.

#### SmsNotifier

SMS-оповещения клиенту об операциях по карте.

- **Поля (3):** `_sentMessages` (std::vector<std::string>), `_smsPrice` (double), `_isEnabled` (bool)
- **Ассоциации:** CardHolder, Transaction
- **Поведения (5):**
  - `notify()` — Отправить SMS об операции. Возвращает false, если оповещения выключены или телефон неверный.
  - `enable()` — Включить оповещения.
  - `disable()` — Выключить оповещения.
  - `totalCost()` — Сколько стоили все отправленные SMS.
  - `messagesTo()` — Сколько SMS отправлено на номер.

#### Technician : Employee

Техник: находит неисправные устройства банкомата и чинит их, записывая ремонт в журнал.

- **Поля (3):** `_specialization` (std::string), `_repairsDone` (int), `_callOutFee` (double)
- **Ассоциации:** Atm, AtmDevice, MaintenanceLog
- **Поведения (4):**
  - `role()` — своя реализация виртуального метода базового класса
  - `diagnose()` — Список неисправных устройств банкомата.
  - `repair()` — Починить устройство и записать ремонт в журнал. Бросает DeviceException, если устройство исправно.
  - `repairAll()` — Починить всё неисправное в банкомате (нужен служебный код). Возвращает число ремонтов.
