#include <iostream>

#include "mining_pool.h"
#include "miner.h"


MiningPool::MiningPool() {}

MiningPool::MiningPool(std::string miner_file) {
  // populate pool with miners from file
}

void MiningPool::join(std::shared_ptr<Miner> miner) {
  miners.push_back(miner);
}


void MiningPool::network_share(Miner *miner) {
  
}

void MiningPool::pool_share(Miner *miner) {
  
}
