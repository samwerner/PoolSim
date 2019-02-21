#pragma once

#include <string>
#include <cstdint>

#include "simulation.h"

class Simulator {
public:
  static Simulator from_config_file(const std::string& filepath);

  Simulator(Simulation simulation);

  void run();

private:
  Simulation simulation;
};
