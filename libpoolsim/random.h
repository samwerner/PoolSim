#pragma once

#include <random>
#include <memory>

#include "factory.h"

#include <nlohmann/json.hpp>

namespace poolsim {

// Exception thrown if the Random class
// has not yet been initialized exactly once
class RandomInitException : public std::exception {
public:
  explicit RandomInitException(const char* _message);
  const char* what() const throw();
private:
  const char* message;
};


// Interface used for mocking
class Random {
public:
  virtual ~Random() {}
  // Returns a random double between 0 and 1
  virtual double drand48() = 0;

  // Returns a random address (0x prefix + 40 hex chars)
  virtual std::string get_address() = 0;

  virtual std::shared_ptr<std::default_random_engine> get_random_engine() = 0;
};


// Singleton which delegates to
// standard library
class SystemRandom : public Random {
public:
  static void initialize(long seed);
  static void ensure_initialized(long seed);
  static std::shared_ptr<SystemRandom> get_instance();

  std::string get_address() override;

  // Delegates to standard drand48()
  double drand48() override;

  std::shared_ptr<std::default_random_engine> get_random_engine();

  // Avoid accidental copies
  SystemRandom(SystemRandom const&) = delete;
  void operator=(SystemRandom const&) = delete;
private:
  static bool initialized;
  std::shared_ptr<std::default_random_engine> random_engine;
  SystemRandom();
};

// Base class for a probability distribution
class Distribution {
public:
  Distribution();
  explicit Distribution(std::shared_ptr<Random> random);
  virtual double get() = 0;
protected:
  std::shared_ptr<Random> random;
};

MAKE_FACTORY(DistributionFactory, Distribution, const nlohmann::json&)


class NormalDistribution : public Distribution,
                           public Creatable1<Distribution, NormalDistribution, const nlohmann::json&> {
public:
  explicit NormalDistribution(const nlohmann::json& args);
  NormalDistribution(double mean, double variance);
  NormalDistribution(double mean, double variance, std::shared_ptr<Random> random);
  virtual double get();
private:
  std::normal_distribution<double> dist;
};

class UniformDistribution : public Distribution,
                            public Creatable1<Distribution, UniformDistribution, const nlohmann::json&> {
public:
  explicit UniformDistribution(const nlohmann::json& args);
  UniformDistribution(double low, double high);
  UniformDistribution(double low, double high, std::shared_ptr<Random> random);
  virtual double get();
private:
  std::uniform_real_distribution<double> dist;
};

}
