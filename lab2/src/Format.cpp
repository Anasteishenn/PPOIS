#include "Format.h"

#include <iomanip>
#include <sstream>

namespace format {

namespace {
constexpr int DIGITS_AFTER_POINT = 2;
}

std::string number(double value) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(DIGITS_AFTER_POINT) << value;
    return stream.str();
}

}  // namespace format
