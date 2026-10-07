#include "Dictionary.h"

#include <fstream>
#include <istream>
#include <ostream>
#include <stdexcept>

namespace {

const std::string UTF8_BOM = "\xEF\xBB\xBF";
const char CARRIAGE_RETURN = '\r';
const char LINE_FEED = '\n';

bool isValidEntry(const std::string& english, const std::string& russian) {
    if (english.empty() || russian.empty()) {
        return false;
    }
    bool englishIsClean = english.find(Dictionary::SEPARATOR) == std::string::npos &&
                          english.find(LINE_FEED) == std::string::npos;
    bool russianIsClean = russian.find(LINE_FEED) == std::string::npos;
    return englishIsClean && russianIsClean;
}

void removeLineEnding(std::string& line) {
    if (!line.empty() && line.back() == CARRIAGE_RETURN) {
        line.pop_back();
    }
}

void removeBom(std::string& line) {
    if (line.compare(0, UTF8_BOM.size(), UTF8_BOM) == 0) {
        line.erase(0, UTF8_BOM.size());
    }
}

}  // namespace

Dictionary::Node::Node(std::string englishWord, std::string translation)
    : english(std::move(englishWord)), russian(std::move(translation)), left(nullptr), right(nullptr) {}

Dictionary::Dictionary() : root_(nullptr), size_(0) {}

Dictionary::Dictionary(const Dictionary& other) : root_(cloneTree(other.root_)), size_(other.size_) {}

Dictionary& Dictionary::operator=(const Dictionary& other) {
    Dictionary copy(other);
    swap(copy);
    return *this;
}

Dictionary::~Dictionary() {
    destroyTree(root_);
}

void Dictionary::swap(Dictionary& other) noexcept {
    std::swap(root_, other.root_);
    std::swap(size_, other.size_);
}

bool Dictionary::add(const std::string& english, const std::string& russian) {
    if (!isValidEntry(english, russian)) {
        throw std::invalid_argument("Недопустимое слово или перевод");
    }
    bool inserted = false;
    root_ = insertNode(root_, english, russian, inserted);
    if (inserted) {
        ++size_;
    }
    return inserted;
}

bool Dictionary::remove(const std::string& english) {
    bool removed = false;
    root_ = removeNode(root_, english, removed);
    if (removed) {
        --size_;
    }
    return removed;
}

bool Dictionary::contains(const std::string& english) const {
    return findNode(english) != nullptr;
}

std::size_t Dictionary::size() const noexcept {
    return size_;
}

bool Dictionary::empty() const noexcept {
    return size_ == 0;
}

void Dictionary::clear() {
    destroyTree(root_);
    root_ = nullptr;
    size_ = 0;
}

std::vector<Dictionary::Entry> Dictionary::entries() const {
    std::vector<Entry> result;
    collectEntries(root_, result);
    return result;
}

Dictionary& Dictionary::operator+=(const Entry& entry) {
    if (!add(entry.first, entry.second)) {
        throw std::invalid_argument("Слово уже есть в словаре: " + entry.first);
    }
    return *this;
}

Dictionary& Dictionary::operator+=(const CStringEntry& entry) {
    if (entry.first == nullptr || entry.second == nullptr) {
        throw std::invalid_argument("Нулевой указатель вместо строки");
    }
    return *this += Entry(entry.first, entry.second);
}

Dictionary& Dictionary::operator-=(const std::string& english) {
    if (!remove(english)) {
        throw std::out_of_range("Слова нет в словаре: " + english);
    }
    return *this;
}

std::string& Dictionary::operator[](const std::string& english) {
    return requireNode(english)->russian;
}

const std::string& Dictionary::operator[](const std::string& english) const {
    return requireNode(english)->russian;
}

bool Dictionary::operator==(const Dictionary& other) const {
    return size_ == other.size_ && entries() == other.entries();
}

bool Dictionary::operator!=(const Dictionary& other) const {
    return !(*this == other);
}

std::size_t Dictionary::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Не удалось открыть файл: " + path);
    }
    Dictionary loaded;
    if (!(file >> loaded)) {
        throw std::runtime_error("Неверный формат файла: " + path);
    }
    swap(loaded);
    return size_;
}

