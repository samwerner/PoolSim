#include "random.h"

#include <sstream>

namespace poolsim {

using nlohmann::json;


const char* RandomInitException::what() const throw() {
  return message;
}

RandomInitException::RandomInitException(const char* _message): message(_message) {}

SystemRandom::SystemRandom() :
  random_engine(std::make_shared<std::default_random_engine>()) {}

double SystemRandom::drand48() {
  return ::drand48();
}

int SystemRandom::random_int(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(*get_random_engine());
}

std::string SystemRandom::get_address() {
  std::stringstream result;
  std::uniform_int_distribution<int> dist(0, 15);
  for (size_t i = 0; i < 40; i++) {
    auto value = dist(*get_random_engine());
    result << std::hex << value;
  }
  return "0x" + result.str();
}

std::shared_ptr<std::default_random_engine> SystemRandom::get_random_engine() {
  return random_engine;
}

std::shared_ptr<SystemRandom> SystemRandom::get_instance() {
  if (!initialized) {
    throw RandomInitException("random not initialized");
  }

  static std::shared_ptr<SystemRandom> instance = std::shared_ptr<SystemRandom>(new SystemRandom);
  return instance;
}

void SystemRandom::ensure_initialized(long seed) {
  if (!initialized) {
    initialize(seed);
  }
}

void SystemRandom::initialize(long seed) {
  if (initialized) {
    throw RandomInitException("random already initialized");
  }
  initialized = true;
  srand(seed);
  srand48(seed);
  get_instance()->get_random_engine()->seed(seed);
}

bool SystemRandom::initialized = false;


Distribution::Distribution() : Distribution(SystemRandom::get_instance()) {}
Distribution::Distribution(std::shared_ptr<Random> _random)
  : random(_random) {}

NormalDistribution::NormalDistribution(double mean, double variance)
  : Distribution(), dist(std::normal_distribution<double>(mean, variance)) {}

NormalDistribution::NormalDistribution(double mean, double variance, std::shared_ptr<Random> _random)
  : Distribution(_random), dist(std::normal_distribution<double>(mean, variance)) {}

NormalDistribution::NormalDistribution(const json& args)
  : Distribution(), dist(std::normal_distribution<double>(args["mean"], args["variance"])) {}

double NormalDistribution::get() {
  return dist(*random->get_random_engine());
}

REGISTER(Distribution, NormalDistribution, "normal")

LogNormalDistribution::LogNormalDistribution(double mean, double variance)
  : Distribution(), dist(std::lognormal_distribution<double>(mean, variance)) {}

LogNormalDistribution::LogNormalDistribution(double mean, double variance, std::shared_ptr<Random> _random)
  : Distribution(_random), dist(std::lognormal_distribution<double>(mean, variance)) {}

LogNormalDistribution::LogNormalDistribution(const json& args)
  : Distribution(), dist(std::lognormal_distribution<double>(args["mean"], args["variance"])) {}

double LogNormalDistribution::get() {
  return dist(*random->get_random_engine());
}

REGISTER(Distribution, LogNormalDistribution, "lognormal")


UniformDistribution::UniformDistribution(double low, double high)
  : Distribution(), dist(std::uniform_real_distribution<double>(low, high)) {}

UniformDistribution::UniformDistribution(double low, double high, std::shared_ptr<Random> _random)
  : Distribution(_random), dist(std::uniform_real_distribution<double>(low, high)) {}

UniformDistribution::UniformDistribution(const json& args)
  : Distribution(), dist(std::uniform_real_distribution<double>(args["low"], args["high"])) {}

double UniformDistribution::get() {
  return dist(*random->get_random_engine());
}

REGISTER(Distribution, UniformDistribution, "uniform")

}
