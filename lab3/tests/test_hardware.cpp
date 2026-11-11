#include "TestData.h"
#include "doctest.h"
#include "exceptions/AtmException.h"

using doctest::Approx;

TEST_CASE("AtmDevice: поломка, ремонт, состояние") {
    CHECK_THROWS_AS(CardReader(""), DeviceException);
    CardReader reader("CR-1");
    CHECK(reader.deviceName() == "Картоприёмник");
    CHECK(reader.selfTest());
    CHECK(reader.statusLine() == "Картоприёмник (CR-1): исправно");
    reader.reportError("застряла карта");
    CHECK_FALSE(reader.isWorking());
    CHECK(reader.getLastError() == "застряла карта");
    CHECK_THROWS_AS(reader.ensureWorking(), DeviceException);
    CHECK(reader.statusLine() == "Картоприёмник (CR-1): неисправно (застряла карта)");
    reader.service();
    CHECK(reader.isWorking());
    CHECK(reader.getLastError().empty());
    for (int index = 0; index < AtmDevice::MAX_ERRORS_BEFORE_REPLACEMENT; ++index) {
        reader.reportError("сбой");
    }
    CHECK(reader.needsReplacement());
    CHECK(reader.getErrorCount() == 6);
    CHECK(reader.getSerialNumber() == "CR-1");
}

TEST_CASE("CardReader: приём, возврат и изъятие карты") {
    CardReader reader("CR-1");
    auto card = testdata::debitCard(testdata::account());
    CHECK_THROWS_AS(reader.insertCard(nullptr), InvalidCardException);
    CHECK_THROWS_AS(reader.ejectCard(), DeviceException);
    reader.insertCard(card);
    CHECK(reader.hasCard());
    CHECK(reader.getInsertedCard() == card);
    CHECK_THROWS_AS(reader.insertCard(card), DeviceException);
    CHECK(reader.ejectCard() == card);
    CHECK_FALSE(reader.hasCard());
    reader.insertCard(card);
    reader.captureCard();
    CHECK(reader.capturedCount() == 1);
    CHECK(reader.getCapturedCards().size() == 1);
    CHECK(reader.releaseCapturedCards().size() == 1);
    CHECK(reader.capturedCount() == 0);
    reader.reportError("сбой");
    CHECK_THROWS_AS(reader.insertCard(card), DeviceException);
}

TEST_CASE("Keypad: набор цифр, маскировка") {
    Keypad keypad("KP-1", true);
    CHECK(keypad.deviceName() == "Клавиатура");
    CHECK(keypad.pressKey('1'));
    CHECK_FALSE(keypad.pressKey('a'));
    CHECK(keypad.typeText("2a34") == 3);
    CHECK(keypad.getBuffer() == "1234");
    CHECK(keypad.displayText() == "****");
    keypad.backspace();
    CHECK(keypad.takeInput() == "123");
    CHECK(keypad.getBuffer().empty());
    keypad.backspace();
    Keypad shortKeypad("KP-2", false, 3);
    CHECK_FALSE(shortKeypad.isMasked());
    CHECK(shortKeypad.typeText("12345") == 3);
    CHECK(shortKeypad.displayText() == "123");
    shortKeypad.clear();
    CHECK(shortKeypad.displayText().empty());
    shortKeypad.reportError("залипла кнопка");
    CHECK_THROWS_AS(shortKeypad.pressKey('1'), DeviceException);
}

TEST_CASE("Display: сообщения и язык") {
    Display display("DS-1", 2);
    CHECK(display.deviceName() == "Экран");
    CHECK(display.lastMessage().empty());
    display.show("один");
    display.show("два");
    display.show("три");
    CHECK(display.getMessages().size() == 2);
    CHECK(display.lastMessage() == "три");
    display.clear();
    CHECK(display.lastMessage().empty());
    CHECK(display.welcomeText() == "Добро пожаловать! Вставьте карту");
    CHECK_THROWS_AS(display.switchLanguage("DE"), DeviceException);
    display.switchLanguage("EN");
    CHECK(display.getLanguage() == "EN");
    CHECK(display.welcomeText() == "Welcome! Please insert your card");
    display.reportError("погас");
    CHECK_THROWS_AS(display.show("x"), DeviceException);
}

