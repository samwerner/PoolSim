#include "random.h"

const char* RandomInitException::what() const throw() {
  return message;
}

RandomInitException::RandomInitException(const char* _message): message(_message) {}

double SystemRandom::drand48() {
  return ::drand48();
}

SystemRandom::SystemRandom() {}

SystemRandom& SystemRandom::getInstance() {
  if (!initialized) {
    throw RandomInitException("random not initialized");
  }
  static SystemRandom instance;
  return instance;
}

void SystemRandom::ensureInitialized(long seed) {
  if (!initialized) {
    initialize(seed);
  }
}

void SystemRandom::initialize(long seed) {
  if (initialized) {
    throw RandomInitException("random already initialized");
  }
  initialized = true;
  srand48(seed);
}

bool SystemRandom::initialized = false;
