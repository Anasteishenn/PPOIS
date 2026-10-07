/**
 * @file fifteen_main.cpp
 * @brief Консольное меню для игры «Пятнашки» (проверка класса Fifteen).
 */
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "ConsoleUtils.h"
#include "Fifteen.h"

namespace {

const int EXIT_CHOICE = 0;
const int CELL_WIDTH = 4;

using Action = void (*)(Fifteen&);

struct MenuItem {
    const char* title;
    Action action;
};

void printBoard(const Fifteen& puzzle) {
    std::cout << '\n';
    for (int row = 0; row < puzzle.size(); ++row) {
        for (int col = 0; col < puzzle.size(); ++col) {
            int value = puzzle.valueAt(row, col);
            if (value == Fifteen::EMPTY_CELL) {
                std::cout << std::setw(CELL_WIDTH) << '.';
            } else {
                std::cout << std::setw(CELL_WIDTH) << value;
            }
        }
        std::cout << '\n';
    }
}

void startNewGame(Fifteen& puzzle) {
    std::string prompt = "Размер поля (" + std::to_string(Fifteen::MIN_SIZE) + "-" +
                         std::to_string(Fifteen::MAX_SIZE) + "): ";
    int size = console::readInt(prompt);
    Fifteen newPuzzle(size);
    newPuzzle.randomize();
    puzzle = newPuzzle;
}

void makeMove(Fifteen& puzzle) {
    int tile = console::readInt("Номер плитки, которую нужно сдвинуть: ");
    if (!puzzle.moveTile(tile)) {
        std::cout << "Эту плитку нельзя сдвинуть.\n";
    }
}

void checkSolved(Fifteen& puzzle) {
    std::cout << (puzzle.isSolved() ? "Головоломка собрана.\n" : "Головоломка ещё не собрана.\n");
    std::cout << (puzzle.isSolvable() ? "Её можно собрать.\n" : "Эту расстановку собрать нельзя.\n");
}

void saveToFile(Fifteen& puzzle) {
    std::string path = console::readNonEmptyLine("Путь к файлу: ");
    std::ofstream file(path);
    if (!file) {
        throw std::runtime_error("Не удалось открыть файл для записи: " + path);
    }
    file << puzzle;
    std::cout << "Игра сохранена.\n";
}

void loadFromFile(Fifteen& puzzle) {
    std::string path = console::readNonEmptyLine("Путь к файлу (например, data/fifteen_sample.txt): ");
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Не удалось открыть файл: " + path);
    }
    Fifteen loaded;
    if (!(file >> loaded)) {
        throw std::runtime_error("Неверный формат файла: " + path);
    }
    puzzle = loaded;
    std::cout << "Игра загружена.\n";
}

const std::vector<MenuItem> MENU = {
    {"Новая игра", startNewGame},
    {"Сделать ход", makeMove},
    {"Проверить расстановку", checkSolved},
    {"Сохранить в файл", saveToFile},
    {"Загрузить из файла", loadFromFile},
};

void printMenu() {
    std::cout << "\n=== Пятнашки ===\n";
    for (std::size_t index = 0; index < MENU.size(); ++index) {
        std::cout << index + 1 << ". " << MENU[index].title << '\n';
    }
    std::cout << EXIT_CHOICE << ". Выход\n";
}

void runAction(const MenuItem& item, Fifteen& puzzle) {
    try {
        item.action(puzzle);
    } catch (const std::logic_error& error) {
        console::printError(error);
    } catch (const std::runtime_error& error) {
        console::printError(error);
    }
}

void runMenuLoop(Fifteen& puzzle) {
    while (true) {
        printBoard(puzzle);
        if (puzzle.isSolved()) {
            std::cout << "Поздравляем, головоломка собрана!\n";
        }
        printMenu();
        int choice = console::readInt("Выберите пункт: ");
        if (choice == EXIT_CHOICE) {
            return;
        }
        if (choice < 1 || choice > static_cast<int>(MENU.size())) {
            std::cout << "Нет такого пункта меню.\n";
            continue;
        }
        runAction(MENU[choice - 1], puzzle);
    }
}

}  // namespace

int main() {
    console::setupEncoding();
    Fifteen puzzle;
    puzzle.randomize();
    try {
        runMenuLoop(puzzle);
    } catch (const console::InputClosed&) {
        std::cout << "\nВвод завершён.\n";
    }
    return 0;
}
