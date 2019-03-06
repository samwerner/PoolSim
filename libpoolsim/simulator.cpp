#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cassert>

#include <spdlog/spdlog.h>

#include "simulator.h"
#include "network.h"
#include "reward_scheme.h"
#include "miner.h"
#include "event.h"
#include "miner_creator.h"

namespace poolsim {

using nlohmann::json;


Simulator::Simulator(Simulation _simulation)
    : Simulator(_simulation, SystemRandom::get_instance()) {}

Simulator::Simulator(Simulation _simulation, std::shared_ptr<Random> _random)
    : simulation(_simulation), network(std::make_shared<Network>(_simulation.network_difficulty)),
      random(_random), current_time(0), blocks_mined(0) {}

std::shared_ptr<Simulator> Simulator::from_config_file(const std::string& filepath) {
    auto simulation = Simulation::from_config_file(filepath);
    SystemRandom::initialize(simulation.seed);
    spdlog::debug("initialized random with seed {}", simulation.seed);

    return  std::shared_ptr<Simulator>(new Simulator(simulation));
}


void Simulator::initialize() {
    for (size_t i = 0; i < simulation.pools.size(); i++) {
        auto pool_config = simulation.pools[i];
        auto miner_config = pool_config.miner_config;
        auto reward_scheme_config = pool_config.reward_scheme_config;
        auto miner_creator = MinerCreatorFactory::create(miner_config.generator, network);
        auto new_miners = miner_creator->create_miners(miner_config.params);
        auto reward_scheme = RewardSchemeFactory::create(reward_scheme_config.scheme_type,
                                                        reward_scheme_config.params);
        std::string pool_name = pool_config.name;
        if (pool_name.empty()) {
            std::stringstream s;
            s << "pool-" << i;
            pool_name = s.str();
        }
        
        assert(pool_config.difficulty);
        auto pool = MiningPool::create(pool_name,
                                       pool_config.difficulty,
                                       pool_config.uncle_block_prob,
                                       std::move(reward_scheme),
                                       network);
        network->register_pool(pool);
        pool->add_observer(shared_from_this());
        for (auto miner : new_miners) {
            miner->join_pool(pool);
            add_miner(miner);
        }
        add_pool(pool);
    }
}

void Simulator::run() {
  initialize();

  spdlog::debug("loaded {} pools with a total of {} miners", pools.size(), miners.size());

  if (pools.empty() || miners.empty()) {
    throw InvalidSimulationException("simulation must have at least one miner and one pool");
  }

  schedule_all();

  spdlog::info("running {} blocks", simulation.blocks);
  while (get_blocks_mined() < simulation.blocks) {
    auto event = queue.pop();
    process_event(event);
  }
}

void Simulator::save_simulation_data(const std::string& filepath) {
    json result;

    result["blocks"] = json::array();
    for (auto block : block_events) {
        result["blocks"].push_back(block);
    }

    result["pools"] = json::array();
    for (auto pool : pools) {
        result["pools"].push_back(*pool);
    }

    result["miners"] = json::array();
    for (auto miner_kv : miners) {
        result["miners"].push_back(*miner_kv.second);
    }

    std::ofstream o(filepath);
    o << std::setw(4) << result << std::endl;
}

void Simulator::schedule_all() {
  for (auto miner_kv: miners) {
    schedule_miner(miner_kv.second);
  }
}

void Simulator::process_event(const Event& event) {
  current_time = event.time;
  auto miner = get_miner(event.miner_address);
  auto pool = miner->get_pool();
  double p = (double) pool->get_difficulty() / simulation.network_difficulty;
  bool is_network_share = random->drand48() < p;
  uint8_t share_flags = Share::Property::none;
  if (is_network_share) {
    blocks_mined++;
    if (blocks_mined % 100 == 0) {
      spdlog::debug("progress: {} / {}", blocks_mined, simulation.blocks);
    }
    share_flags |= Share::Property::valid_block;
  }
  Share share(share_flags);
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

std::shared_ptr<Miner> Simulator::get_miner(const std::string& miner_address) {
  return miners[miner_address];
}

std::shared_ptr<Network> Simulator::get_network() const {
    return network;
}

double Simulator::get_current_time() const {
  return current_time;
}

uint64_t Simulator::get_blocks_mined() const {
  return blocks_mined;
}

size_t Simulator::get_miners_count() const {
  return miners.size();
}

size_t Simulator::get_pools_count() const {
  return pools.size();
}

size_t Simulator::get_events_count() const {
  return queue.size();
}

Event Simulator::get_next_event() const {
  return queue.get_top();
}

void Simulator::process(const BlockEvent& block_event) {
    BlockEvent block_event_copy = block_event;
    block_event_copy.time = current_time;
    block_events.push_back(block_event_copy);
}

}
