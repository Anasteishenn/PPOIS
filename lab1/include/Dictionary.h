/**
 * @file Dictionary.h
 * @brief Объявление класса Dictionary — англо-русского словаря на двоичном дереве поиска.
 */
#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>
#include <utility>
#include <vector>

/**
 * @brief Англо-русский словарь.
 *
 * Слова хранятся в двоичном дереве поиска. В узле лежит пара «английское слово — перевод»,
 * ключом является английское слово. Сравнение слов регистрозависимое.
 *
 * Формат текстового представления (файл и потоки): одна строка на слово, вида
 * @code
 * cat;кошка
 * @endcode
 * Английское слово не может содержать символ ';' и перевод строки, перевод не может содержать перевод строки.
 * Обе части должны быть непустыми.
 */
class Dictionary {
public:
    /// Пара «английское слово, перевод» (std::string).
    using Entry = std::pair<std::string, std::string>;
    /// Пара «английское слово, перевод» (C-строки).
    using CStringEntry = std::pair<const char*, const char*>;

    /// Символ-разделитель слова и перевода в текстовом представлении.
    static constexpr char SEPARATOR = ';';

    /// Создаёт пустой словарь.
    Dictionary();

    /// Конструктор копирования (глубокое копирование дерева).
    Dictionary(const Dictionary& other);

    /// Присваивание копированием (идиома copy-and-swap).
    Dictionary& operator=(const Dictionary& other);

    /// Деструктор: освобождает все узлы дерева.
    ~Dictionary();

    /// Обменивает содержимое двух словарей.
    void swap(Dictionary& other) noexcept;

    /**
     * @brief Добавляет слово и перевод.
     * @return true, если слово добавлено; false, если такое слово уже есть (словарь не меняется).
     * @throw std::invalid_argument если слово или перевод пустые или содержат недопустимые символы.
     */
    bool add(const std::string& english, const std::string& russian);

    /**
     * @brief Удаляет слово.
     * @return true, если слово было в словаре и удалено.
     */
    bool remove(const std::string& english);

    /// Проверяет, есть ли слово в словаре.
    bool contains(const std::string& english) const;

    /// Количество слов в словаре.
    std::size_t size() const noexcept;

    /// Проверяет, что словарь пуст.
    bool empty() const noexcept;

    /// Удаляет все слова.
    void clear();

    /// Возвращает все пары, упорядоченные по английскому слову.
    std::vector<Entry> entries() const;

    /**
     * @brief Добавление слова: dictionary += Entry("cat", "кошка").
     * @throw std::invalid_argument если слово уже есть или данные некорректны.
     */
    Dictionary& operator+=(const Entry& entry);

    /**
     * @brief Добавление слова для C-строк: dictionary += std::make_pair("cat", "кошка").
     * @throw std::invalid_argument если один из указателей нулевой, слово уже есть или данные некорректны.
     */
    Dictionary& operator+=(const CStringEntry& entry);

    /**
     * @brief Удаление слова: dictionary -= "cat".
     * @throw std::out_of_range если слова нет в словаре.
     */
    Dictionary& operator-=(const std::string& english);

    /**
     * @brief Поиск перевода и его замена: dictionary["cat"] = "кот".
     * @return Ссылка на перевод. Присваивать пустую строку или строку с переводом строки нельзя.
     * @throw std::out_of_range если слова нет в словаре.
     */
    std::string& operator[](const std::string& english);

    /// Поиск перевода в константном словаре.
    const std::string& operator[](const std::string& english) const;

    /// Словари равны, если содержат одинаковые пары (форма деревьев не важна).
    bool operator==(const Dictionary& other) const;

    /// Словари различаются.
    bool operator!=(const Dictionary& other) const;

    /**
     * @brief Загружает словарь из файла. Прежнее содержимое заменяется.
     * @return Количество слов в словаре после загрузки.
     * @throw std::runtime_error если файл не открылся или имеет неверный формат.
     */
    std::size_t loadFromFile(const std::string& path);

    /**
     * @brief Сохраняет словарь в файл.
     * @throw std::runtime_error если файл не удалось открыть для записи.
     */
    void saveToFile(const std::string& path) const;

    /// Записывает словарь в поток (по строке на слово).
    friend std::ostream& operator<<(std::ostream& output, const Dictionary& dictionary);

    /**
     * @brief Читает словарь из потока до конца данных. Прежнее содержимое заменяется.
     *
     * При ошибке формата устанавливается failbit, а словарь остаётся без изменений.
     * Повторяющиеся слова во входных данных пропускаются, пустые строки игнорируются.
     */
    friend std::istream& operator>>(std::istream& input, Dictionary& dictionary);

private:
    /// Узел двоичного дерева поиска.
    struct Node {
        Node(std::string englishWord, std::string translation);

        std::string english;
        std::string russian;
        Node* left;
        Node* right;
    };

    static Node* cloneTree(const Node* node);
    static void destroyTree(Node* node);
    static Node* insertNode(Node* node, const std::string& english, const std::string& russian, bool& inserted);
    static Node* removeNode(Node* node, const std::string& english, bool& removed);
    static Node* detachNode(Node* node);
    static Node* minNode(Node* node);
    static void collectEntries(const Node* node, std::vector<Entry>& result);

    Node* findNode(const std::string& english) const;
    Node* requireNode(const std::string& english) const;
    bool readEntries(std::istream& input);
    bool addLine(const std::string& line);

    Node* root_;
    std::size_t size_;
};
