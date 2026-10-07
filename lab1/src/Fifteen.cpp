#include "Fifteen.h"

#include <algorithm>
#include <istream>
#include <numeric>
#include <ostream>
#include <stdexcept>

namespace {

const int MOVES_PER_CELL = 50;
const int NOT_FOUND = -1;
const int PARITY_DIVISOR = 2;

int checkedSize(int size) {
    if (!Fifteen::isValidSize(size)) {
        throw std::invalid_argument("Недопустимый размер поля");
    }
    return size;
}

}  // namespace

Fifteen::Fifteen(int size) : size_(checkedSize(size)), cells_(size_ * size_), emptyIndex_(0) {
    resetToSolved();
}

bool Fifteen::isValidSize(int size) {
    return size >= MIN_SIZE && size <= MAX_SIZE;
}

int Fifteen::size() const noexcept {
    return size_;
}

void Fifteen::randomize() {
    std::random_device device;
    randomize(device());
}

void Fifteen::randomize(unsigned int seed) {
    std::mt19937 engine(seed);
    do {
        resetToSolved();
        makeRandomMoves(engine);
    } while (isSolved());
}

bool Fifteen::canMove(int tile) const {
    if (tile == EMPTY_CELL) {
        return false;
    }
    int index = indexOfTile(tile);
    if (index == NOT_FOUND) {
        return false;
    }
    std::vector<int> neighbors = neighborsOfEmpty();
    return std::find(neighbors.begin(), neighbors.end(), index) != neighbors.end();
}

bool Fifteen::moveTile(int tile) {
    if (!canMove(tile)) {
        return false;
    }
    swapWithEmpty(indexOfTile(tile));
    return true;
}

int Fifteen::operator[](int index) const {
    if (index < 0 || index >= static_cast<int>(cells_.size())) {
        throw std::out_of_range("Индекс клетки вне поля");
    }
    return cells_[index];
}

int Fifteen::valueAt(int row, int col) const {
    if (row < 0 || row >= size_ || col < 0 || col >= size_) {
        throw std::out_of_range("Координаты клетки вне поля");
    }
    return cells_[row * size_ + col];
}

bool Fifteen::isSolved() const {
    int lastIndex = static_cast<int>(cells_.size()) - 1;
    for (int index = 0; index < lastIndex; ++index) {
        if (cells_[index] != index + 1) {
            return false;
        }
    }
    return cells_[lastIndex] == EMPTY_CELL;
}

bool Fifteen::isSolvable() const {
    int inversions = countInversions();
    if (size_ % PARITY_DIVISOR != 0) {
        return inversions % PARITY_DIVISOR == 0;
    }
    int emptyRow = emptyIndex_ / size_;
    return (inversions + emptyRow) % PARITY_DIVISOR != 0;
}

bool Fifteen::operator==(const Fifteen& other) const {
    return size_ == other.size_ && cells_ == other.cells_;
}

bool Fifteen::operator!=(const Fifteen& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& output, const Fifteen& puzzle) {
    output << puzzle.size_ << '\n';
    for (int index = 0; index < static_cast<int>(puzzle.cells_.size()); ++index) {
        bool isRowEnd = (index + 1) % puzzle.size_ == 0;
        output << puzzle.cells_[index] << (isRowEnd ? '\n' : ' ');
    }
    return output;
}

std::istream& operator>>(std::istream& input, Fifteen& puzzle) {
    int newSize = 0;
    if (!(input >> newSize) || !Fifteen::isValidSize(newSize)) {
        input.setstate(std::ios::failbit);
        return input;
    }
    std::vector<int> newCells(newSize * newSize);
    for (int& value : newCells) {
        input >> value;
    }
    if (!input || !puzzle.assignCells(newSize, newCells)) {
        input.setstate(std::ios::failbit);
    }
    return input;
}

void Fifteen::resetToSolved() {
    std::iota(cells_.begin(), cells_.end(), 1);
    cells_.back() = EMPTY_CELL;
    emptyIndex_ = static_cast<int>(cells_.size()) - 1;
}

void Fifteen::makeRandomMoves(std::mt19937& engine) {
    const int moveCount = size_ * size_ * MOVES_PER_CELL;
    for (int step = 0; step < moveCount; ++step) {
        std::vector<int> neighbors = neighborsOfEmpty();
        std::uniform_int_distribution<std::size_t> pickNeighbor(0, neighbors.size() - 1);
        swapWithEmpty(neighbors[pickNeighbor(engine)]);
    }
}

std::vector<int> Fifteen::neighborsOfEmpty() const {
    std::vector<int> neighbors;
    int row = emptyIndex_ / size_;
    int col = emptyIndex_ % size_;
    if (row > 0) {
        neighbors.push_back(emptyIndex_ - size_);
    }
    if (row < size_ - 1) {
        neighbors.push_back(emptyIndex_ + size_);
    }
    if (col > 0) {
        neighbors.push_back(emptyIndex_ - 1);
    }
    if (col < size_ - 1) {
        neighbors.push_back(emptyIndex_ + 1);
    }
    return neighbors;
}

void Fifteen::swapWithEmpty(int index) {
    std::swap(cells_[index], cells_[emptyIndex_]);
    emptyIndex_ = index;
}

int Fifteen::indexOfTile(int tile) const {
    auto position = std::find(cells_.begin(), cells_.end(), tile);
    if (position == cells_.end()) {
        return NOT_FOUND;
    }
    return static_cast<int>(position - cells_.begin());
}

int Fifteen::countInversions() const {
    int inversions = 0;
    for (std::size_t first = 0; first < cells_.size(); ++first) {
        for (std::size_t second = first + 1; second < cells_.size(); ++second) {
            bool bothAreTiles = cells_[first] != EMPTY_CELL && cells_[second] != EMPTY_CELL;
            if (bothAreTiles && cells_[first] > cells_[second]) {
                ++inversions;
            }
        }
    }
    return inversions;
}

bool Fifteen::assignCells(int newSize, const std::vector<int>& newCells) {
    std::vector<int> sortedCells = newCells;
    std::sort(sortedCells.begin(), sortedCells.end());
    std::vector<int> expectedCells(newCells.size());
    std::iota(expectedCells.begin(), expectedCells.end(), 0);
    if (sortedCells != expectedCells) {
        return false;
    }
    size_ = newSize;
    cells_ = newCells;
    emptyIndex_ = indexOfTile(EMPTY_CELL);
    return true;
}
