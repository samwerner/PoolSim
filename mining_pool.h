#pragma once

#include <vector>
#include <string>
#include <memory>

#include "miner.h"

class MiningPool {
 private:
  // list of miners in pool
  std::vector<std::shared_ptr<Miner>> miners;

  // share and network difficulty; total hashrate of pool
  unsigned long long share_diff, net_diff, hashrate;

  // probability that a block is an uncle
  double uncle_prob;

  // reward scheme used by pool for distributing block rewards among miners
  

  // total blocks mined by miners in pool
  unsigned long blocks_mined;
  
 public:
  MiningPool();

  // populate pool with miners from .csv file 'miner_file'
  MiningPool(std::string miner_file);

  // network share submitted by a miner in the pool
  void network_share(Miner *miner);

  // share submitted to pool by a miner in the pool
  void pool_share(Miner *miner);

  friend class Queue_Based;

};
