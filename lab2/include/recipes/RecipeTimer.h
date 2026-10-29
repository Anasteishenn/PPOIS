#pragma once

/// @brief Кухонный таймер: отсчитывает минуты процесса приготовления.
class RecipeTimer {
public:
    explicit RecipeTimer(int durationMinutes);

    int getDuration() const { return _durationMinutes; }
    bool isRunning() const { return _isRunning; }

    /// Запустить таймер.
    void start();
    /// Прошло несколько минут. Когда время вышло, таймер сам останавливается.
    void tick(int minutes);
    /// Вышло ли время.
    bool isFinished() const;
    /// Сколько минут осталось.
    int remaining() const;
    /// Сбросить таймер в начальное состояние.
    void reset();

private:
    int _durationMinutes;
    int _elapsedMinutes;
    bool _isRunning;
};
