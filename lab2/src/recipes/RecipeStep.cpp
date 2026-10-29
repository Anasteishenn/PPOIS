#include "recipes/RecipeStep.h"

#include <utility>

#include "exceptions/CulinaryException.h"

RecipeStep::RecipeStep(int number, const std::string& instruction, std::shared_ptr<CookingProcess> process)
    : _number(number), _instruction(instruction), _process(std::move(process)) {
    if (number <= 0 || instruction.empty() || !_process) {
        throw RecipeException("У шага нужен положительный номер, текст и процесс");
    }
}

std::string RecipeStep::perform() const {
    return "Шаг " + std::to_string(_number) + ": " + _instruction + " — " + _process->run();
}

int RecipeStep::duration() const {
    return _process->getDuration();
}
