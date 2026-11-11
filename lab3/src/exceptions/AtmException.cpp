#include "exceptions/AtmException.h"

AtmException::AtmException(const std::string& message) : _message(message) {}

const char* AtmException::what() const noexcept {
    return _message.c_str();
}
