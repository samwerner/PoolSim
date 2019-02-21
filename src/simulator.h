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

  // Runs the simulator
  void run();

  // Schedules all the miners
  // This should only be used for the first initialization
  void schedule_all();

  // Processes an event from the queue
  // This will also schedule the next event for the miner
  void process_event(const Event& event);

  // Schedules a single miner
  void schedule_miner(const std::shared_ptr<Miner> miner);

  // Returns the miner with the given address:
  std::shared_ptr<Miner> get_miner(const std::string& miner_address);

  // Returns the current event queue
  EventQueue& get_event_queue();

  // Returns the current number of blocks mined
  uint64_t get_blocks_mined() const;

  // Returns the current time
  double get_current_time() const;

  // Adds a miner to the simulator
  void add_miner(std::shared_ptr<Miner> miner);

  // Adds a pool to the simulator
  void add_pool(std::shared_ptr<MiningPool> pool);


private:
  // Setup of the simulation to run
  Simulation simulation;

  // Interface able to return random numbers
  // used mostly for testing purposes
  std::shared_ptr<Random> random;

  // Event queue of the simulator
  EventQueue queue;

  // Pools in the current simulation
  std::vector<std::shared_ptr<MiningPool>> pools;

  // Miners in the current simulation
  std::map<std::string, std::shared_ptr<Miner>> miners;

  // Current time
  double current_time;

  // Total number of blocks mined
  uint64_t blocks_mined;
};
