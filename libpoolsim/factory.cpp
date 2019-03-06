#include "factory.h"

#include <cstring>

namespace poolsim {

NotRegisteredException::NotRegisteredException(const std::string& _name)
  : name(_name) {}

char const* NotRegisteredException::what() const throw() {
    std::string message = name + " is not registered";
    char* result = new char[message.size() + 1];
    std::strncpy(result, message.c_str(), message.size());
    return result;
}

}
