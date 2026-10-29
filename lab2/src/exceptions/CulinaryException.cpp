#include "exceptions/CulinaryException.h"

CulinaryException::CulinaryException(const std::string& message) : _message(message) {}

const char* CulinaryException::what() const noexcept {
    return _message.c_str();
}
