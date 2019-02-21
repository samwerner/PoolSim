#pragma once

#include <memory>

class RandomInitException : public std::exception {
public:
  explicit RandomInitException(const char* _message);
  const char* what() const throw();
private:
  const char* message;
};

class Random {
public:
  virtual ~Random() {}
  virtual double drand48() = 0;
};


class SystemRandom : public Random {
public:
  static void initialize(long seed);
  static void ensureInitialized(long seed);
  static SystemRandom& getInstance();
  double drand48() override;

  SystemRandom(SystemRandom const&) = delete;
  void operator=(SystemRandom const&) = delete;
private:
  static bool initialized;
  SystemRandom();
};