TEST_CASE("Banknote: номинал, серийный номер, пригодность") {
    CHECK(Banknote::isValidDenomination(100));
    CHECK_FALSE(Banknote::isValidDenomination(30));
    Banknote good(100, "BYN", "AB1234567");
    CHECK(good.hasValidSerial());
    CHECK(good.isAcceptable("BYN"));
    CHECK_FALSE(good.isAcceptable("USD"));
    CHECK_FALSE(Banknote(100, "BYN", "ab1234567").hasValidSerial());
    CHECK_FALSE(Banknote(100, "BYN", "AB123").hasValidSerial());
    CHECK_FALSE(Banknote(100, "BYN", "ABC234567").hasValidSerial());
    Banknote damaged(50, "BYN", "AB1234567", true);
    CHECK(damaged.isDamaged());
    CHECK_FALSE(damaged.isAcceptable("BYN"));
    CHECK(good.getDenomination() == 100);
    CHECK(good.getCurrencyCode() == "BYN");
    CHECK(good.getSerialNumber() == "AB1234567");
}

TEST_CASE("BanknoteValidator: причины отказа и статистика") {
    BanknoteValidator validator("BYN");
    CHECK(validator.rejectionRate() == Approx(0));
    CHECK(validator.rejectionReason(Banknote(100, "USD", "AB1234567")) == "чужая валюта USD");
    CHECK(validator.rejectionReason(Banknote(30, "BYN", "AB1234567")) == "неизвестный номинал 30");
    CHECK(validator.rejectionReason(Banknote(100, "BYN", "1")) == "подозрение на подделку");
    CHECK(validator.rejectionReason(Banknote(100, "BYN", "AB1234567", true)) == "купюра повреждена");
    CHECK(validator.rejectionReason(Banknote(100, "BYN", "AB1234567")).empty());
    CHECK(validator.inspect(Banknote(100, "BYN", "AB1234567")));
    CHECK_THROWS_AS(validator.check(Banknote(30, "BYN", "AB1234567")), InvalidBanknoteException);
    validator.check(Banknote(50, "BYN", "AB1234567"));
    CHECK(validator.getAcceptedCount() == 2);
    CHECK(validator.getRejectedCount() == 1);
    CHECK(validator.rejectionRate() == Approx(100.0 / 3));
    CHECK(validator.getCurrencyCode() == "BYN");
}

TEST_CASE("CashCassette: выдача, загрузка, заполненность") {
    CHECK_THROWS_AS(CashCassette(30, 1, 10, "BYN"), InvalidBanknoteException);
    CHECK_THROWS_AS(CashCassette(50, 1, 0, "BYN"), DeviceException);
    CHECK_THROWS_AS(CashCassette(50, 11, 10, "BYN"), DeviceException);
    CashCassette cassette(50, 10, 100, "BYN");
    CHECK(cassette.totalValue() == 500);
    CHECK(cassette.fillPercent() == Approx(10));
    CHECK(cassette.isLow());
    CHECK_THROWS_AS(cassette.take(0), InvalidAmountException);
    CHECK_THROWS_AS(cassette.take(11), CashShortageException);
    cassette.take(10);
    CHECK(cassette.isEmpty());
    CHECK_THROWS_AS(cassette.load(0), InvalidAmountException);
    CHECK_THROWS_AS(cassette.load(101), DeviceException);
    cassette.load(30);
    CHECK_FALSE(cassette.isLow());
    CHECK(cassette.fillToCapacity() == 70);
    CHECK(cassette.getCount() == 100);
    CHECK(cassette.getCapacity() == 100);
    CHECK(cassette.getDenomination() == 50);
    CHECK(cassette.getCurrencyCode() == "BYN");
}

