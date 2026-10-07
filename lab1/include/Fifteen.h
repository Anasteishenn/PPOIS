/**
 * @file Fifteen.h
 * @brief Объявление класса Fifteen — игры-головоломки «Пятнашки».
 */
#pragma once

#include <iosfwd>
#include <random>
#include <vector>

/**
 * @brief Игра «Пятнашки» на квадратном поле.
 *
 * Плитки пронумерованы от 1 до size*size - 1, пустая клетка обозначается нулём.
 * Клетки нумеруются по строкам слева направо, сверху вниз (индекс = строка * размер + столбец).
 * Собранной считается головоломка, в которой плитки идут по порядку, а пустая клетка стоит последней.
 *
 * Формат текстового представления (файл и потоки):
 * @code
 * 3
 * 1 2 3
 * 4 5 6
 * 7 8 0
 * @endcode
 */
class Fifteen {
public:
    /// Размер поля по умолчанию (классические пятнашки 4x4).
    static constexpr int DEFAULT_SIZE = 4;
    /// Минимальный размер стороны поля.
    static constexpr int MIN_SIZE = 2;
    /// Максимальный размер стороны поля.
    static constexpr int MAX_SIZE = 10;
    /// Значение пустой клетки.
    static constexpr int EMPTY_CELL = 0;

    /**
     * @brief Создаёт собранную головоломку.
     * @param size Сторона поля (от MIN_SIZE до MAX_SIZE).
     * @throw std::invalid_argument если размер вне допустимых границ.
     */
    explicit Fifteen(int size = DEFAULT_SIZE);

    /// Конструктор копирования.
    Fifteen(const Fifteen& other) = default;

    /// Присваивание копированием.
    Fifteen& operator=(const Fifteen& other) = default;

    /// Деструктор (память освобождает std::vector).
    ~Fifteen() = default;

    /// Проверяет, допустим ли размер стороны поля.
    static bool isValidSize(int size);

    /// Сторона поля.
    int size() const noexcept;

    /**
     * @brief Случайно перемешивает плитки, начальное состояние берётся из random_device.
     *
     * Перемешивание делается случайными допустимыми ходами из собранного состояния,
     * поэтому полученную головоломку всегда можно собрать.
     */
    void randomize();

    /// То же, что randomize(), но с заданным начальным значением генератора (результат воспроизводим).
    void randomize(unsigned int seed);

    /**
     * @brief Проверяет, можно ли сдвинуть плитку на пустую клетку.
     * @param tile Номер плитки.
     * @return true, если плитка существует и стоит рядом с пустой клеткой.
     */
    bool canMove(int tile) const;

    /**
     * @brief Переставляет плитку и пустую клетку местами, если это допустимый ход.
     * @return true, если ход выполнен.
     */
    bool moveTile(int tile);

    /**
     * @brief Значение клетки по индексу: puzzle[index].
     * @throw std::out_of_range если индекс вне поля.
     */
    int operator[](int index) const;

    /**
     * @brief Значение клетки по строке и столбцу (с нуля).
     * @throw std::out_of_range если координаты вне поля.
     */
    int valueAt(int row, int col) const;

    /// Проверяет, что плитки расставлены правильно (головоломка собрана).
    bool isSolved() const;

    /// Проверяет, можно ли собрать текущую расстановку (по чётности перестановки).
    bool isSolvable() const;

    /// Головоломки равны, если совпадают размер и расстановка.
    bool operator==(const Fifteen& other) const;

    /// Головоломки различаются.
    bool operator!=(const Fifteen& other) const;

    /// Записывает поле в поток в формате, описанном в документации класса.
    friend std::ostream& operator<<(std::ostream& output, const Fifteen& puzzle);

    /**
     * @brief Читает поле из потока.
     *
     * При ошибке (неверный размер, числа не образуют перестановку 0..size*size-1)
     * устанавливается failbit, а головоломка остаётся без изменений.
     */
    friend std::istream& operator>>(std::istream& input, Fifteen& puzzle);

private:
    void resetToSolved();
    void makeRandomMoves(std::mt19937& engine);
    std::vector<int> neighborsOfEmpty() const;
    void swapWithEmpty(int index);
    int indexOfTile(int tile) const;
    int countInversions() const;
    bool assignCells(int newSize, const std::vector<int>& newCells);

    int size_;
    std::vector<int> cells_;
    int emptyIndex_;
};
