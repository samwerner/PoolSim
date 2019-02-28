#include "factory.h"

namespace poolsim {

NotRegisteredException::NotRegisteredException(const std::string& _name)
  : name(_name) {}

char const* NotRegisteredException::what() const throw() {
  return (name + "not registered").c_str();
}

}
