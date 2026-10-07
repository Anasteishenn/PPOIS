/**
 * @file dictionary_main.cpp
 * @brief Консольное меню для проверки класса Dictionary (англо-русский словарь).
 */
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "ConsoleUtils.h"
#include "Dictionary.h"

namespace {

const int EXIT_CHOICE = 0;

using Action = void (*)(Dictionary&);

struct MenuItem {
    const char* title;
    Action action;
};

void addWord(Dictionary& dictionary) {
    std::string english = console::readNonEmptyLine("Английское слово: ");
    std::string russian = console::readNonEmptyLine("Перевод: ");
    dictionary += Dictionary::Entry(english, russian);
    std::cout << "Слово добавлено.\n";
}

void removeWord(Dictionary& dictionary) {
    std::string english = console::readNonEmptyLine("Слово для удаления: ");
    dictionary -= english;
    std::cout << "Слово удалено.\n";
}

void translateWord(Dictionary& dictionary) {
    std::string english = console::readNonEmptyLine("Слово для поиска: ");
    const std::string& translation = dictionary[english];
    std::cout << english << " - " << translation << '\n';
}

void replaceTranslation(Dictionary& dictionary) {
    std::string english = console::readNonEmptyLine("Слово: ");
    std::cout << "Текущий перевод: " << dictionary[english] << '\n';
    dictionary[english] = console::readNonEmptyLine("Новый перевод: ");
    std::cout << "Перевод заменён.\n";
}

void showCount(Dictionary& dictionary) {
    std::cout << "Слов в словаре: " << dictionary.size() << '\n';
}

void listWords(Dictionary& dictionary) {
    if (dictionary.empty()) {
        std::cout << "Словарь пуст.\n";
        return;
    }
    for (const Dictionary::Entry& entry : dictionary.entries()) {
        std::cout << entry.first << " - " << entry.second << '\n';
    }
}

void loadFromFile(Dictionary& dictionary) {
    std::string path = console::readNonEmptyLine("Путь к файлу (например, data/words.txt): ");
    std::size_t count = dictionary.loadFromFile(path);
    std::cout << "Загружено слов: " << count << '\n';
}

void saveToFile(Dictionary& dictionary) {
    std::string path = console::readNonEmptyLine("Путь к файлу: ");
    dictionary.saveToFile(path);
    std::cout << "Словарь сохранён.\n";
}

const std::vector<MenuItem> MENU = {
    {"Добавить слово", addWord},
    {"Удалить слово", removeWord},
    {"Найти перевод", translateWord},
    {"Заменить перевод", replaceTranslation},
    {"Количество слов", showCount},
    {"Показать весь словарь", listWords},
    {"Загрузить из файла", loadFromFile},
    {"Сохранить в файл", saveToFile},
};

void printMenu() {
    std::cout << "\n=== Англо-русский словарь ===\n";
    for (std::size_t index = 0; index < MENU.size(); ++index) {
        std::cout << index + 1 << ". " << MENU[index].title << '\n';
    }
    std::cout << EXIT_CHOICE << ". Выход\n";
}

void runAction(const MenuItem& item, Dictionary& dictionary) {
    try {
        item.action(dictionary);
    } catch (const std::logic_error& error) {
        console::printError(error);
    } catch (const std::runtime_error& error) {
        console::printError(error);
    }
}

void runMenuLoop(Dictionary& dictionary) {
    while (true) {
        printMenu();
        int choice = console::readInt("Выберите пункт: ");
        if (choice == EXIT_CHOICE) {
            return;
        }
        if (choice < 1 || choice > static_cast<int>(MENU.size())) {
            std::cout << "Нет такого пункта меню.\n";
            continue;
        }
        runAction(MENU[choice - 1], dictionary);
    }
}

}  // namespace

int main() {
    console::setupEncoding();
    Dictionary dictionary;
    try {
        runMenuLoop(dictionary);
    } catch (const console::InputClosed&) {
        std::cout << "\nВвод завершён.\n";
    }
    return 0;
}
