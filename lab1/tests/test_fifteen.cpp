#include <sstream>
#include <stdexcept>
#include <string>

#include "Fifteen.h"
#include "doctest.h"

namespace {

const unsigned int TEST_SEED = 12345;

Fifteen loadPuzzle(const std::string& text) {
    std::stringstream stream(text);
    Fifteen puzzle;
    stream >> puzzle;
    return puzzle;
}

}  // namespace

TEST_CASE("Новая головоломка собрана и имеет размер по умолчанию") {
    Fifteen puzzle;
    CHECK(puzzle.size() == Fifteen::DEFAULT_SIZE);
    CHECK(puzzle.isSolved());
    CHECK(puzzle.isSolvable());
    CHECK(puzzle[0] == 1);
    CHECK(puzzle[14] == 15);
    CHECK(puzzle[15] == Fifteen::EMPTY_CELL);
}

TEST_CASE("Недопустимый размер поля") {
    CHECK_THROWS_AS(Fifteen(Fifteen::MIN_SIZE - 1), std::invalid_argument);
    CHECK_THROWS_AS(Fifteen(Fifteen::MAX_SIZE + 1), std::invalid_argument);
    CHECK_THROWS_AS(Fifteen(-3), std::invalid_argument);
    CHECK(Fifteen::isValidSize(Fifteen::MIN_SIZE));
    CHECK(Fifteen::isValidSize(Fifteen::MAX_SIZE));
}

TEST_CASE("Получение значения клетки") {
    Fifteen puzzle(3);
    CHECK(puzzle.valueAt(0, 0) == 1);
    CHECK(puzzle.valueAt(1, 2) == 6);
    CHECK(puzzle.valueAt(2, 2) == Fifteen::EMPTY_CELL);
    CHECK(puzzle[4] == 5);
}

TEST_CASE("Обращение к клетке вне поля") {
    Fifteen puzzle(3);
    CHECK_THROWS_AS(puzzle[-1], std::out_of_range);
    CHECK_THROWS_AS(puzzle[9], std::out_of_range);
    CHECK_THROWS_AS(puzzle.valueAt(-1, 0), std::out_of_range);
    CHECK_THROWS_AS(puzzle.valueAt(3, 0), std::out_of_range);
    CHECK_THROWS_AS(puzzle.valueAt(0, -1), std::out_of_range);
    CHECK_THROWS_AS(puzzle.valueAt(0, 3), std::out_of_range);
}

TEST_CASE("Допустимый ход переставляет плитку и пустую клетку") {
    Fifteen puzzle(3);
    CHECK(puzzle.canMove(6));
    CHECK(puzzle.moveTile(6));
    CHECK(puzzle[5] == Fifteen::EMPTY_CELL);
    CHECK(puzzle[8] == 6);
    CHECK_FALSE(puzzle.isSolved());
}

TEST_CASE("Недопустимые ходы") {
    Fifteen puzzle(3);
    CHECK_FALSE(puzzle.canMove(1));
    CHECK_FALSE(puzzle.moveTile(1));
    CHECK_FALSE(puzzle.moveTile(Fifteen::EMPTY_CELL));
    CHECK_FALSE(puzzle.moveTile(9));
    CHECK_FALSE(puzzle.moveTile(-5));
    CHECK(puzzle.isSolved());
}

TEST_CASE("Ходы возможны с разных сторон от пустой клетки") {
    Fifteen puzzle(3);
    puzzle.moveTile(6);
    puzzle.moveTile(5);
    puzzle.moveTile(4);
    CHECK(puzzle[3] == Fifteen::EMPTY_CELL);
    CHECK(puzzle.canMove(1));
    CHECK(puzzle.canMove(7));
    CHECK(puzzle.canMove(4));
    CHECK_FALSE(puzzle.canMove(9));
}

TEST_CASE("Возврат плитки на место снова собирает головоломку") {
    Fifteen puzzle;
    CHECK(puzzle.moveTile(15));
    CHECK_FALSE(puzzle.isSolved());
    CHECK(puzzle.moveTile(15));
    CHECK(puzzle.isSolved());
}

TEST_CASE("Неверная расстановка не считается собранной") {
    Fifteen puzzle = loadPuzzle("2\n2 1\n3 0\n");
    CHECK_FALSE(puzzle.isSolved());
    Fifteen emptyNotLast = loadPuzzle("2\n1 2\n0 3\n");
    CHECK_FALSE(emptyNotLast.isSolved());
}

TEST_CASE("Случайное размещение: можно собрать и не собрано") {
    for (int size = Fifteen::MIN_SIZE; size <= 5; ++size) {
        Fifteen puzzle(size);
        puzzle.randomize(TEST_SEED);
        CHECK(puzzle.isSolvable());
        CHECK_FALSE(puzzle.isSolved());
    }
}

TEST_CASE("Одинаковое начальное значение даёт одинаковое размещение") {
    Fifteen first;
    Fifteen second;
    first.randomize(TEST_SEED);
    second.randomize(TEST_SEED);
    CHECK(first == second);
}

TEST_CASE("Случайное размещение без начального значения") {
    Fifteen puzzle;
    puzzle.randomize();
    CHECK(puzzle.isSolvable());
    CHECK_FALSE(puzzle.isSolved());
}

TEST_CASE("Проверка возможности собрать расстановку") {
    CHECK(loadPuzzle("3\n1 2 3\n4 5 6\n7 0 8\n").isSolvable());
    CHECK_FALSE(loadPuzzle("3\n2 1 3\n4 5 6\n7 8 0\n").isSolvable());
    CHECK_FALSE(loadPuzzle("4\n1 2 3 4\n5 6 7 8\n9 10 11 12\n13 15 14 0\n").isSolvable());
    CHECK(loadPuzzle("4\n1 2 3 4\n5 6 7 8\n9 10 11 12\n13 14 0 15\n").isSolvable());
}

TEST_CASE("Сравнение головоломок") {
    Fifteen first(3);
    Fifteen second(3);
    CHECK(first == second);
    second.moveTile(6);
    CHECK(first != second);
    Fifteen otherSize(4);
    CHECK(first != otherSize);
}

TEST_CASE("Копирование и присваивание независимы") {
    Fifteen original(3);
    Fifteen copy(original);
    Fifteen assigned(2);
    assigned = original;
    CHECK(copy == original);
    CHECK(assigned == original);
    assigned.moveTile(6);
    CHECK(assigned != original);
    CHECK(original.isSolved());
}

TEST_CASE("Запись в поток и чтение обратно") {
    Fifteen original(3);
    original.randomize(TEST_SEED);
    std::stringstream stream;
    stream << original;
    Fifteen loaded(2);
    stream >> loaded;
    CHECK(loaded == original);
    CHECK(loaded.size() == 3);
}

TEST_CASE("Формат записи в поток") {
    std::stringstream stream;
    stream << Fifteen(2);
    CHECK(stream.str() == "2\n1 2\n3 0\n");
}

TEST_CASE("Чтение некорректных данных не меняет головоломку") {
    Fifteen puzzle(3);
    const char* brokenInputs[] = {
        "",
        "abc",
        "1\n0\n",
        "11\n",
        "2\n1 2 3\n",
        "2\n1 1 2 0\n",
        "2\n1 2 3 4\n",
        "2\n-1 1 2 3\n",
    };
    for (const char* text : brokenInputs) {
        std::stringstream stream(text);
        CHECK_FALSE(static_cast<bool>(stream >> puzzle));
    }
    CHECK(puzzle == Fifteen(3));
}
