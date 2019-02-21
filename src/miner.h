#pragma once

#include <string>
#include <memory>

class MiningPool;

class Miner: public std::enable_shared_from_this<Miner> {
 private:
  std::string address;
  double hashrate;
  std::shared_ptr<MiningPool> pool;
  unsigned long long credits, shares;
  unsigned long blocks_mined, blocks_received, uncles_mined, uncles_received;
  
 public:
  Miner(std::string _address, double _hashrate);
  Miner(std::string _address, double _hashrate, std::shared_ptr<MiningPool> _pool);

  std::string get_address() const;
  double get_hashrate() const;
  std::shared_ptr<MiningPool> get_pool() const;


  void join_pool(std::shared_ptr<MiningPool> pool);
  void leave_pool();

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
