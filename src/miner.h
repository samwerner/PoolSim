#pragma once

#include <string>
#include <memory>

#include "share.h"
#include "mining_pool.h"
#include "share_handler.h"


class Miner: public std::enable_shared_from_this<Miner> {
 private:
  std::string address;
  double hashrate;
  std::weak_ptr<MiningPool> pool;
  unsigned long long credits, shares;
  unsigned long blocks_mined, blocks_received, uncles_mined, uncles_received;

  std::unique_ptr<ShareHandler> share_handler;
  
 public:
  Miner(std::string _address, double _hashrate);
  Miner(std::string _address, double _hashrate, std::shared_ptr<MiningPool> _pool);
  virtual ~Miner() {}

  std::string get_address() const;
  double get_hashrate() const;
  std::shared_ptr<MiningPool> get_pool() const;

  // Processes the share by delegating to different strategies
  virtual void process_share(const Share& share);

  // Joins the given pool, updates the state of the pool too
  void join_pool(std::shared_ptr<MiningPool> pool);

  // sets credit balance of miner to _credits
  void set_credits(unsigned long long _credits);

  // increments balance of blocks mined by miner
  void inc_blocks_mined();

  // increments balance of total blocks received by mined
  void inc_blocks_received();

  // increments total balance of uncles mined by miner
  void inc_uncles_mined();

  // increments total balance of uncles received by miner
  void inc_uncles_received();

};
