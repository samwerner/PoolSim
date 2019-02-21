#include <fstream>
#include <iostream>

#include <spdlog/spdlog.h>

#include "simulator.h"
#include "miner.h"

Simulator::Simulator(Simulation _simulation) : simulation(_simulation) {}

void Simulator::run() {
  spdlog::info("running {} rounds", simulation.rounds);
}

void Simulator::schedule_all() {
  for (auto pool: pools) {
    for (auto miner_address: pool->get_miners()) {
      schedule_miner(miner_address);
    }
  }
}

std::shared_ptr<Miner> Simulator::get_miner(const std::string& miner_address) {
  return miners[miner_address];
}


void Simulator::run_miner(const std::string& miner_address) {
  auto miner = get_miner(miner_address);
  // check if is network share
  // create share
  // ask miner to handle share
  // schedule next share for miner
}

void Simulator::schedule_miner(const std::string& miner_address) {
  auto miner = get_miner(miner_address);
  // schedule next share for miner
    // assert(queue != NULL);

    // if (hashrate <= 0) {
    //     return false;
    // }

    // double lambda = (double) hashrate / pool->getShareDiff();
    // double t = -log(drand48()) / lambda;
    // double p = (double) pool->getShareDiff() / pool->getNetDiff();

    // // TODO: allow
    // // minerBehavior.processShare(self, pool, isValidProof)

    // if (drand48() < p) {
    //     queue->schedule(new NetworkShareEvent(this, queue->getTime() + t));
    //     return true;
    // } else {
    //   queue->schedule(new MiningPoolShareEvent(this, queue->getTime() + t));
    //   return true;
    // }
}
