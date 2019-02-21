#pragma once

#include <string>
#include <vector>
#include <map>

#include "miner.h"
#include "mining_pool.h"
#include "simulation.h"
#include "event_queue.h"
#include "event.h"
#include "random.h"

class Simulator {
public:
  Simulator(Simulation simulation);
  Simulator(Simulation simulation, std::shared_ptr<Random> random);

  void run();

  void schedule_all();
  void schedule_miner(const std::shared_ptr<Miner> miner);
  void process_event(const Event& event);
  std::shared_ptr<Miner> get_miner(const std::string& miner_address);

  EventQueue& get_event_queue();
  double get_current_time() const;

  void add_miner(std::shared_ptr<Miner> miner);
  void add_pool(std::shared_ptr<MiningPool> pool);

private:
  Simulation simulation;
  std::shared_ptr<Random> random;
  EventQueue queue;
  std::vector<std::shared_ptr<MiningPool>> pools;
  std::map<std::string, std::shared_ptr<Miner>> miners;
  double current_time;
};
