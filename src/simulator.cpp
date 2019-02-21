#include <fstream>
#include <iostream>

#include <spdlog/spdlog.h>

#include "simulator.h"
#include "miner.h"
#include "event.h"

Simulator::Simulator(Simulation _simulation)
  : Simulator(_simulation, std::shared_ptr<SystemRandom>(&SystemRandom::getInstance())) {}

Simulator::Simulator(Simulation _simulation, std::shared_ptr<Random> _random)
  : simulation(_simulation), random(_random), current_time(0), blocks_mined(0) {}

void Simulator::run() {
  spdlog::info("running {} blocks", simulation.blocks);
  // TODO: initialize the simulator

  schedule_all();

  while (get_blocks_mined() <= simulation.blocks) {
    auto event = queue.pop();
    process_event(event);
  }
}

EventQueue& Simulator::get_event_queue() {
  return queue;
}

double Simulator::get_current_time() const {
  return current_time;
}

uint64_t Simulator::get_blocks_mined() const {
  return blocks_mined;
}

void Simulator::schedule_all() {
  for (auto miner_kv: miners) {
    schedule_miner(miner_kv.second);
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
  if (is_network_share) {
    blocks_mined++;
    spdlog::debug("new block mined: {} / {}", blocks_mined, simulation.blocks);
  }
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

void Simulator::add_miner(std::shared_ptr<Miner> miner) {
  miners[miner->get_address()] = miner;
}

void Simulator::add_pool(std::shared_ptr<MiningPool> pool) {
  pools.push_back(pool);
}
