#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Dictionary.h"
#include "doctest.h"

namespace {

Dictionary makeSampleDictionary() {
    Dictionary dictionary;
    dictionary += Dictionary::Entry("dog", "собака");
    dictionary += Dictionary::Entry("cat", "кошка");
    dictionary += Dictionary::Entry("house", "дом");
    return dictionary;
}

std::string makeTempPath(const std::string& name) {
    return (std::filesystem::temp_directory_path() / name).string();
}

}  // namespace

TEST_CASE("Новый словарь пуст") {
    Dictionary dictionary;
    CHECK(dictionary.empty());
    CHECK(dictionary.size() == 0);
    CHECK_FALSE(dictionary.contains("cat"));
    CHECK(dictionary.entries().empty());
}

TEST_CASE("Добавление слов через += для std::string и C-строк") {
    Dictionary dictionary;
    dictionary += Dictionary::Entry("cat", "кошка");
    dictionary += std::make_pair("dog", "собака");
    CHECK(dictionary.size() == 2);
    CHECK(dictionary["cat"] == "кошка");
    CHECK(dictionary["dog"] == "собака");
}

TEST_CASE("Цепочка += возвращает тот же словарь") {
    Dictionary dictionary;
    (dictionary += std::make_pair("a", "а")) += std::make_pair("b", "б");
    CHECK(dictionary.size() == 2);
}

TEST_CASE("Повторное добавление слова") {
    Dictionary dictionary = makeSampleDictionary();
    CHECK_FALSE(dictionary.add("cat", "кот"));
    CHECK(dictionary.size() == 3);
    CHECK(dictionary["cat"] == "кошка");
    CHECK_THROWS_AS(dictionary += Dictionary::Entry("cat", "кот"), std::invalid_argument);
}

TEST_CASE("Некорректные данные при добавлении") {
    Dictionary dictionary;
    CHECK_THROWS_AS(dictionary.add("", "перевод"), std::invalid_argument);
    CHECK_THROWS_AS(dictionary.add("word", ""), std::invalid_argument);
    CHECK_THROWS_AS(dictionary.add("wo;rd", "слово"), std::invalid_argument);
    CHECK_THROWS_AS(dictionary.add("wo\nrd", "слово"), std::invalid_argument);
    CHECK_THROWS_AS(dictionary.add("word", "сло\nво"), std::invalid_argument);
    Dictionary::CStringEntry nullWord(nullptr, "слово");
    Dictionary::CStringEntry nullTranslation("word", nullptr);
    CHECK_THROWS_AS(dictionary += nullWord, std::invalid_argument);
    CHECK_THROWS_AS(dictionary += nullTranslation, std::invalid_argument);
    CHECK(dictionary.empty());
}

TEST_CASE("Перевод допускает символ-разделитель") {
    Dictionary dictionary;
    dictionary.add("semicolon", "точка;с;запятой");
    std::stringstream stream;
    stream << dictionary;
    Dictionary loaded;
    stream >> loaded;
    CHECK(loaded == dictionary);
}

TEST_CASE("Поиск и замена перевода через []") {
    Dictionary dictionary = makeSampleDictionary();
    CHECK(dictionary["house"] == "дом");
    dictionary["house"] = "здание";
    CHECK(dictionary["house"] == "здание");
    const Dictionary& constDictionary = dictionary;
    CHECK(constDictionary["dog"] == "собака");
}

TEST_CASE("Поиск отсутствующего слова бросает исключение") {
    Dictionary dictionary = makeSampleDictionary();
    const Dictionary& constDictionary = dictionary;
    CHECK_THROWS_AS(dictionary["bird"], std::out_of_range);
    CHECK_THROWS_AS(constDictionary["bird"], std::out_of_range);
}

TEST_CASE("Слова упорядочены по алфавиту") {
    Dictionary dictionary = makeSampleDictionary();
    std::vector<Dictionary::Entry> expected = {{"cat", "кошка"}, {"dog", "собака"}, {"house", "дом"}};
    CHECK(dictionary.entries() == expected);
}

TEST_CASE("Удаление листа, узла с одним потомком и узла с двумя потомками") {
    Dictionary dictionary;
    for (const char* word : {"m", "f", "t", "c", "h", "p", "w", "a"}) {
        dictionary.add(word, "перевод");
    }
    dictionary -= "a";
    CHECK_FALSE(dictionary.contains("a"));
    dictionary -= "c";
    CHECK_FALSE(dictionary.contains("c"));
    dictionary -= "t";
    CHECK_FALSE(dictionary.contains("t"));
    dictionary -= "m";
    CHECK_FALSE(dictionary.contains("m"));
    CHECK(dictionary.size() == 4);
    for (const char* word : {"f", "h", "p", "w"}) {
        CHECK(dictionary.contains(word));
    }
}

TEST_CASE("Удаление узла, у которого есть только левый потомок") {
    Dictionary dictionary;
    dictionary.add("b", "б");
    dictionary.add("a", "а");
    CHECK(dictionary.remove("b"));
    CHECK(dictionary.contains("a"));
    CHECK(dictionary.size() == 1);
}

