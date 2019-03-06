#pragma once

#include <random>
#include <memory>
#include <iterator>

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

    // returns a random integer between min and max
    virtual int random_int(int min, int max) = 0;

    // returns a random element from a container
    template<typename It>
    typename std::iterator_traits<It>::reference random_element(It begin, It end);

    virtual std::shared_ptr<std::default_random_engine> get_random_engine() = 0;
};

template<typename It>
typename std::iterator_traits<It>::reference Random::random_element(It begin, It end) {
    const uint64_t n = std::distance(begin, end);
    const uint64_t divisor = (static_cast<uint64_t>(RAND_MAX) + 1) / n;

    uint64_t k;
    do { k = random_int(0, RAND_MAX) / divisor; } while (k >= n);
    std::advance(begin, k);
    return begin[0];
}


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

  int random_int(int min, int max) override;

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
  NormalDistribution(double mean, double stddev);
  NormalDistribution(double mean, double stddev, std::shared_ptr<Random> random);
  virtual double get();
private:
  std::normal_distribution<double> dist;
};

class LogNormalDistribution : public Distribution,
                              public Creatable1<Distribution, LogNormalDistribution, const nlohmann::json&> {
public:
  explicit LogNormalDistribution(const nlohmann::json& args);
  LogNormalDistribution(double mean, double stddev);
  LogNormalDistribution(double mean, double stddev, std::shared_ptr<Random> random);
  virtual double get();
private:
  std::lognormal_distribution<double> dist;
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
