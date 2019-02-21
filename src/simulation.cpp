#include <nlohmann/json.hpp>


#include "simulation.h"

using json = nlohmann::json;


Simulation Simulation::from_config_file(const std::string& filepath) {
  std::ifstream file_input(filepath);
  json j;
  file_input >> j;

  Simulation simulation;
  simulation.rounds = j["rounds"];

  return simulation;
}
