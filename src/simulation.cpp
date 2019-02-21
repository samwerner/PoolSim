#include <nlohmann/json.hpp>

#include <fstream>
#include <sstream>

#include "simulation.h"

using json = nlohmann::json;

void from_json(const json& j, MinerConfig& miner_config) {
  j.at("generator").get_to(miner_config.generator);
  miner_config.arguments = j["arguments"];
}

void from_json(const json& j, PoolConfig& pool_config) {
  j.at("difficulty").get_to(pool_config.difficulty);
  j.at("miners").get_to(pool_config.miner_config);
}

void from_json(const json& j, Simulation& simulation) {
  j.at("blocks").get_to(simulation.blocks);
  j.at("networkDifficulty").get_to(simulation.network_difficulty);
  j.at("pools").get_to(simulation.pools);
}

Simulation Simulation::from_stream(std::istream& stream) {
  json j;
  stream >> j;
  return j.get<Simulation>();
}

Simulation Simulation::from_config_file(const std::string& filepath) {
  std::ifstream file_input(filepath);
  return from_stream(file_input);
}

Simulation Simulation::from_string(const std::string& string) {
  std::stringstream stream(string);
  return from_stream(stream);
}
