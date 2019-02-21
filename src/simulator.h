#pragma once

#include <string>
#include <vector>
#include <map>

#include "miner.h"
#include "mining_pool.h"
#include "simulation.h"

class Simulator {
public:
  Simulator(Simulation simulation);

  void run();

  void schedule_all();
  void schedule_miner(const std::string& miner_address);
  void run_miner(const std::string& miner_address);
  std::shared_ptr<Miner> get_miner(const std::string& miner_address);

private:
  Simulation simulation;
  std::vector<std::shared_ptr<MiningPool>> pools;
  std::map<std::string, std::shared_ptr<Miner>> miners;
};
