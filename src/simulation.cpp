#include <nlohmann/json.hpp>

#include <fstream>
#include <sstream>

#include "simulation.h"

using json = nlohmann::json;

Simulation Simulation::from_stream(std::istream& stream) {
  json j;
  stream >> j;

  Simulation simulation;
  simulation.rounds = j["rounds"];
  simulation.network_difficulty = j["networkDifficulty"];

  return simulation;
}


Simulation Simulation::from_config_file(const std::string& filepath) {
  std::ifstream file_input(filepath);
  return from_stream(file_input);
}

Simulation Simulation::from_string(const std::string& string) {
  std::stringstream stream(string);
  return from_stream(stream);
}