void Dictionary::saveToFile(const std::string& path) const {
    std::ofstream file(path);
    if (!file) {
        throw std::runtime_error("Не удалось открыть файл для записи: " + path);
    }
    file << *this;
}

std::ostream& operator<<(std::ostream& output, const Dictionary& dictionary) {
    for (const Dictionary::Entry& entry : dictionary.entries()) {
        output << entry.first << Dictionary::SEPARATOR << entry.second << LINE_FEED;
    }
    return output;
}

std::istream& operator>>(std::istream& input, Dictionary& dictionary) {
    Dictionary loaded;
    if (!loaded.readEntries(input)) {
        input.setstate(std::ios::failbit);
        return input;
    }
    input.clear(std::ios::eofbit);
    dictionary.swap(loaded);
    return input;
}

bool Dictionary::readEntries(std::istream& input) {
    std::string line;
    bool isFirstLine = true;
    while (std::getline(input, line)) {
        if (isFirstLine) {
            removeBom(line);
            isFirstLine = false;
        }
        removeLineEnding(line);
        if (!line.empty() && !addLine(line)) {
            return false;
        }
    }
    return true;
}

bool Dictionary::addLine(const std::string& line) {
    std::size_t separatorPosition = line.find(SEPARATOR);
    if (separatorPosition == std::string::npos) {
        return false;
    }
    std::string english = line.substr(0, separatorPosition);
    std::string russian = line.substr(separatorPosition + 1);
    if (!isValidEntry(english, russian)) {
        return false;
    }
    add(english, russian);
    return true;
}

Dictionary::Node* Dictionary::cloneTree(const Node* node) {
    if (node == nullptr) {
        return nullptr;
    }
    Node* copy = new Node(node->english, node->russian);
    copy->left = cloneTree(node->left);
    copy->right = cloneTree(node->right);
    return copy;
}

void Dictionary::destroyTree(Node* node) {
    if (node == nullptr) {
        return;
    }
    destroyTree(node->left);
    destroyTree(node->right);
    delete node;
}

Dictionary::Node* Dictionary::insertNode(Node* node, const std::string& english, const std::string& russian,
                                         bool& inserted) {
    if (node == nullptr) {
        inserted = true;
        return new Node(english, russian);
    }
    if (english < node->english) {
        node->left = insertNode(node->left, english, russian, inserted);
    } else if (english > node->english) {
        node->right = insertNode(node->right, english, russian, inserted);
    }
    return node;
}

Dictionary::Node* Dictionary::removeNode(Node* node, const std::string& english, bool& removed) {
    if (node == nullptr) {
        return nullptr;
    }
    if (english < node->english) {
        node->left = removeNode(node->left, english, removed);
        return node;
    }
    if (english > node->english) {
        node->right = removeNode(node->right, english, removed);
        return node;
    }
    removed = true;
    return detachNode(node);
}

Dictionary::Node* Dictionary::detachNode(Node* node) {
    if (node->left == nullptr || node->right == nullptr) {
        Node* child = (node->left != nullptr) ? node->left : node->right;
        delete node;
        return child;
    }
    const Node* successor = minNode(node->right);
    node->english = successor->english;
    node->russian = successor->russian;
    bool successorRemoved = false;
    node->right = removeNode(node->right, node->english, successorRemoved);
    return node;
}

Dictionary::Node* Dictionary::minNode(Node* node) {
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}

void Dictionary::collectEntries(const Node* node, std::vector<Entry>& result) {
    if (node == nullptr) {
        return;
    }
    collectEntries(node->left, result);
    result.emplace_back(node->english, node->russian);
    collectEntries(node->right, result);
}

Dictionary::Node* Dictionary::findNode(const std::string& english) const {
    Node* current = root_;
    while (current != nullptr && current->english != english) {
        current = (english < current->english) ? current->left : current->right;
    }
    return current;
}

Dictionary::Node* Dictionary::requireNode(const std::string& english) const {
    Node* node = findNode(english);
    if (node == nullptr) {
        throw std::out_of_range("Слова нет в словаре: " + english);
    }
    return node;
}