TEST_CASE("Удаление несуществующего слова") {
    Dictionary dictionary = makeSampleDictionary();
    CHECK_FALSE(dictionary.remove("bird"));
    CHECK_THROWS_AS(dictionary -= "bird", std::out_of_range);
    CHECK(dictionary.size() == 3);
}

TEST_CASE("Удаление всех слов и очистка") {
    Dictionary dictionary = makeSampleDictionary();
    dictionary -= "cat";
    dictionary -= "dog";
    dictionary -= "house";
    CHECK(dictionary.empty());
    dictionary = makeSampleDictionary();
    dictionary.clear();
    CHECK(dictionary.empty());
    CHECK_FALSE(dictionary.contains("cat"));
}

TEST_CASE("Конструктор копирования создаёт независимую копию") {
    Dictionary original = makeSampleDictionary();
    Dictionary copy(original);
    CHECK(copy == original);
    copy["cat"] = "кот";
    copy += std::make_pair("bird", "птица");
    CHECK(original["cat"] == "кошка");
    CHECK_FALSE(original.contains("bird"));
}

TEST_CASE("Присваивание копированием") {
    Dictionary original = makeSampleDictionary();
    Dictionary target;
    target += std::make_pair("old", "старый");
    target = original;
    CHECK(target == original);
    CHECK_FALSE(target.contains("old"));
    target["dog"] = "пёс";
    CHECK(original["dog"] == "собака");
}

TEST_CASE("Самоприсваивание не портит словарь") {
    Dictionary dictionary = makeSampleDictionary();
    const Dictionary& alias = dictionary;
    dictionary = alias;
    CHECK(dictionary.size() == 3);
    CHECK(dictionary["cat"] == "кошка");
}

TEST_CASE("Сравнение словарей не зависит от порядка добавления") {
    Dictionary first;
    first.add("a", "а");
    first.add("b", "б");
    Dictionary second;
    second.add("b", "б");
    second.add("a", "а");
    CHECK(first == second);
    CHECK_FALSE(first != second);
}

TEST_CASE("Различающиеся словари") {
    Dictionary first = makeSampleDictionary();
    Dictionary differentTranslation = makeSampleDictionary();
    differentTranslation["cat"] = "кот";
    Dictionary differentSize = makeSampleDictionary();
    differentSize.add("bird", "птица");
    CHECK(first != differentTranslation);
    CHECK(first != differentSize);
}

TEST_CASE("Запись в поток и чтение обратно") {
    Dictionary original = makeSampleDictionary();
    std::stringstream stream;
    stream << original;
    CHECK(stream.str() == "cat;кошка\ndog;собака\nhouse;дом\n");
    Dictionary loaded;
    loaded.add("junk", "мусор");
    stream >> loaded;
    CHECK(loaded == original);
}

TEST_CASE("Чтение из потока: CRLF, BOM, пустые строки и повторы") {
    std::stringstream stream("\xEF\xBB\xBF" "cat;кошка\r\n\r\ndog;собака\r\ncat;кот\r\n");
    Dictionary dictionary;
    CHECK(static_cast<bool>(stream >> dictionary));
    CHECK(dictionary.size() == 2);
    CHECK(dictionary["cat"] == "кошка");
}

TEST_CASE("Чтение из потока: неверный формат оставляет словарь без изменений") {
    Dictionary dictionary = makeSampleDictionary();
    std::stringstream withoutSeparator("cat кошка\n");
    CHECK_FALSE(static_cast<bool>(withoutSeparator >> dictionary));
    std::stringstream emptyTranslation("cat;\n");
    CHECK_FALSE(static_cast<bool>(emptyTranslation >> dictionary));
    CHECK(dictionary == makeSampleDictionary());
}

TEST_CASE("Загрузка и сохранение файла") {
    std::string path = makeTempPath("lab1_dictionary_test.txt");
    Dictionary original = makeSampleDictionary();
    original.saveToFile(path);
    Dictionary loaded;
    loaded.add("junk", "мусор");
    CHECK(loaded.loadFromFile(path) == 3);
    CHECK(loaded == original);
    std::filesystem::remove(path);
}

TEST_CASE("Ошибки работы с файлами") {
    std::string missingPath = makeTempPath("lab1_missing_dir/missing.txt");
    Dictionary dictionary = makeSampleDictionary();
    CHECK_THROWS_AS(dictionary.loadFromFile(missingPath), std::runtime_error);
    CHECK_THROWS_AS(dictionary.saveToFile(missingPath), std::runtime_error);

    std::string brokenPath = makeTempPath("lab1_dictionary_broken.txt");
    std::ofstream(brokenPath) << "строка без разделителя\n";
    CHECK_THROWS_AS(dictionary.loadFromFile(brokenPath), std::runtime_error);
    CHECK(dictionary == makeSampleDictionary());
    std::filesystem::remove(brokenPath);
}

TEST_CASE("Большой словарь из упорядоченных слов") {
    const int wordCount = 500;
    Dictionary dictionary;
    for (int number = 0; number < wordCount; ++number) {
        dictionary.add("word" + std::to_string(1000 + number), "слово");
    }
    CHECK(dictionary.size() == static_cast<std::size_t>(wordCount));
    Dictionary copy(dictionary);
    CHECK(copy == dictionary);
}