TEST_CASE("CashDispenser: разбивка суммы на купюры и выдача") {
    CashDispenser dispenser("CD-1", 10);
    CHECK(dispenser.deviceName() == "Диспенсер наличных");
    CHECK(dispenser.minDenomination() == 0);
    dispenser.addCassette(CashCassette(50, 10, 100, "BYN"));
    dispenser.addCassette(CashCassette(100, 10, 100, "BYN"));
    dispenser.addCassette(CashCassette(20, 10, 100, "BYN"));
    dispenser.addCassette(CashCassette(10, 10, 100, "BYN"));
    CHECK_THROWS_AS(dispenser.addCassette(CashCassette(10, 1, 100, "BYN")), DeviceException);
    CHECK(dispenser.getCassettes().front().getDenomination() == 100);
    CHECK(dispenser.totalCash() == 1800);
    CHECK_THROWS_AS(dispenser.planNotes(0), InvalidAmountException);
    NotesPlan plan = dispenser.planNotes(380);
    CHECK(plan[100] == 3);
    CHECK(plan[50] == 1);
    CHECK(plan[20] == 1);
    CHECK(plan[10] == 1);
    CHECK_THROWS_AS(dispenser.planNotes(5), CashShortageException);
    CHECK_THROWS_AS(dispenser.planNotes(1380), LimitExceededException);
    CHECK(dispenser.canDispense(80));
    CHECK_FALSE(dispenser.canDispense(15));
    dispenser.dispense(380);
    CHECK(dispenser.totalCash() == 1420);
    CHECK(dispenser.getDispensedTotal() == 380);
    CHECK(dispenser.lowCassettesCount() == 4);
    CHECK(dispenser.minDenomination() == 10);
    CHECK_THROWS_AS(dispenser.loadNotes(200, 1), DeviceException);
    dispenser.loadNotes(10, 1);
    CHECK(dispenser.refillAll() == 9300 + 4550 + 1820 + 900);
    CHECK(dispenser.lowCassettesCount() == 0);
    dispenser.reportError("замятие");
    CHECK_THROWS_AS(dispenser.dispense(100), DeviceException);
}

TEST_CASE("DepositModule: приём купюр и инкассация") {
    DepositModule module("DM-1", BanknoteValidator("BYN"), 3);
    CHECK(module.deviceName() == "Купюроприёмник");
    CHECK_THROWS_AS(module.acceptNotes({}), InvalidAmountException);
    std::vector<Banknote> tooMany(4, Banknote(10, "BYN", "AB1234567"));
    CHECK_THROWS_AS(module.acceptNotes(tooMany), DeviceException);
    std::vector<Banknote> notes = {Banknote(100, "BYN", "AB1234567"), Banknote(30, "BYN", "AB1234567"),
                                   Banknote(50, "BYN", "CD7654321")};
    CHECK(module.acceptNotes(notes) == 150);
    CHECK(module.getLastRejectedCount() == 1);
    CHECK(module.storedValue() == 150);
    CHECK(module.freeSpace() == 1);
    CHECK_FALSE(module.isFull());
    module.acceptNotes({Banknote(20, "BYN", "AB1234567")});
    CHECK(module.isFull());
    CHECK(module.collectAll().size() == 3);
    CHECK(module.getStoredNotes().empty());
    CHECK(module.getCapacity() == 3);
    CHECK(module.getValidator().getRejectedCount() == 1);
    module.reportError("сбой");
    CHECK_THROWS_AS(module.acceptNotes(notes), DeviceException);
}

TEST_CASE("ReceiptPrinter: печать, бумага, обслуживание") {
    CHECK_THROWS_AS(ReceiptPrinter("PR-1", 0), DeviceException);
    ReceiptPrinter printer("PR-1", 2);
    CHECK(printer.deviceName() == "Принтер чеков");
    Receipt receipt("ATM", 1, "**** 0010");
    CHECK(printer.print(receipt).find("Операция №1") != std::string::npos);
    CHECK(printer.isPaperLow() == false);
    printer.print(receipt);
    CHECK(printer.isPaperLow());
    CHECK_THROWS_AS(printer.print(receipt), DeviceException);
    CHECK_FALSE(printer.isWorking());
    CHECK_THROWS_AS(printer.print(receipt), DeviceException);
    printer.service();
    CHECK(printer.isWorking());
    CHECK(printer.getPaperLeft() == 2);
    CHECK(printer.getPrintedCount() == 2);
    CHECK(printer.getPaperCapacity() == 2);
}

TEST_CASE("SecurityCamera: запись и снимки") {
    SecurityCamera camera("CAM-1", 2);
    CHECK(camera.deviceName() == "Камера наблюдения");
    CHECK_THROWS_AS(camera.takeSnapshot("x"), DeviceException);
    camera.startRecording();
    CHECK(camera.isRecording());
    CHECK(camera.takeSnapshot("Вход") == "Снимок #1: Вход");
    camera.takeSnapshot("Снятие");
    camera.takeSnapshot("Выход");
    CHECK(camera.getSnapshots().size() == 2);
    CHECK(camera.getSnapshots().front() == "Снимок #2: Снятие");
    CHECK(camera.clearSnapshots() == 2);
    camera.stopRecording();
    CHECK_FALSE(camera.isRecording());
    camera.reportError("объектив");
    CHECK_THROWS_AS(camera.startRecording(), DeviceException);
}
