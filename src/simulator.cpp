#include <fstream>
#include <iostream>

#include <spdlog/spdlog.h>

#include "simulator.h"
#include "miner.h"
#include "event.h"

Simulator::Simulator(Simulation _simulation)
  : Simulator(_simulation, std::shared_ptr<SystemRandom>(&SystemRandom::getInstance())) {}

Simulator::Simulator(Simulation _simulation, std::shared_ptr<Random> _random)
  : simulation(_simulation), random(_random) {}

void Simulator::run() {
  spdlog::info("running {} rounds", simulation.rounds);
}

EventQueue& Simulator::get_event_queue() {
  return queue;
}

double Simulator::get_current_time() const {
  return current_time;
}

void Simulator::schedule_all() {
  for (auto pool: pools) {
    for (auto miner_address: pool->get_miners()) {
      auto miner = miners[miner_address];
      schedule_miner(miner);
    }
  }
}

std::shared_ptr<Miner> Simulator::get_miner(const std::string& miner_address) {
  return miners[miner_address];
}

void Simulator::process_event(const Event& event) {
  current_time = event.time;
  auto miner = get_miner(event.miner_address);
  auto pool = miner->get_pool();
  double p = (double) pool->get_difficulty() / simulation.network_difficulty;
  bool is_network_share = random->drand48() < p;
  Share share(is_network_share);
  schedule_miner(miner);
  miner->process_share(share);
}

void Simulator::schedule_miner(const std::shared_ptr<Miner> miner) {
  auto pool = miner->get_pool();
  double lambda = miner->get_hashrate() / pool->get_difficulty();
  double t = -log(random->drand48()) / lambda;

  Event miner_next_event(miner->get_address(), current_time + t);
  queue.schedule(miner_next_event);
}
