#include "recipes/ChemicalProcess.h"

#include "Format.h"
#include "exceptions/CulinaryException.h"

ChemicalProcess::ChemicalProcess(const std::string& name, int durationMinutes, const std::string& agent,
                                 double phLevel)
    : CookingProcess(name, durationMinutes), _agent(agent), _phLevel(phLevel) {
    if (phLevel < MIN_PH || phLevel > MAX_PH) {
        throw RecipeException("Кислотность pH должна быть от 0 до 14");
    }
}

std::string ChemicalProcess::execute() const {
    return getName() + " (" + _agent + ", pH " + format::number(_phLevel) + "), " + std::to_string(getDuration()) +
           " мин";
}

bool ChemicalProcess::isAcidic() const {
    return _phLevel < NEUTRAL_PH;
}
