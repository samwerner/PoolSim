#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>

#include "miner.h"
#include "share.h"

class MiningPool {
 private:
  // list of miners in pool
  std::map<std::string, std::weak_ptr<Miner>> miners;

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
  MiningPool(const std::string& miner_file);

  // Returns all the miners currently in the pool
  std::vector<std::weak_ptr<Miner>> get_miners();

  size_t get_miners_count() const;

  // Submit a share to the pool
  // The share can be either a network share or a pool share
  void submit_share(const std::string& miner_address, const Share& share);

  // Joins this mining pool
  // This method does not update the miner state
  void join(std::shared_ptr<Miner> miner);

  // Leaves this mining pool
  // This method does not update the miner state
  void leave(const std::string& miner_address);
};
