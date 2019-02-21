#pragma once

#include <memory>

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
  virtual double drand48() = 0;
};


// Singleton which delegates to
// standard library
class SystemRandom : public Random {
public:
  static void initialize(long seed);
  static void ensureInitialized(long seed);
  static SystemRandom& getInstance();

  // Delegates to standard drand48()
  double drand48() override;

  // Avoid accidental copies
  SystemRandom(SystemRandom const&) = delete;
  void operator=(SystemRandom const&) = delete;
private:
  static bool initialized;
  SystemRandom();
};
